#include "ccdtableview.h"

void CCDTableView::resizeEvent(QResizeEvent *e)
{
    QTableView::resizeEvent(e);
    setColumnWidth(0, (width() / 100) * 80);
    setColumnWidth(1, (width() / 100) * 15);
}
