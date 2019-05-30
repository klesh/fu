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

    connect(ui->tabs, SIGNAL(currentChanged(int)), this, SLOT(reloadTab(int)));
    connect(ui->lstTags->itemDelegate(), SIGNAL(commitData(QWidget*)), this, SLOT(tagsEndEdit(QWidget*)));
    connect(ui->btnAddTag, SIGNAL(clicked()), this, SLOT(tagsAddItem()));
    connect(ui->btnDelTag, SIGNAL(clicked()), this, SLOT(tagsDelItems()));
    ui->tabs->setCurrentIndex(0);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::reloadTab(int id)
{
    switch (id) {
    case TAB_TAGS:
        ui->lstTags->clear();
        QList<Tag> tags = APP->tagService()->getAll();
        for (auto &tag : tags) {
            auto listItem = new QListWidgetItem(tag.name, ui->lstTags);
            listItem->setFlags(listItem->flags() | Qt::ItemIsEditable);
            listItem->setData(Qt::UserRole, tag.id);
            ui->lstTags->addItem(listItem);
        }
        break;
    }
}

void ConfigDialog::bakOpenDataDir()
{
    QFileInfo fileInfo(APP->getDbPath());
    QDesktopServices::openUrl(QUrl("file:///" + fileInfo.dir().absolutePath()));
}

void ConfigDialog::tagsEndEdit(QWidget* editor)
{
    QString newName = reinterpret_cast<QLineEdit*>(editor)->text();
    uint id = ui->lstTags->currentItem()->data(Qt::UserRole).toUInt();
    if (!newName.isEmpty()) {
        if (id) {
            APP->tagService()->update(id, newName);
        } else {
            APP->tagService()->append(newName);
        }
    }
    reloadTab(TAB_TAGS);
}

void ConfigDialog::tagsAddItem()
{
    auto listItem = new QListWidgetItem();
    listItem->setFlags(listItem->flags() | Qt::ItemIsEditable);
    listItem->setData(Qt::UserRole, 0);
    ui->lstTags->insertItem(0, listItem);
    ui->lstTags->setCurrentItem(listItem);
    ui->lstTags->editItem(listItem);
}

void ConfigDialog::tagsDelItems()
{
    auto selectedItems = ui->lstTags->selectedItems();
    if (selectedItems.size() == 0) {
        ErrorMessage::showInfo(tr("No tag is selected!"), this);
    } else {
        if (ErrorMessage::confirm(tr("Are you sure to delete all selected tags?"), this) == QMessageBox::Yes) {
            for (auto selectedItem : selectedItems) {
                APP->tagService()->remove(selectedItem->data(Qt::UserRole).toUInt());
            }
            reloadTab(TAB_TAGS);
        }
    }
}
