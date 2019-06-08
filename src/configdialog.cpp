#include "application.h"
#include "configdialog.h"
#include "ui_configdialog.h"
#include "components/folderpicker.h"

#include <QDesktopServices>
#include <QFileInfo>
#include <QSpinBox>
#include <QToolTip>

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

    connect(ui->lstServers, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(serversShowItem(QListWidgetItem*, QListWidgetItem*)));
    connect(ui->lstServers, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(serversEditItem(QListWidgetItem*)));
    connect(ui->btnAddServer, SIGNAL(clicked()), this, SLOT(serversAddItem()));
    connect(ui->btnDelServer, SIGNAL(clicked()), this, SLOT(serversDelItems()));
    connect(ui->btnSaveServer, SIGNAL(clicked()), this, SLOT(serversEditItemSave()));
    connect(ui->btnCancelServer, SIGNAL(clicked()), this, SLOT(serversEditItemCancel()));
    connect(ui->cbbProtocol, SIGNAL(currentTextChanged(const QString&)), this, SLOT(serversReloadSettingsFrame(const QString&)));

    connect(ui->lstTags->itemDelegate(), SIGNAL(commitData(QWidget*)), this, SLOT(tagsEndEdit(QWidget*)));
    connect(ui->btnAddTag, SIGNAL(clicked()), this, SLOT(tagsAddItem()));
    connect(ui->btnDelTag, SIGNAL(clicked()), this, SLOT(tagsDelItems()));

    ui->tabs->setCurrentIndex(0);

    // load all protocols
    for (auto protocol : APP->serverService()->getProtocols()) {
        ui->cbbProtocol->addItem(protocol->getTitle(), protocol->getName());
    }

    reloadTab(TAB_SERVERS);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::reloadTab(int id)
{
    qDebug() << "reloading tab " << id;

    switch (id) {
    case TAB_SERVERS:
    {
        ui->lstServers->clear();
        QList<Server> servers = APP->serverService()->getAll();
        for (auto &server : servers) {
            auto listItem = new QListWidgetItem(server.name, ui->lstServers);
            listItem->setData(Qt::UserRole, server.id);
            ui->lstServers->addItem(listItem);
        }
        break;
    }
    case TAB_TAGS:
    {
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

void ConfigDialog::serversShowItem(QListWidgetItem* current, QListWidgetItem* previous)
{
    Q_UNUSED(previous);
    if (!current)
        return;

    uint id = current->data(Qt::UserRole).toUInt();
    Server server;
    if (id) {
        try {
            server = APP->serverService()->findById(id);
        } catch (Error &e) {
            ErrorMessage::showFatal(e.text(), this);
        }
    } else {
        server.protocol = ui->cbbProtocol->currentText();
    }
    ui->txtServerName->setText(server.name);
    ui->cbbProtocol->setCurrentIndex(ui->cbbProtocol->findData(server.protocol));

    qDebug() << "start updating setting components";
    auto protocol = APP->serverService()->findProtocol(server.protocol);

    for (auto settingInfo : protocol->getSettingInfos()) {
        switch (settingInfo.type) {
            case Directory:
            {
                auto folderPicker = ui->frmServerSettings->findChild<FolderPicker*>(settingInfo.name);
                assert(folderPicker);
                folderPicker->setCurrentPath(server.settings[settingInfo.name].toString());
                break;
            }
            case Integer:
            {
                auto spinBox = ui->frmServerSettings->findChild<QSpinBox*>(settingInfo.name);
                assert(spinBox);
                spinBox->setValue(server.settings[settingInfo.name].toInt());
                break;
            }
            case Text:
            {
                auto lineEdit = ui->frmServerSettings->findChild<QLineEdit*>(settingInfo.name);
                assert(lineEdit);
                lineEdit->setText(server.settings[settingInfo.name].toString());
                break;
            }

        }
    }
}

void ConfigDialog::serversEditItem(QListWidgetItem *)
{
    ui->grpServerForm->setEnabled(true);
    ui->grpServerList->setEnabled(false);
}

void ConfigDialog::serversEditItemSave()
{
    auto highlight = [](QWidget * widget, QString hint) {
        widget->setFocus();
        QToolTip::showText(widget->mapToGlobal(QPoint()), hint);
    };

    if (ui->txtServerName->text().isEmpty()) {
        return highlight(ui->txtServerName, tr("Please enter the name of server"));
    }

    Server server;
    server.id = ui->lstServers->currentItem()->data(Qt::UserRole).toUInt();
    server.name = ui->txtServerName->text();
    server.protocol = ui->cbbProtocol->currentData().toString();

    // collect settings
    auto protocol = APP->serverService()->findProtocol(ui->cbbProtocol->currentData().toString());
    for (auto settingInfo : protocol->getSettingInfos()) {
        switch (settingInfo.type) {
            case Directory:
            {
                auto folderPicker = ui->frmServerSettings->findChild<FolderPicker*>(settingInfo.name);
                assert(folderPicker);
                if (settingInfo.required && folderPicker->currentPath().isEmpty()) {
                    return highlight(folderPicker, settingInfo.hint);
                }
                qDebug() << folderPicker->currentPath();
                server.settings[settingInfo.name] = folderPicker->currentPath();
                break;
            }
            case Integer:
            {
                auto spinBox = ui->frmServerSettings->findChild<QSpinBox*>(settingInfo.name);
                assert(spinBox);
                server.settings[settingInfo.name] = spinBox->value();
                break;
            }
            case Text:
            {
                auto lineEdit = ui->frmServerSettings->findChild<QLineEdit*>(settingInfo.name);
                assert(lineEdit);
                if (settingInfo.required && lineEdit->text().isEmpty()) {
                    return highlight(lineEdit, settingInfo.hint);
                }
                server.settings[settingInfo.name] = lineEdit->text();
                break;
            }

        }
    }

    try {
        APP->serverService()->save(server);
        reloadTab(TAB_SERVERS);
    } catch (Error &e) {
        ErrorMessage::showFatal(e.text(), this);
    }
    serversEditItemCancel();
}

void ConfigDialog::serversEditItemCancel()
{
    ui->grpServerForm->setEnabled(false);
    ui->grpServerList->setEnabled(true);
}

void ConfigDialog::serversAddItem()
{
    auto listItem = new QListWidgetItem();
    listItem->setText(tr("new server"));
    listItem->setData(Qt::UserRole, 0);
    ui->lstServers->insertItem(0, listItem);
    ui->lstServers->setCurrentItem(listItem);
    serversEditItem(listItem);
}

void ConfigDialog::serversDelItems()
{

}

void ConfigDialog::serversReloadSettingsFrame(const QString &protocolTitle)
{
    qDebug() << "reload setting components for " << protocolTitle;
    if (protocolTitle.isEmpty())
        return;

    QLayoutItem* child = nullptr;
    QFormLayout *layout = qobject_cast<QFormLayout*>(ui->frmServerSettings->layout());
    while ((child = layout->takeAt(0))) {
        delete child;
    }
    QWidget *ctrl = nullptr;
    while ((ctrl = ui->frmServerSettings->findChild<QWidget*>())) {
        delete ctrl;
    }

    auto protocol = APP->serverService()->findProtocol(ui->cbbProtocol->currentData().toString());

    for (auto settingInfo : protocol->getSettingInfos()) {
        QWidget* input;
        switch (settingInfo.type) {
            case Directory:
            {
                auto folderPicker = new FolderPicker(this);
                folderPicker->setCurrentPath(settingInfo.defaultValue.toString());
                input = folderPicker;
                break;
            }
            case Integer:
            {
                auto spinbox = new QSpinBox(this);
                spinbox->setValue(settingInfo.defaultValue.toInt());
                input = spinbox;
                break;
            }
            case Text:
            {
                auto lineEdit = new QLineEdit(this);
                lineEdit->setText(settingInfo.defaultValue.toString());
                input = lineEdit;
                break;
            }
        }
        input->setToolTip(settingInfo.description);
        input->setObjectName(settingInfo.name);
        layout->addRow(settingInfo.label, input);
    }

    qDebug() << "setting components are ready";
}
