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
        ui->sclServers->layout()->addWidget(serverCtrl);
    }

    reload();

    auto test = new QAction(this);
    test->setText("test");
    addAction(test);
    connect(ui->btnApply, SIGNAL(clicked()), this, SLOT(reload()));
    connect(ui->sclClips, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showClipsContextMenu(const QPoint &)));
}

HistoryWindow::~HistoryWindow()
{
    delete ui;
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

    // rebuild new widgets
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
            PreviewBox *box = new PreviewBox(this);
            QPixmap thumbnail;
            thumbnail.loadFromData(clip.rawPngThumb, "PNG");
            box->setImage(thumbnail);
            box->setTags(clip.tags);
            box->setName(clip.name);
            groupedLayout->addWidget(box);
        }
        layout->addWidget(groupedFrame);
    }
}

void HistoryWindow::showClipsContextMenu(const QPoint &pos)
{
    QMenu contextMenu(this);
    QAction copyUrlAction(tr("Copy as ") + "Plain Url", this);
    contextMenu.addAction(&copyUrlAction);

    QAction copyAsMarkdownImage(tr("Copy as ") + "Markdown Image", this);
    contextMenu.addAction(&copyAsMarkdownImage);

    QAction copyAsMarkdownLink(tr("Copy as ") + "Markdown Link", this);
    contextMenu.addAction(&copyAsMarkdownLink);

    contextMenu.addSeparator();

    QAction editAction(tr("&Edit"), this);
    contextMenu.addAction(&editAction);

    contextMenu.addSeparator();

    QAction deleteAction(tr("&Delete"), this);
    contextMenu.addAction(&deleteAction);

    QAction clearAction(tr("&Clean Up History"), this);
    contextMenu.addAction(&clearAction);

    contextMenu.addSeparator();

    QAction selectAllAction(tr("Select &All"), this);
    contextMenu.addAction(&selectAllAction);

    QAction unselectAllAction(tr("&Unselect All"), this);
    contextMenu.addAction(&unselectAllAction);

    contextMenu.exec(ui->sclClips->mapToGlobal(pos));
}
