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
