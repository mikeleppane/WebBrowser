#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <memory>
#include <QMap>
#include <QSet>
#include <QPair>
#include <QModelIndex>

namespace Ui {
class MainWindow;
}

class BookmarkDB;

constexpr int MaxWebHistoryItems() {return 10;}

class QWebView;
class QLineEdit;
class QProgressBar;
class QToolBar;
class QUrl;
class QMenu;
class QAction;
class QTimer;
class QNetworkAccessManager;
class QNetworkReply;
class WebHistory;
class QModelIndex;
class QTableView;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QUrl&);
    ~MainWindow();

signals:

private slots:
    void changePage();
    void setProgress(int);
    void setBrowserTitle();
    void finishedLoading(bool);
    void setWebHistory();
    void networkTimeOut();
    void showHostInfo();
    void showWebHistory();
    void clearWebHistory();
    void openPageHistoryUrl();
    void addNewBookmark();
    void httpFinished();
    void openClicked(QModelIndex);

private:
    Ui::MainWindow *ui;

    void createWidgets(const QUrl&);
    void createLayout();
    void createConnections();
    void createActions();
    void createMenus();
    void updatePageHistoryActionList();
    void loadBookmarks();
    void startRequest(const QUrl&);

    QWebView *view;
    QLineEdit *locationEdit;
    QProgressBar *progressBar;
    QToolBar *toolBar;
    QMenu *fileMenu;
    QMenu *pageHistoryMenu;
    QMenu *bookmarkMenu;
    QMenu *showLastAddedBookMarks;

    QAction *exitAction;
    QAction *showHistoryAction;
    QAction *clearHistoryAction;
    QAction *addNewBookmarkAction;
    QAction *hostInfoAction;
    QTimer *pageTimer;
    QList<QAction*> pageHistoryItems;
    QList<QAction*> bookmarks;
    QList<QPair<QUrl,QString>> visitedUrls;
    std::unique_ptr<BookmarkDB> bookmarkDB;
    std::unique_ptr<WebHistory> webHistoryView;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    int progress;
};

#endif // MAINWINDOW_H
