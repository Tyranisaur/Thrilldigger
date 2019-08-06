#include "headers/settingswindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SettingsWindow w;
    w.show();

    return QApplication::exec();
}
