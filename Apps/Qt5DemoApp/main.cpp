#include "MainWindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QTranslator>
int main(int argc, char** argv)
{
    QApplication  app(argc, argv);


    QPixmap       pixmap(QString::fromLocal8Bit(":/arduino.png"));
    QSplashScreen splash(pixmap);
    splash.show();

    QTranslator* translator = new QTranslator;
    translator->load(":/Translations/zh_CN.qm");
    app.installTranslator(translator);
    
    MainWindow mainwindow;
    mainwindow.show();

    splash.finish(&mainwindow);
    return app.exec();
}
