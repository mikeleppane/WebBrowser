#include "bookmarkdb.h"
#include <QString>
#include <QUrl>
#include <QStandardPaths>
#include <QDir>
#include <QtSql/QSqlError>
#include <QDebug>

/* BookmarkDB uses SQLite database to store
 * all the bookmarks. It's a simple table in which
 * each record has two fields title and url. Title
 * represents web page title and url a address of
 * a web page.
*/

BookmarkDB::BookmarkDB(QObject *parent) : QObject(parent)
{
    createConnection();
}

void BookmarkDB::createConnection()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString databaseDir =
        QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    const QString dbFileName = QString("_mybookmarks.db");
    QDir dir(databaseDir);
    if (!QDir().mkpath(databaseDir)) {
        qWarning("Cannot create directory %s",
                 qPrintable(QStandardPaths::writableLocation(
                     QStandardPaths::DataLocation)));
        return;
    }
    db.setDatabaseName(dir.absoluteFilePath(dbFileName));
    if (!db.open())
        qFatal("Error while opening the database: %s",
               qPrintable(db.lastError().text()));

    createInitialData();
}

QSqlQuery BookmarkDB::prepare(const QString &statement) const
{
    QSqlQuery query(db);
    query.setForwardOnly(true);
    if (!query.prepare(statement)) {
        qWarning() << Q_FUNC_INFO << "failed to prepare query";
        qWarning() << query.lastQuery();
        qWarning() << query.lastError().text();
        return QSqlQuery();
    }
    return query;
}

bool BookmarkDB::execute(QSqlQuery &query) const
{
    if (!query.exec()) {
        qWarning() << Q_FUNC_INFO << "failed execute query";
        qWarning() << query.lastQuery();
        qWarning() << query.lastError().text();
        return false;
    }
    return true;
}

void BookmarkDB::createInitialData() const
{
    QSqlQuery query =
        prepare(QString("CREATE TABLE IF NOT EXISTS "
                        "Bookmarks (id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "title TEXT NOT NULL, "
                        "url TEXT NOT NULL);"));
    execute(query);
}

void BookmarkDB::addBookmark(const QString &title, const QUrl &url) const
{
    QSqlQuery query;
    query = prepare("INSERT INTO Bookmarks "
                    "(title, url) "
                    "VALUES (:title, :url);");
    query.bindValue(":title", title);
    query.bindValue(":url", url.toString());
    execute(query);
}

QMap<QString, QUrl> BookmarkDB::getBookmarks() const
{
    QSqlQuery query = prepare(QString("SELECT title, url FROM Bookmarks;"));
    QMap<QString, QUrl> bookmarks;
    if (!execute(query))
        return bookmarks;
    while (query.next()) {
        if (query.value(0) != Invalid && query.value(1) != Invalid) {
            bookmarks[query.value(0).toString()] = query.value(1).toUrl();
        }
    }
    return bookmarks;
}
