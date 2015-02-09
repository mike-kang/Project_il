#include <QtGui/QApplication>
#include "mainwindow.h"
using namespace std;

int main(int argc, char *argv[])
{
    const char* config = NULL;
    QApplication a(argc, argv);
    int opt = getopt(argc, argv, "c:");   
    MainWindow w;
    if(opt == 'c'){
      cout << config << endl;
      config = optarg;
    }

    MainDelegator::createInstance(&w, config);

    w.show();
    
    return a.exec();
}
