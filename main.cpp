#include "Simple.h"
#include <QApplication>
#include <QSplashScreen>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Simple w(argc,argv);
    w.show();

    return a.exec();
}
