#include "BHMI.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BHMI w;
    w.show();
    return a.exec();
}
