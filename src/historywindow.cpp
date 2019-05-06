#include "historywindow.h"
#include "ui_historywindow.h"
#include "components/flowlayout.h"
#include "components/qtag.h"

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
    const static QPixmap test_pixmap = QPixmap(":/icons/icon.png");

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

        QFrame *box = new QFrame(ui->sclRecords);
        QVBoxLayout *boxLayout = new QVBoxLayout(box);
        box->setLayout(boxLayout);
        box->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        box->setMaximumSize(200, 200);
        box->setMinimumSize(120, 120);
        box->setObjectName("box");
        box->setStyleSheet("#box{border-style: solid;border-width: 1px;border-color: #eee}");

        QLabel *img = new QLabel(box);
        img->setPixmap(test_pixmap);
        img->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        img->setMaximumSize(180, 180);
        img->setScaledContents(true);
        boxLayout->addWidget(img);

        QLabel *server = new QLabel(box);
        server->setText(tr("Uploaded to: ") + "haha");
        server->setAlignment(Qt::AlignHCenter);
        boxLayout->addWidget(server);

        QTag *tag = new QTag("hello");
        boxLayout->addWidget(tag);

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
