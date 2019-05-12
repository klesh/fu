#include "historywindow.h"
#include "ui_historywindow.h"
#include "components/flowlayout.h"
#include "components/previewbox.h"
#include "models/record.h"
#include "components/tagbutton.h"

#include <QDebug>
#include <QRandomGenerator>

HistoryWindow &HistoryWindow::getInstance()
{
    static HistoryWindow instance;
    return instance;
}

HistoryWindow::HistoryWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HistoryWindow)
{
    ;
    setWindowIcon(QIcon(":/icons/icon.png"));
    ui->setupUi(this);


    // mock records
    QList<Record> records;
    const static QPixmap thumbnail = QPixmap("D:/Nextcloud/kleshwong/wallpapers/6fVBDMW-dark-minimalist-wallpaper.jpg").scaled(160, 160, Qt::KeepAspectRatio);
    for (int i = 0; i < 100; i++)
    {

        Record record;
        record.setUploadedTo("imgur.com");
        record.setThumbnail(thumbnail);
        record.setTags({"Mr.Robot", "Hello", "World"});
        QDateTime createdAt = QDateTime::currentDateTime().addDays(-QRandomGenerator::global()->bounded(0, 5));
        record.setCreatedAt(createdAt);
        records.append(record);
    }
    updateRecords(records);

    // set up context menu
    connect(ui->sclRecords, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showRecordsContextMenu(const QPoint &)));
}

HistoryWindow::~HistoryWindow()
{
    delete ui;
}

void HistoryWindow::closeEvent(QCloseEvent *evt)
{
    hide();
    evt->ignore();
}

void HistoryWindow::updateRecords(const QList<Record> &records)
{
    QLayout *recordsLayout = ui->sclRecords->layout();

    // clean up old widgets
    QLayoutItem *item;
    while ((item = recordsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // rebuild new widgets
    QMap<QDate, QList<Record>> dateRecordsMap = Record::groupByCreationDate(records);
    for (const QDate &date : dateRecordsMap.keys()) {
        QLabel *dateLabel = new QLabel(this);
        dateLabel->setText(date.toString("yyyy-MM-dd"));
        dateLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        dateLabel->setStyleSheet("font-size: 24px; font: bold; border-bottom: 1px solid black;");
        dateLabel->setMargin(10);
        recordsLayout->addWidget(dateLabel);

        QFrame *groupedFrame = new QFrame(this);
        FlowLayout *groupedLayout = new FlowLayout(groupedFrame);
        groupedLayout->setMargin(0);
        groupedFrame->setLayout(groupedLayout);
        for (const Record &record : dateRecordsMap[date]) {
            PreviewBox *box = new PreviewBox(this);
            box->setUploadedTo(record.getUploadedTo());
            box->setImage(record.getThumbnail());
            box->setTags(record.getTags());
            groupedLayout->addWidget(box);
        }
        recordsLayout->addWidget(groupedFrame);
    }
}

void HistoryWindow::showRecordsContextMenu(const QPoint &pos)
{
    qDebug() <<"hello";
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

    contextMenu.exec(ui->sclRecords->mapToGlobal(pos));
}
