#include "uploaddialog.h"
#include "ui_uploaddialog.h"

UploadDialog::UploadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UploadDialog)
{
    ui->setupUi(this);
}

UploadDialog::~UploadDialog()
{
    delete ui;
}
