#include <QApplication>
#include "MainWindow.h"
#include <QShortcut>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.showMaximized();
    QShortcut *esc = new QShortcut(QKeySequence(Qt::Key_Escape), &window);
    QObject::connect(esc, &QShortcut::activated, [&window]() {
        if (window.isFullScreen()) {
            window.showNormal();
            window.showMaximized();
        }
    });
    return app.exec();
}
