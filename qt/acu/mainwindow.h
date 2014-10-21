#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "maindelegator.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow, public MainDelegator::EventListener
{
    Q_OBJECT
    
public:
    virtual void onRFSerialNumber(char* serial);
    virtual void onMessage(const char* msg);
    
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void update();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
