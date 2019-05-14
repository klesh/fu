#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include "models/record.h"

#include <QMainWindow>
#include <QMenuBar>
#include <QTranslator>
#include <QCloseEvent>
#include <QMenu>


namespace Ui {
class HistoryWindow;
}

class HistoryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HistoryWindow();
    ~HistoryWindow();

private:
    Ui::HistoryWindow *ui;

    void updateRecords(const QList<Record> &records);

private slots:
    void showRecordsContextMenu(const QPoint &pos);
};

#endif // HISTORYWINDOW_H
