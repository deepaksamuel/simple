#include "Simple.h"
#include <QApplication>
#include <QSplashScreen>
// File modified by Deepak Samuel on 25 Sep 2019
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Simple w(argc,argv);
    w.show();

    return a.exec();
}
