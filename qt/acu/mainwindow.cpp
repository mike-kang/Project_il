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
    QDateTime curDate = QDateTime::currentDateTime();   // �ý��ۿ��� ���� ��¥ ��������
    QString date_string = curDate.toString("yyyy-MM-dd hh:mm:ss"); // QDate Ÿ���� QString Ÿ������ ��ȯ
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
