// This interface class stores given bookmarks
#ifndef BOOKMARKDB_H
#define BOOKMARKDB_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QMap>
#include <QVariant>

class QString;
class QUrl;

class BookmarkDB : public QObject
{
    Q_OBJECT
public:
    explicit BookmarkDB(QObject *parent = 0);
    void addBookmark(const QString&, const QUrl&) const;
    QMap<QString,QUrl> getBookmarks() const;

private:
    QSqlQuery prepare(const QString& statement) const;
    bool execute(QSqlQuery &query) const;
    void createConnection();
    void createInitialData() const;
    const QVariant Invalid;
    QSqlDatabase db;

};

#endif // BOOKMARKDB_H
