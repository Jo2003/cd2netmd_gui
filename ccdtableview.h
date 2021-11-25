#pragma once
#include <QObject>
#include <QTableView>
#include <QResizeEvent>

class CCDTableView : public QTableView
{
    Q_OBJECT
public:
    using QTableView::QTableView;
protected:
    void resizeEvent(QResizeEvent *e) override;
};
