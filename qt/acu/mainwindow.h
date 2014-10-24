#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "maindelegator.h"
#include <QtGui/QLabel>
namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow, public MainDelegator::EventListener
{
    Q_OBJECT
    
public:
    //virtual void onRFSerialNumber(char* serial);
    virtual void onMessage(std::string tag, std::string data);
    virtual void onEmployeeInfo(std::string CoName, std::string Name, std::string PinNo, char* img_buf, int img_sz);
    virtual void onStatus(std::string status);
    
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void updateTime();

private:
    Ui::MainWindow *ui;
    QLabel* statusLabel; 
    std::map<std::string, QLabel*> labelTable;
};

#endif // MAINWINDOW_H
