#include "Simple.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Simple w(argc,argv);
    w.show();

    return a.exec();
}
