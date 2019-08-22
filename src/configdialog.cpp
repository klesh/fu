#include "application.h"
#include "configdialog.h"
#include "ui_configdialog.h"
#include "components/pathpicker.h"

#include <QDesktopServices>
#include <QFileInfo>
#include <QSpinBox>
#include <QToolTip>
#include <QFileDialog>

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

    connect(ui->lstFormats, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(formatsShowItem(QListWidgetItem*, QListWidgetItem*)));
    connect(ui->lstFormats, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(formatsEditItem(QListWidgetItem*)));
    connect(ui->btnAddFormat, SIGNAL(clicked()), this, SLOT(formatsAddItem()));
    connect(ui->btnDelFormat, SIGNAL(clicked()), this, SLOT(formatsDelItems()));
    connect(ui->btnSaveFormat, SIGNAL(clicked()), this, SLOT(formatsEditItemSave()));
    connect(ui->btnCancelFormat, SIGNAL(clicked()), this, SLOT(formatsEditItemCancel()));

    connect(ui->btnWatermarkImagePicker, SIGNAL(clicked()), this, SLOT(imagePickWatermarkFile()));
    connect(ui->cbxImageCompression, SIGNAL(stateChanged(int)), this, SLOT(imageSaveCompressionSetting(int)));
    connect(ui->cbxImageWatermark, SIGNAL(stateChanged(int)), this, SLOT(imageSaveWatermarkSetting(int)));
    connect(ui->btnWatermarkPositionTopLeft, SIGNAL(toggled(bool)), this, SLOT(imageSaveWatermarkPositionSetting(bool)));
    connect(ui->btnWatermarkPositionTopCenter, SIGNAL(toggled(bool)), this, SLOT(imageSaveWatermarkPositionSetting(bool)));
    connect(ui->btnWatermarkPositionTopRight, SIGNAL(toggled(bool)), this, SLOT(imageSaveWatermarkPositionSetting(bool)));
    connect(ui->btnWatermarkPositionMiddleLeft, SIGNAL(toggled(bool)), this, SLOT(imageSaveWatermarkPositionSetting(bool)));
    connect(ui->btnWatermarkPositionMiddleCenter, SIGNAL(toggled(bool)), this, SLOT(imageSaveWatermarkPositionSetting(bool)));
    connect(ui->btnWatermarkPositionMiddleRight, SIGNAL(toggled(bool)), this, SLOT(imageSaveWatermarkPositionSetting(bool)));
    connect(ui->btnWatermarkPositionBottomLeft, SIGNAL(toggled(bool)), this, SLOT(imageSaveWatermarkPositionSetting(bool)));
    connect(ui->btnWatermarkPositionBottomCenter, SIGNAL(toggled(bool)), this, SLOT(imageSaveWatermarkPositionSetting(bool)));
    connect(ui->btnWatermarkPositionBottomRight, SIGNAL(toggled(bool)), this, SLOT(imageSaveWatermarkPositionSetting(bool)));

    connect(ui->lstTags->itemDelegate(), SIGNAL(commitData(QWidget*)), this, SLOT(tagsEndEdit(QWidget*)));
    connect(ui->btnAddTag, SIGNAL(clicked()), this, SLOT(tagsAddItem()));
    connect(ui->btnDelTag, SIGNAL(clicked()), this, SLOT(tagsDelItems()));

    connect(ui->btnOpenDataDir, SIGNAL(clicked()), this, SLOT(bakOpenDataDir()));

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
    case TAB_IMAGE:
    {
        ui->cbxImageCompression->setChecked(APP->settingService()->imageCompressionEnabled());

        auto imageWatermarkEnabled = APP->settingService()->imageWatermarkEnabled();
        ui->cbxImageWatermark->setChecked(imageWatermarkEnabled);
        ui->btnWatermarkImagePicker->setEnabled(imageWatermarkEnabled);

        auto imageWatermarkPath = APP->settingService()->imageWatermarkPath();
        if (!imageWatermarkPath.isEmpty()) {
            QFileInfo fi(imageWatermarkPath);
            if (fi.exists() && fi.isFile()) {
                ui->lblWatermarkImagePreviewer->setPixmap(QPixmap(imageWatermarkPath).scaled(200, 200, Qt::KeepAspectRatio));
            }
        }

        auto imageWatermarkPosition = APP->settingService()->imageWatermarkPosition();
        auto btnWatermarkPosition = ui->frmWatermarkPosition->findChild<QPushButton*>(QString("btnWatermarkPosition%1").arg(imageWatermarkPosition));
        btnWatermarkPosition->setChecked(true);
        break;
    }
    case TAB_OUTPUT_FORMATS:
    {
        ui->lstFormats->clear();
        QList<Format> formats = APP->formatService()->getAll();
        for (auto &format : formats) {
            auto listItem = new QListWidgetItem(format.name, ui->lstFormats);
            listItem->setData(Qt::UserRole, format.id);
            ui->lstFormats->addItem(listItem);
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

void ConfigDialog::serversShowItem(QListWidgetItem* current, QListWidgetItem* previous)
{
    ui->btnDelServer->setEnabled(current);

    Q_UNUSED(previous)
    if (!current)
        return;

    uint id = current->data(Qt::UserRole).toUInt();

    if (!id)
        return;

    Server server;
    if (id) {
        server = APP->serverService()->findById(id);
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
            auto folderPicker = ui->frmServerSettings->findChild<PathPicker*>(settingInfo.name);
            assert(folderPicker);
            folderPicker->setCurrentPath(server.settings[settingInfo.name].toString());
            break;
        }
        case File:
        {
            auto filePicker = ui->frmServerSettings->findChild<PathPicker*>(settingInfo.name);
            filePicker->setCurrentPath(server.settings[settingInfo.name].toString());
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
    if (ui->txtServerName->text().isEmpty()) {
        return highlightWidget(ui->txtServerName, tr("Please enter the name of server"));
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
                auto folderPicker = ui->frmServerSettings->findChild<PathPicker*>(settingInfo.name);
                assert(folderPicker);
                if (settingInfo.required && folderPicker->currentPath().isEmpty()) {
                    return highlightWidget(folderPicker, settingInfo.hint);
                }
                qDebug() << folderPicker->currentPath();
                server.settings[settingInfo.name] = folderPicker->currentPath();
                break;
            }
            case File:
            {
                auto filePicker = ui->frmServerSettings->findChild<PathPicker*>(settingInfo.name);
                assert(filePicker);
                if (settingInfo.required && filePicker->currentPath().isEmpty()) {
                    return highlightWidget(filePicker, settingInfo.hint);
                }
                qDebug() << filePicker->currentPath();
                server.settings[settingInfo.name] = filePicker->currentPath();
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
                    return highlightWidget(lineEdit, settingInfo.hint);
                }
                server.settings[settingInfo.name] = lineEdit->text();
                break;
            }

        }
    }

    if (APP->serverService()->save(server)) {
        reloadTab(TAB_SERVERS);
    } else {
        ErrorMessage::showInfo("Server with the same name already exists.", this);
    }
    ui->grpServerForm->setEnabled(false);
    ui->grpServerList->setEnabled(true);
}

void ConfigDialog::serversEditItemCancel()
{
    ui->grpServerForm->setEnabled(false);
    ui->grpServerList->setEnabled(true);
    reloadTab(TAB_SERVERS);
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
    APP->serverService()->remove(ui->lstServers->currentItem()->data(Qt::UserRole).toUInt());
    reloadTab(TAB_SERVERS);
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
                auto folderPicker = new PathPicker(this);
                folderPicker->setCurrentPath(settingInfo.defaultValue.toString());
                input = folderPicker;
                break;
            }
            case File:
            {
                auto filePicker = new PathPicker(this, PathPicker::File);
                filePicker->setCurrentPath(settingInfo.defaultValue.toString());
                input = filePicker;
                break;
            }
            case Integer:
            {
                auto spinbox = new QSpinBox(this);
                spinbox->setValue(settingInfo.defaultValue.toInt());
                spinbox->setRange(1, 65536);
                input = spinbox;
                break;
            }
            default:
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

void ConfigDialog::imagePickWatermarkFile()
{
    auto watermarkPath = QFileDialog::getOpenFileName(this, tr("Select an image file as watermark"), "", tr("Images (*.png *.gif *.jpg)"));
    APP->settingService()->setImageWatermarkPath(watermarkPath);
    reloadTab(TAB_IMAGE);
}

void ConfigDialog::imageSaveCompressionSetting(int enabled)
{
    APP->settingService()->setImageCompressionEnabled(enabled);
}

void ConfigDialog::imageSaveWatermarkSetting(int enabled)
{
    APP->settingService()->setImageWatermarkEnabled(enabled);
    ui->btnWatermarkImagePicker->setEnabled(enabled);
}

void ConfigDialog::imageSaveWatermarkPositionSetting(bool toggled)
{
    qDebug() << "saving watermark position setting " << toggled;
    if (toggled) // avoid initialization trigger saving action
        return;


    for (auto &btn : ui->frmWatermarkPosition->findChildren<QPushButton*>()) {
        if (btn->isChecked()) {
            qDebug() << "saving watermark position setting " << btn->objectName().mid(20);
            APP->settingService()->setImageWatermarkPosition(btn->objectName().mid(20));
            break;
        }
    }
}

void ConfigDialog::formatsShowItem(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)

    ui->btnDelFormat->setEnabled(current);

    if (!current)
        return;

    uint id = current->data(Qt::UserRole).toUInt();
    if (id) {
        Format format = APP->formatService()->findById(id);
        ui->txtFormatName->setText(format.name);
        ui->txtFormatTemplate->setText(format.format);
    }
}

void ConfigDialog::formatsEditItem(QListWidgetItem *)
{
    ui->grpFormatForm->setEnabled(true);
    ui->grpFormatList->setEnabled(false);
}

void ConfigDialog::formatsEditItemSave()
{
    if (ui->txtFormatName->text().isEmpty()) {
        return highlightWidget(ui->txtFormatName, tr("Please enter the name of output format"));
    }
    if (ui->txtFormatTemplate->toPlainText().isEmpty()) {
        return highlightWidget(ui->txtFormatTemplate, tr("Please enter the template of output format"));
    }

    Format format;
    format.id = ui->lstFormats->currentItem()->data(Qt::UserRole).toUInt();
    format.name = ui->txtFormatName->text();
    format.format = ui->txtFormatTemplate->toPlainText();


    if (APP->formatService()->save(format)) {
        reloadTab(TAB_OUTPUT_FORMATS);
    } else {
        ErrorMessage::showInfo("Format with the same name already exists.", this);
    }
    ui->grpFormatForm->setEnabled(false);
    ui->grpFormatList->setEnabled(true);
}

void ConfigDialog::formatsEditItemCancel()
{
    ui->grpFormatForm->setEnabled(false);
    ui->grpFormatList->setEnabled(true);
    reloadTab(TAB_OUTPUT_FORMATS);
}

void ConfigDialog::formatsAddItem()
{
    auto listItem = new QListWidgetItem();
    listItem->setText(tr("new format"));
    listItem->setData(Qt::UserRole, 0);
    ui->lstFormats->insertItem(0, listItem);
    ui->lstFormats->setCurrentItem(listItem);
    formatsEditItem(listItem);
}

void ConfigDialog::formatsDelItems()
{
    APP->formatService()->remove(ui->lstFormats->currentItem()->data(Qt::UserRole).toUInt());
    reloadTab(TAB_OUTPUT_FORMATS);
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

void ConfigDialog::bakOpenDataDir()
{
    QFileInfo fileInfo(APP->getDbPath());
    QDesktopServices::openUrl(QUrl("file:///" + fileInfo.dir().absolutePath()));
}

void ConfigDialog::highlightWidget(QWidget *widget, const QString hint)
{
    widget->setFocus();
    QToolTip::showText(widget->mapToGlobal(QPoint()), hint);
}
