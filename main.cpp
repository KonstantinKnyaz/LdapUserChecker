#include <QCoreApplication>

#include <QDebug>

#include "starter.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Starter st;

    qInfo() << st.getLastError().getErrorString();

    return st.getLastError().getLastErrorCode();
}
