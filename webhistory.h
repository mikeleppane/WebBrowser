// This interface class provides access to web history
#ifndef WEBHISTORY_H
#define WEBHISTORY_H

#include <QMainWindow>
#include <QList>
#include <QPair>

class QTableView;
class QStandardItemModel;
class QUrl;
class QString;

class WebHistory : public QMainWindow
{
    Q_OBJECT
  public:
    explicit WebHistory(const QList<QPair<QUrl, QString> > &,
                        QMainWindow *parent = 0);
    QTableView *getView() const
    {
        return view;
    }
    QUrl getUrlForItem(const QModelIndex &) const;

  private:
    void createWidgets();
    void initializeModel(const QList<QPair<QUrl, QString> > &);

    QTableView *view;
    QStandardItemModel *model;
};

#endif // WEBHISTORY_H
