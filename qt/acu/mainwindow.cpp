#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QTimer>
#include <QDateTime>

using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainDelegator* md = MainDelegator::getInstance();
    md->setEventListener(this);

    //QDate* date = new QDate();
    QDateTime curDate = QDateTime::currentDateTime();   // 시스템에서 현재 날짜 가져오기
    QString date_string = curDate.toString("yyyy-MM-dd hh:mm:ss"); // QDate 타입을 QString 타입으로 변환
    qDebug() << date_string;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onRFSerialNumber(char* serial)
{
    cout << "ok serialnum:" << serial << endl;
    QMetaObject::invokeMethod(ui->label, "setText", Q_ARG(QString, serial));

}

void MainWindow::update()
{
    qDebug() << "update";
    QDateTime curDate = QDateTime::currentDateTime();
    QString date_string = curDate.toString("yyyy-MM-dd hh:mm:ss");
    ui->label->setText(date_string);
}
