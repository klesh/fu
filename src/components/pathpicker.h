#ifndef FOLDERPICKER_H
#define FOLDERPICKER_H

#include <QFrame>
#include <QLineEdit>
#include <QPushButton>

class PathPicker : public QFrame
{
    Q_OBJECT


public:
    enum TargetType { File, Directory };
    PathPicker(QWidget *parent = nullptr, TargetType targetType = Directory);
    QString currentPath();
    void setCurrentPath(QString text);

public slots:
    void showPickingDialog();

private:
    QLineEdit *_txtCurrentPath;
    QPushButton *_btnBrowse;
    TargetType _targetType;
};

#endif // FOLDERPICKER_H
