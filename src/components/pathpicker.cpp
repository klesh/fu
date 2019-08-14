#include "pathpicker.h"

#include <QHBoxLayout>
#include <QFileDialog>

PathPicker::PathPicker(QWidget *parent, TargetType targetType)
    : QFrame (parent), _targetType(targetType)
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

    connect(_btnBrowse, SIGNAL(clicked()), this, SLOT(showPickingDialog()));
}

QString PathPicker::currentPath()
{
    return _txtCurrentPath->text();
}

void PathPicker::setCurrentPath(QString text)
{
    _txtCurrentPath->setText(text);
}

void PathPicker::showPickingDialog()
{
    if (_targetType == Directory) {
        _txtCurrentPath->setText(QFileDialog::getExistingDirectory(this, "", _txtCurrentPath->text()));
    } else {
        _txtCurrentPath->setText(QFileDialog::getOpenFileName(this, "", _txtCurrentPath->text()));
    }
}
