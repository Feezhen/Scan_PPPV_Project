#include "dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
//    qputenv("QT_IM_MODULE",QByteArray("qtvirtualkeyboard"));
    qputenv("QT_IM_MODULE",QByteArray("Qt5Input"));
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    return a.exec();
}
