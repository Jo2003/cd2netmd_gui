#pragma once
#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class CCDDBEntriesDialog;
}

class CCDDBEntriesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CCDDBEntriesDialog(const QStringList& entries, QWidget *parent = nullptr);
    ~CCDDBEntriesDialog();
    QString request();

protected:
    void showEvent(QShowEvent *e) override;
    QStringList mEntries;
    QString mRequest;

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::CCDDBEntriesDialog *ui;
};
