#include "historywindow.h"
#include "ui_historywindow.h"
#include "components/flowlayout.h"
#include "components/previewbox.h"

#include <QDebug>

const QString HistoryWindow::TAG_STYLE_DEFAULT = "border-radius:5px;\nborder-style:solid;\nborder-width:1px;\nborder-color: #999;\nbackground: #eee;\npadding: 3px 5px;\ncolor: #999;";
const QString HistoryWindow::TAG_STYLE_ACTIVE = "border-radius:5px;\nborder-style:solid;\nborder-width:1px;\nborder-color: #333;\nbackground: #00ff00;\npadding: 3px 5px;";
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

    FlowLayout *tagsLayout = new FlowLayout(ui->sclTags, 10);
    FlowLayout *recordsLayout = new FlowLayout(ui->sclRecords, 10);

    for (int i = 0; i < 100; i++)
    {
        QPushButton *btn = new QPushButton(ui->sclTags);
        btn->setText("hello" + QString("Hello%1").arg(i));
        if (i % 5 == 0) {
            btn->setStyleSheet(TAG_STYLE_ACTIVE);
        } else {
            btn->setStyleSheet(TAG_STYLE_DEFAULT);
        }
        tagsLayout->addWidget(btn);


        PreviewBox *box = new PreviewBox(ui->sclRecords);
        box->setImage("D:/Nextcloud/kleshwong/wallpapers/6fVBDMW-dark-minimalist-wallpaper.jpg");
        box->setUploadedTo("imgur.com");
        box->setTags({"fsociety", "Mr.Robot", "Maching Learning"});

        recordsLayout->addWidget(box);
    }
    ui->sclTags->setLayout(tagsLayout);
    ui->sclRecords->setLayout(recordsLayout);
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
