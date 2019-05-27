#include "application.h"
#include "configdialog.h"
#include "ui_configdialog.h"
#include "core/tagservice.h"

#include <QDesktopServices>
#include <QFileInfo>

#define TAB_SERVERS 0
#define TAB_IMAGE 1
#define TAB_OUTPUT_FORMATS 2
#define TAB_TAGS 3
#define TAB_BACKUP_RESTORE 4


ConfigDialog::ConfigDialog() :
    QDialog(),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);

    connect(ui->tabs, SIGNAL(currentChanged(int)), this, SLOT(on_currentTab_changed(int)));
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::on_btnOpenDataDir_clicked()
{
    QFileInfo fileInfo(APP->getDbPath());
    QDesktopServices::openUrl(QUrl("file:///" + fileInfo.dir().absolutePath()));
}

void ConfigDialog::on_currentTab_changed(int id)
{
    switch (id) {
    case TAB_TAGS:
        QList<Tag> tags = APP->tagService()->getAll();
        for (auto &tag : tags) {
            ui->lstTags->addItem(tag.getName());
        }
        break;
    }
    qDebug() << id;
}
