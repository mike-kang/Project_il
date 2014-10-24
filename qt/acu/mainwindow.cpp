#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QTimer>
#include <QDateTime>

#define insertTable(tag)  labelTable.insert(pair<std::string, QLabel*>(#tag, ui->label##tag)) 

using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusLabel = new QLabel;
    statusBar()->addWidget(statusLabel);

    ui->labelGateLoc->setText("");
    ui->labelGateNo->setText("");
    ui->labelServer->setText("");
    ui->labelRfid->setText("");
    ui->labelRfidNo->setText("");
    ui->labelResult->setText("");
    ui->labelMsg->setText("");
    ui->labelCoName->setText("");
    ui->labelName->setText("");
    ui->labelPinNo->setText("");

    insertTable(GateLoc);
    insertTable(GateNo);
    insertTable(Server);
    insertTable(Rfid);
    //insertTable(CoName);
    //insertTable(Name);
    //insertTable(PinNo);
    insertTable(RfidNo);
    insertTable(Result);
    insertTable(Msg);

    MainDelegator* md = MainDelegator::createInstance(this);
    //md->setEventListener(this);

    //QDate* date = new QDate();
    QDateTime curDate = QDateTime::currentDateTime();   // 시스템에서 현재 날짜 가져오기
    QString date_string = curDate.toString("yyyy-MM-dd hh:mm:ss"); // QDate 타입을 QString 타입으로 변환
    qDebug() << date_string;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start(1000);
    



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onEmployeeInfo(std::string CoName, std::string Name, std::string PinNo, char* img_buf, int img_sz)
{
    QMetaObject::invokeMethod(ui->labelCoName, "setText", Q_ARG(QString, CoName.c_str()));
    QMetaObject::invokeMethod(ui->labelName, "setText", Q_ARG(QString, Name.c_str()));
    QMetaObject::invokeMethod(ui->labelPinNo, "setText", Q_ARG(QString, PinNo.c_str()));

}

void MainWindow::onMessage(std::string tag, std::string data)
{
    cout << "onMessage:" << tag << ":" << data << endl;
    for(std::map<std::string, QLabel*>::iterator iter = labelTable.begin(); iter != labelTable.end(); iter++){
      if(iter->first == tag){
        QMetaObject::invokeMethod(iter->second, "setText", Q_ARG(QString, data.c_str()));
      }
        
    }
}

void MainWindow::onStatus(std::string status)
{
  cout << status << endl;
  QMetaObject::invokeMethod(statusLabel, "setText", Q_ARG(QString, status.c_str()));
}

void MainWindow::updateTime()
{
    //qDebug() << "update";
    QDateTime curDate = QDateTime::currentDateTime();
    QString date_string = curDate.toString("yyyy-MM-dd hh:mm:ss");
    ui->labelTime->setText(date_string);
}
