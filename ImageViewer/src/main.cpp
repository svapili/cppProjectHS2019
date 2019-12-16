/**
*   C++ II HS2019
*   Text extraction GUI application
*
*   @author Simon Schweizer
*   @version 1.3
*/

#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle("ImageViewer V1.3 - OCR");

    // start application
    window.show();
    return app.exec();
}
