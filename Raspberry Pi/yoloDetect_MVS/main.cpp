#include "widget.h"
#include "mainwidget.h"
#include <QApplication>
#include <math.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    QApplication a(argc, argv);
    MainWidget mw;
    mw.show();
    return a.exec();
}
