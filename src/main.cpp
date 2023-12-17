#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    auto window = new CMainWindow();
    window->setWindowTitle("Co Compiler");
    window->show();

    return QApplication::exec();
}
