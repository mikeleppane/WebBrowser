#include <QTableView>
#include <QStandardItemModel>
#include <QUrl>
#include <QString>
#include <QStandardItem>
#include <QHeaderView>
#include <QBrush>
#include <QColor>
#include <QFont>

#include "webhistory.h"

WebHistory::WebHistory(const QList<QPair<QUrl, QString> > &items,
                       QMainWindow *parent)
    : QMainWindow(parent)
{
    initializeModel(items);
    createWidgets();
    setCentralWidget(view);
    setWindowTitle(tr("Web history archive"));
    resize(640, 480);
    show();
}

void WebHistory::createWidgets()
{
    // Set up tableview and initialize some properties
    // I just realized that I have not used style sheets
    // before...wonder why they are great.
    view = new QTableView(this);
    view->setWindowTitle(tr("Web history archive"));
    view->verticalHeader()->hide();
    view->setStyleSheet("QHeaderView::section {background: lightblue;"
                        "font-family: Verdana;"
                        "font-size: 16px;"
                        "font-weight: bold}");
    view->horizontalHeader()->setStretchLastSection(true);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    view->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    view->setSelectionBehavior(QTableView::SelectRows);
    view->setSortingEnabled(true);

    view->setModel(model);
}

void WebHistory::initializeModel(const QList<QPair<QUrl, QString> > &history)
{
    model = new QStandardItemModel();
    model->setHorizontalHeaderLabels((QStringList() << "Name"
                                                    << "Address"));
    model->setColumnCount(2);
    int k = 0;
    for (const auto &historyItem : history) {
        QStandardItem *item1 = new QStandardItem(historyItem.second);
        item1->setEditable(false);
        QStandardItem *item2 = new QStandardItem(historyItem.first.toString());
        item2->setEditable(false);
        QFont font("Verdana", 10);
        item1->setFont(font);
        item2->setFont(font);
        // paint every second item's background with different color
        if (k % 2) {
            item1->setBackground(QBrush(QColor(135,206,250)));
            item2->setBackground(QBrush(QColor(135,206,250)));
        }
        model->appendRow(QList<QStandardItem *>() << item1 << item2);
        k++;
    }
}

QUrl WebHistory::getUrlForItem(const QModelIndex &index) const
{
    return QUrl(model->item(index.row(), 1)->text());
}
