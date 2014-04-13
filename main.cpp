#include "mainwindow.h"
#include <QApplication>
#include <QtWebKit/QWebSettings>
#include <QUrl>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QUrl url;
    if (argc > 1)
        url = QUrl::fromUserInput(argv[1]);
    else
        url = QUrl("http://www.google.com");

    app.setApplicationName(app.translate("main", "Web Browser"));
    app.setCursorFlashTime(0);

    QWebSettings *webSettings = QWebSettings::globalSettings();
    webSettings->setAttribute(QWebSettings::AutoLoadImages, true);
    webSettings->setAttribute(QWebSettings::JavascriptEnabled, true);
    webSettings->setAttribute(QWebSettings::PluginsEnabled, true);
    webSettings->setAttribute(QWebSettings::ZoomTextOnly, true);

    MainWindow window(url);
    window.show();

    return app.exec();
}
