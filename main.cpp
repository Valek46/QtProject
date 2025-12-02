#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv); // 1) создаём "двигатель" Qt

    MainWindow w;  // 2) создаём наше главное окно
    w.show();      // 3) показываем его на экране

    return app.exec(); // 4) запускаем цикл обработки событий
}
