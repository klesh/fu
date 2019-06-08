#include "folderpicker.h"

#include <QHBoxLayout>
#include <QFileDialog>

FolderPicker::FolderPicker(QWidget *parent)
    : QFrame (parent)
{
    auto layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setLayout(layout);

    _txtCurrentPath = new QLineEdit(parent);
    _txtCurrentPath->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addWidget(_txtCurrentPath);

    _btnBrowse = new QPushButton(parent);
    _btnBrowse->setText(tr("Browse"));
    _btnBrowse->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(_btnBrowse);

    connect(_btnBrowse, SIGNAL(clicked()), this, SLOT(pickFolder()));
}

QString FolderPicker::currentPath()
{
    return _txtCurrentPath->text();
}

void FolderPicker::setCurrentPath(QString text)
{
    _txtCurrentPath->setText(text);
}

void FolderPicker::pickFolder()
{
    _txtCurrentPath->setText(QFileDialog::getExistingDirectory(this, "", _txtCurrentPath->text()));
}
