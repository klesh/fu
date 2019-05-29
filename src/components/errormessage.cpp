#include "errormessage.h"

ErrorMessage::ErrorMessage(QWidget *parent)
    : QMessageBox(parent)
{

}

int ErrorMessage::showFatal(const QString &message, QWidget *parent)
{
    ErrorMessage msgBox(parent);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(tr("Fatal Error"));
    msgBox.setText(tr("A critical error has occurred! Please report this issue along with following detail if you don't know what is going on."));
    msgBox.setInformativeText("<a href=\"https://github.com/klesh/fu/issues\">" + tr("Go to issue website") + "</a>");
    msgBox.setDetailedText(message);
    return msgBox.exec();
}

int ErrorMessage::showInfo(const QString &message, QWidget *parent)
{
    ErrorMessage msgBox(parent);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(tr("Notice"));
    msgBox.setText(message);
    return msgBox.exec();
}

int ErrorMessage::confirm(const QString &message, QWidget *parent)
{
    ErrorMessage msgBox(parent);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(tr("Warning"));
    msgBox.setText(message);
    msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    return msgBox.exec();
}
