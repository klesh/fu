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
    static HistoryWindow &getInstance();
    HistoryWindow(HistoryWindow const&) = delete;
    void operator=(HistoryWindow const&) = delete;

private:
    explicit HistoryWindow(QWidget *parent = nullptr);
    ~HistoryWindow();
    void closeEvent(QCloseEvent *evt);

    Ui::HistoryWindow *ui;
    const static QString TAG_STYLE_DEFAULT;
    const static QString TAG_STYLE_ACTIVE;
};

#endif // HISTORYWINDOW_H
