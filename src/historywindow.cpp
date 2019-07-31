#include "historywindow.h"
#include "ui_historywindow.h"
#include "components/flowlayout.h"
#include "components/previewbox.h"
#include "components/tagbutton.h"
#include "application.h"

#include <QDebug>
#include <QRandomGenerator>
#include <QCheckBox>
#include <stdio.h>

HistoryWindow::HistoryWindow() :
    QMainWindow(),
    ui(new Ui::HistoryWindow)
{
    ui->setupUi(this);

    ui->dteFrom->setDate(QDate::currentDate().addMonths(-1));
    ui->dteTo->setDate(QDate::currentDate());

    for (auto &server : APP->serverService()->getAll()) {
        auto serverCtrl = new QCheckBox(ui->sclServers);
        serverCtrl->setText(server.name);
        serverCtrl->setProperty("serverId", server.id);
        serverCtrl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        ui->sclServers->layout()->addWidget(serverCtrl);
    }
    ui->sclServers->layout()->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    reload();

    connect(ui->btnPaste, SIGNAL(clicked()), this, SLOT(reloadImage()));
    connect(ui->btnApply, SIGNAL(clicked()), this, SLOT(reload()));
    connect(ui->btnClean, SIGNAL(clicked()), this, SLOT(cleanAll()));
    connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(deleteSelected()));
    connect(ui->btnReload, SIGNAL(clicked()), this, SLOT(reload()));
    connect(ui->btnDeselect, SIGNAL(clicked()), this, SLOT(deselectAll()));
    connect(ui->sclClips, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));
}

HistoryWindow::~HistoryWindow()
{
    delete ui;
}

bool HistoryWindow::confirm(const QString &message)
{
    return QMessageBox::question(this, tr("Confirmation"), message, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes;
}

void HistoryWindow::reloadImage()
{
    auto clips = APP->clipService()->getAllFromClipboard();
    if (clips.isEmpty() || clips.first().isImage == false)
        return;

    ui->tnlImage->setPixmap(clips.first().thumbnailPixmap());
}


void HistoryWindow::reload()
{
    QMap<QString, QVariant> filter;
    if (ui->grpByDate->isChecked()) {
        filter["dateFrom"] = ui->dteFrom->date();
        filter["dateTo"] = ui->dteTo->date();
    }
    if (ui->grpByServers->isChecked()) {
        QList<uint> serverIds;
        for (auto &serverCtrl : ui->sclServers->findChildren<QCheckBox*>()) {
            if (serverCtrl->isChecked()) {
                auto serverId = serverCtrl->property("serverId").toUInt();
                serverIds.append(serverId);
            }
        }
        filter["serverIds"] = QVariant::fromValue(serverIds);
    }

    if (ui->grpTags->isChecked()) {
        filter["tags"] = ui->tgeTags->tags();
    }

    if (ui->grpByImage->isChecked()) {
        filter["image"] = ui->tnlImage->pixmap()->toImage();
        filter["threshold"] = ui->sldThreshold->value();
    }

    QLayout *layout = ui->sclClips->layout();

    // clean up old widgets
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // rebuild previews
    auto datedClips = APP->clipService()->searchAndGroup(filter);
    for (auto &pair : datedClips) {
        auto date = pair.first;
        QLabel *dateLabel = new QLabel(this);
        dateLabel->setText(date.toString("yyyy-MM-dd"));
        dateLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        dateLabel->setStyleSheet("font-size: 24px; font: bold; border-bottom: 1px solid black;");
        dateLabel->setMargin(10);
        layout->addWidget(dateLabel);

        QFrame *groupedFrame = new QFrame(this);
        FlowLayout *groupedLayout = new FlowLayout(groupedFrame);
        groupedLayout->setMargin(0);
        groupedFrame->setLayout(groupedLayout);
        for (auto &clip: pair.second) {
            auto preview = new PreviewBox(this);
            preview->setProperty("clipId", clip.id);
            preview->setImage(clip.thumbnailPixmap());
            preview->setTags(clip.tags);
            preview->setName(clip.name);
            groupedLayout->addWidget(preview);

            connect(preview, &PreviewBox::clicked, [&]() {
                QList<PreviewBox*> selected;
                for (auto &p : ui->sclClips->findChildren<PreviewBox*>()) {
                    if (p->isSelected())
                        selected.append(p);
                }
                if (selected.length() == 2) {
                    uint distance = 0;
                    auto a = selected.front();
                    auto b = selected.back();
                    for (int i = 0; i < 10000; i++) {
                        distance += (a->bits[i] - b->bits[i])^2;
                    }
                    qDebug() << "distance : " << distance;
                }
            });
        }
        layout->addWidget(groupedFrame);
    }
}

void HistoryWindow::cleanAll()
{
    if (confirm(tr("Are you sure you want to delete all history PERMANENTLY?"))) {
        APP->clipService()->clean();
        reload();
    }
}

void HistoryWindow::deleteSelected()
{
    if (confirm(tr("Are you sure you want to delete all selected clips?"))) {
        for (auto &preview : ui->sclClips->findChildren<PreviewBox*>()) {
            if (preview->isSelected()) {
                APP->clipService()->remove(preview->property("clipId").toUInt());
            }
        }
        reload();
    }
}

void HistoryWindow::deselectAll()
{
    for (auto &preview : ui->sclClips->findChildren<PreviewBox*>()) {
        preview->toggle(false);
    }
}

void HistoryWindow::deleteClip(uint clipId)
{
    if (confirm(tr("Are you sure you want to delete this clip?"))) {
        APP->clipService()->remove(clipId);
        reload();
    }
}


void HistoryWindow::showContextMenu(const QPoint &pos)
{
    qDebug() << pos;

    PreviewBox *preview = nullptr;
    QWidget* child = ui->sclClips->childAt(pos);
    while (child) {
        preview = dynamic_cast<PreviewBox*>(child);
        if (preview)
            break;
        child = child->parentWidget();
    }
    if (!preview)
        return;

    preview->toggle(true);

    QMenu contextMenu(this);

    auto formats = APP->formatService()->getAll();

    uint clipId = preview->property("clipId").toUInt();
    QList<QObject*> pointers;
    for (auto &upload : APP->uploadService()->getAllByClipId(clipId)) {
        // stack will be released outside the loop
        auto serverMenu = new QMenu(upload.serverName);
        pointers.append(serverMenu);

        for (auto &format : formats) {
            auto outputAs = new QAction(tr("Copy as %1").arg(format.name));
            serverMenu->addAction(outputAs);
            connect(outputAs, &QAction::triggered, [&]() {
                Clip clip = APP->clipService()->findById(clipId);
                APP->clipService()->setClipboard(format.generate(upload.url, clip.description));
            });
            pointers.append(outputAs);
        }

        contextMenu.addMenu(serverMenu);
    }

    contextMenu.addSeparator();

    QAction editAction(tr("&Edit"), this);
    connect(&editAction, &QAction::triggered, [this, clipId]() {
        auto editDialog = new UploadDialog(this, clipId);
        editDialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(editDialog, SIGNAL(accepted()), this, SLOT(reload()));
        editDialog->show();
    });
    contextMenu.addAction(&editAction);

    QAction deleteAction(tr("&Delete this"), this);
    connect(&deleteAction, &QAction::triggered, [&]() {
        this->deleteClip(clipId);
    });
    contextMenu.addAction(&deleteAction);
    contextMenu.exec(ui->sclClips->mapToGlobal(pos));

    for (auto &pointer : pointers)
        delete pointer;
}
