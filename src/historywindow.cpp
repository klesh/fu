#include "historywindow.h"
#include "ui_historywindow.h"
#include "components/flowlayout.h"
#include "components/previewbox.h"
#include "components/tagbutton.h"
#include "application.h"

#include <QDebug>
#include <QRandomGenerator>
#include <QCheckBox>


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

    reload();

    connect(ui->btnApply, SIGNAL(clicked()), this, SLOT(reload()));
    connect(ui->btnClean, SIGNAL(clicked()), this, SLOT(cleanAll()));
    connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(deleteSelected()));
    connect(ui->btnReload, SIGNAL(clicked()), this, SLOT(reload()));
    connect(ui->sclClips, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));

    /*
    for (auto &server : APP->serverService()->getAllByClipId(9)) {
        qDebug() << "server name: " << server.name;
    }
    */
}

HistoryWindow::~HistoryWindow()
{
    delete ui;
}

bool HistoryWindow::confirm(const QString &message)
{
    return QMessageBox::question(this, tr("Confirmation"), message, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes;
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
        filter["image"] = ui->tnlImage->pixmap();
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
    for (const auto &pair : datedClips) {
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
            QPixmap thumbnail;
            thumbnail.loadFromData(clip.rawPngThumb, "PNG");
            preview->setImage(thumbnail);
            preview->setTags(clip.tags);
            preview->setName(clip.name);
            groupedLayout->addWidget(preview);
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

    auto outputFormats = APP->outputFormatService()->getAll();

    uint clipId = preview->property("clipId").toUInt();
    QList<QObject*> pointers;
    for (auto &upload : APP->uploadService()->getAllByClipId(clipId)) {
        auto server = APP->serverService()->findById(upload.serverId);
        auto serverMenu = new QMenu(server.name);
        pointers.append(serverMenu);

        for (auto &outputFormat : outputFormats) {
            auto outputAs = new QAction(tr("Copy as %1").arg(outputFormat.name));
            serverMenu->addAction(outputAs);
            connect(outputAs, &QAction::triggered, [=]() {
                Clip clip = APP->clipService()->findById(clipId);
                APP->clipService()->setClipboard(OutputFormatService::format(outputFormat, clip, upload));
            });
            pointers.append(outputAs);
        }

        contextMenu.addMenu(serverMenu);
    }

    contextMenu.addSeparator();

    QAction editAction(tr("&Edit"), this);
    connect(&editAction, &QAction::triggered, [this]() {
        (new UploadDialog(this))->show();
    });
    contextMenu.addAction(&editAction);

    QAction deleteAction(tr("&Delete this"), this);
    connect(&deleteAction, &QAction::triggered, [=]() {
        this->deleteClip(clipId);
    });
    contextMenu.addAction(&deleteAction);
    contextMenu.exec(ui->sclClips->mapToGlobal(pos));

    for (auto &pointer : pointers)
        delete pointer;
}
