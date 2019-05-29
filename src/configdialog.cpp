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
    connect(ui->lstTags->itemDelegate(), SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)), this, SLOT(on_lstTags_endEdit(QWidget*, QAbstractItemDelegate::EndEditHint)));
    connect(ui->btnAddTag, SIGNAL(clicked()), this, SIGNAL(on_btnAddTag_clicked()));
    connect(ui->btnDelTag, SIGNAL(clicked()), this, SIGNAL(on_btnDelTag_clicked()));
    ui->tabs->setCurrentIndex(0);
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
        ui->lstTags->clear();
        QList<Tag> tags = APP->tagService()->getAll();
        for (auto &tag : tags) {
            auto listItem = new QListWidgetItem(tag.getName(), ui->lstTags);
            listItem->setFlags(listItem->flags() | Qt::ItemIsEditable);
            listItem->setData(Qt::UserRole, tag.getId());
            ui->lstTags->addItem(listItem);
        }
        break;
    }
}

void ConfigDialog::on_lstTags_endEdit(QWidget* editor, QAbstractItemDelegate::EndEditHint hint)
{
    Q_UNUSED(hint);
    QString newName = reinterpret_cast<QLineEdit*>(editor)->text();
    if (!newName.isEmpty()) {
        uint id = ui->lstTags->currentItem()->data(Qt::UserRole).toUInt();
        if (id) {
            APP->tagService()->update(id, newName);
        } else {
            APP->tagService()->append(newName);
        }
    }
    on_currentTab_changed(TAB_TAGS);
}

void ConfigDialog::on_btnAddTag_clicked()
{
    auto listItem = new QListWidgetItem(ui->lstTags);
    listItem->setFlags(listItem->flags() | Qt::ItemIsEditable);
    listItem->setData(Qt::UserRole, 0);
    ui->lstTags->addItem(listItem);
    ui->lstTags->setCurrentItem(listItem);
    ui->lstTags->editItem(listItem);
}

void ConfigDialog::on_btnDelTag_clicked()
{
    auto selectedItems = ui->lstTags->selectedItems();
    if (selectedItems.size() == 0) {
        ErrorMessage::showInfo(tr("No tag is selected!"), this);
    } else {
        if (ErrorMessage::confirm(tr("Are you sure to delete all selected tags?"), this) == QMessageBox::Yes) {
            for (auto selectedItem : selectedItems) {
                APP->tagService()->remove(selectedItem->data(Qt::UserRole).toUInt());
            }
            on_currentTab_changed(TAB_TAGS);
        }
    }
}
