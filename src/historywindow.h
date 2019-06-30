#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H


#include <QMainWindow>
#include <QMenuBar>
#include <QTranslator>
#include <QCloseEvent>


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
    bool confirm(const QString &message);


public slots:
    void reload();
    void cleanAll();
    void deleteSelected();
};

#endif // HISTORYWINDOW_H
