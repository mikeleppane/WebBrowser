#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWebView>
#include <QToolBar>
#include <QProgressBar>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QWidget>
#include <QSizePolicy>
#include <QUrl>
#include <QDebug>
#include <QFontMetrics>
#include <QListIterator>
#include <QWebHistoryItem>
#include <QWebHistory>
#include <QWebPage>
#include <QMessageBox>
#include <QTimer>
#include <QNetworkInterface>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTableView>
#include <QStandardItemModel>
#include "bookmarkdb.h"
#include "webhistory.h"

MainWindow::MainWindow(const QUrl &url) : ui(new Ui::MainWindow), progress(0)
{
    ui->setupUi(this);

    bookmarkDB = std::unique_ptr<BookmarkDB>{ new BookmarkDB };

    createWidgets(url);
    createActions();
    createMenus();
    createConnections();
    createLayout();

    view->load(url);
    startRequest(url);
    pageTimer->start(1000 * 120);
    loadBookmarks();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createWidgets(const QUrl &url)
{
    view = new QWebView(this);
    view->load(url);
    locationEdit = new QLineEdit(this);
    locationEdit->setSizePolicy(QSizePolicy::Expanding,
                                locationEdit->sizePolicy().verticalPolicy());
    toolBar = addToolBar(tr("Navigation"));
    progressBar = new QProgressBar(this);
    progressBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    pageTimer = new QTimer(this);
}

void MainWindow::createActions()
{
    exitAction = new QAction(tr("Exit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit"));

    showHistoryAction = new QAction(tr("Show web history"), this);
    showHistoryAction->setShortcut(tr("Ctrl+H"));
    showHistoryAction->setStatusTip(tr("Show history"));

    clearHistoryAction = new QAction(tr("Clear history"), this);
    clearHistoryAction->setShortcut(tr("Ctrl+D"));
    clearHistoryAction->setStatusTip(tr("Clear history"));

    addNewBookmarkAction = new QAction(tr("Add new bookmark"), this);
    addNewBookmarkAction->setShortcut(tr("Ctrl+N"));
    addNewBookmarkAction->setStatusTip(tr("Add new bookmark"));

    hostInfoAction = new QAction(tr("Show host info"), this);
    hostInfoAction->setShortcut(tr("Ctrl+I"));
    hostInfoAction->setStatusTip(tr("Show host info"));

    toolBar->addAction(view->pageAction(QWebPage::Back));
    toolBar->addAction(view->pageAction(QWebPage::Forward));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(hostInfoAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    pageHistoryMenu = menuBar()->addMenu(tr("Page history"));
    pageHistoryMenu->addAction(showHistoryAction);
    pageHistoryMenu->addAction(clearHistoryAction);
    pageHistoryMenu->addSeparator();

    bookmarkMenu = menuBar()->addMenu(tr("Bookmarks"));
    bookmarkMenu->addAction(addNewBookmarkAction);
    bookmarkMenu->addSeparator();
    showLastAddedBookMarks = bookmarkMenu->addMenu("Lastly added");
}

void MainWindow::createConnections()
{

    connect(view, SIGNAL(loadProgress(int)), progressBar, SLOT(setValue(int)));
    connect(view, SIGNAL(titleChanged(QString)), SLOT(setBrowserTitle()));
    connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), SLOT(finishedLoading(bool)));
    connect(locationEdit, SIGNAL(returnPressed()), SLOT(changePage()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(pageHistoryMenu, SIGNAL(aboutToShow()), SLOT(setWebHistory()));
    connect(pageTimer, SIGNAL(timeout()), SLOT(networkTimeOut()));
    connect(hostInfoAction, SIGNAL(triggered()), this, SLOT(showHostInfo()));
    connect(showHistoryAction, SIGNAL(triggered()), this,
            SLOT(showWebHistory()));
    connect(clearHistoryAction, SIGNAL(triggered()), this,
            SLOT(clearWebHistory()));
    /*
    connect(view->pageAction(QWebPage::Back), SIGNAL(triggered()), this,
            SLOT(getBackPageUrl()));
    connect(view->pageAction(QWebPage::Forward), SIGNAL(triggered()), this,
            SLOT(getForwardPageUrl()));
    */
    connect(addNewBookmarkAction, SIGNAL(triggered()), SLOT(addNewBookmark()));
}

void MainWindow::createLayout()
{

    toolBar->addWidget(locationEdit);
    toolBar->addWidget(progressBar);
    setCentralWidget(view);
    setUnifiedTitleAndToolBarOnMac(true);
    view->setAttribute(Qt::WA_DeleteOnClose);

    resize(1000, 800);
}

void MainWindow::changePage()
{
    // every time a user enters a url to the address field
    // this function is called. It starts the request and timer.
    // The timer is set to trigger after 120 s and stops
    // the loading
    QUrl url = QUrl::fromUserInput(locationEdit->text());
    if (url.isValid() && !url.isEmpty()) {
        view->load(url);
        startRequest(url);
        view->setFocus();
        pageTimer->start(1000 * 120);
    } else {
        view->stop();
        view->setHtml("<h3><font color=red>Given url (" + url.toString() +
                      ") is not valid."
                      "</font></h3><br>" +
                      url.errorString() + "<br>" +
                      "Please check that you have given a right url.");
    }
}

void MainWindow::setBrowserTitle()
{

    if (progress <= 0 || progress >= 100)
        setWindowTitle(view->title());
    else
        setWindowTitle(QString("%1 (%2%)").arg(view->title()).arg(progress));
}

void MainWindow::setProgress(int value)
{
    // update progress bar
    progress = value;
    setBrowserTitle();
}

void MainWindow::finishedLoading(bool ok)
{
    // Each valid page is being stored to a container
    // for later use so that the last visited page is
    // the first item on the stack.
    if (ok) {
        progress = 100;
        setBrowserTitle();
        locationEdit->setText(view->url().toString());
        if (!view->url().isEmpty() && !view->title().isEmpty()) {
            if (visitedUrls.contains(qMakePair(view->url(), view->title()))) {
                visitedUrls.prepend(visitedUrls.takeAt(visitedUrls.indexOf(
                    qMakePair(view->url(), view->title()))));
            } else {
                visitedUrls.prepend(qMakePair(view->url(), view->title()));
            }
            updatePageHistoryActionList();
        }
    }
    pageTimer->stop();
}

void MainWindow::updatePageHistoryActionList()
{
    // Update web history menu.
    // Restrict the content to 10 items and set
    // a shortcut to each item.
    if (!pageHistoryItems.isEmpty())
        for (const auto &item : pageHistoryItems) {
            pageHistoryMenu->removeAction(item);
        }
    pageHistoryItems.clear();
    QFontMetrics fontMetrics(font());
    int k = 0;
    while (k < visitedUrls.count() && k < MaxWebHistoryItems()) {
        QString title =
            fontMetrics.elidedText(visitedUrls.at(k).second, Qt::ElideRight,
                                   pageHistoryMenu->maximumWidth());
        QAction *action = new QAction(title, pageHistoryMenu);
        action->setData(visitedUrls.at(k).first);
        action->setShortcut(tr("Ctrl+%1").arg(k));
        connect(action, SIGNAL(triggered()), SLOT(openPageHistoryUrl()));
        pageHistoryItems.append(action);
        k++;
    }
}

void MainWindow::setWebHistory()
{
    for (const auto &item : pageHistoryItems) {
        pageHistoryMenu->addAction(item);
    }
}

void MainWindow::openPageHistoryUrl()
{
    QAction *action = qobject_cast<QAction *>(QObject::sender());
    view->load(action->data().toUrl());
}

void MainWindow::networkTimeOut()
{
    view->stop();
    view->setHtml(
        "<h1><font color=red>Network timeout (> 120 s)</h1>"
        "<h2>Failed to connect to the network</font></h2>"
        "Perhaps the proxy settings are wrong, or maybe a proxy is needed.");
}

void MainWindow::showHostInfo()
{
    // Provide info of the host IP-addresses
    QList<QNetworkInterface> iList = QNetworkInterface::allInterfaces();
    QString info;
    for (const auto &item : iList) {
        info += "<p><strong>Interface " + item.humanReadableName() +
                " </strong></p>";
        QList<QNetworkAddressEntry> addressList = item.addressEntries();
        for (const auto &address : addressList) {
            info += "&nbsp&nbsp&nbspIP-Address: " + address.ip().toString() +
                    "<br>";
        }
        info += "<br>";
    }

    view->stop();
    view->setHtml(info);
}

void MainWindow::showWebHistory()
{
    webHistoryView = std::unique_ptr<WebHistory>{ new WebHistory(visitedUrls) };
    connect(webHistoryView->getView(), SIGNAL(doubleClicked(QModelIndex)),
            SLOT(openClicked(QModelIndex)));
}

void MainWindow::clearWebHistory()
{
    QWebHistory *history = view->history();
    history->clear();
    for (const auto &item : pageHistoryItems) {
        pageHistoryMenu->removeAction(item);
    }
    pageHistoryItems.clear();
    visitedUrls.clear();
}

void MainWindow::addNewBookmark()
{
    bookmarkDB->addBookmark(view->title(), view->url());
    QAction *action = new QAction(view->title(), showLastAddedBookMarks);
    action->setData(view->url());
    connect(action, SIGNAL(triggered()), SLOT(openPageHistoryUrl()));
    showLastAddedBookMarks->addAction(action);
    bookmarks.append(action);
}

void MainWindow::loadBookmarks()
{
    auto bm = bookmarkDB->getBookmarks();
    if (!bm.isEmpty()) {
        auto iter = bm.constBegin();
        while (iter != bm.constEnd()) {
            QAction *action = new QAction(iter.key(), showLastAddedBookMarks);
            action->setData(iter.value());
            connect(action, SIGNAL(triggered()), SLOT(openPageHistoryUrl()));
            showLastAddedBookMarks->addAction(action);
            bookmarks.append(action);
            ++iter;
        }
    }
}

void MainWindow::httpFinished()
{
    // If a web page loading fails, give an error.
    if (reply->error()) {
        view->stop();
        QString title =
            tr("Error loading page: %1").arg(reply->url().toString());
        QString html = QString(
            "<html>"
            " <head>"
            "  <title>%1</title>"
            " </head>"
            " <body>"
            "    <h1><font color=red>%2</font></h1>"
            "    <h2>When connecting to: %3.</h2>"
            "    <ul>"
            "      <li>Check the address for errors such as "
            "<b>ww</b>.example.com"
            "      instead of <b>www</b>.example.com</li>"
            "      <li>If the address is correct, try checking the network"
            "      connection.</li>"
            "      <li>If your computer or network is protected by a firewall "
            "or"
            "      proxy, make sure that the browser demo is permitted to "
            "access"
            "      the network.</li>"
            "    </ul>"
            "    <br/><br/>"
            "  </div>"
            " </body>"
            "</html>")
                           .arg(title)
                           .arg(reply->errorString())
                           .arg(reply->url().toString());

        view->setHtml(html, reply->url());
        if (!visitedUrls.isEmpty()) {
            visitedUrls.removeFirst();
        }
    }
    reply->deleteLater();
}

void MainWindow::startRequest(const QUrl &url)
{
    QNetworkRequest request(url);
    reply = view->page()->networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
}

void MainWindow::openClicked(QModelIndex index)
{
    view->load(webHistoryView->getUrlForItem(index));
}
