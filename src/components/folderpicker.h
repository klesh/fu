#ifndef FOLDERPICKER_H
#define FOLDERPICKER_H

#include <QFrame>
#include <QLineEdit>
#include <QPushButton>

class FolderPicker : public QFrame
{
    Q_OBJECT

    QLineEdit *_txtCurrentPath;
    QPushButton *_btnBrowse;

public:
    FolderPicker(QWidget *parent = nullptr);
    QString currentPath();
    void setCurrentPath(QString text);

public slots:
    void pickFolder();
};

#endif // FOLDERPICKER_H
