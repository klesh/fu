#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H


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


private slots:
    void reload();
    void showClipsContextMenu(const QPoint &pos);
};

#endif // HISTORYWINDOW_H
