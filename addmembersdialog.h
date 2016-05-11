#ifndef ADDMEMBERSDIALOG_H
#define ADDMEMBERSDIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "databasemanager.h"

namespace Ui {
class AddMembersDialog;
}

class AddMembersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddMembersDialog(QWidget *parent = 0);
    Ui::AddMembersDialog *ui;
    void readSettings();
    int cancelValue;
    ~AddMembersDialog();

private:
    DatabaseManager *mpDbManager;
    QMessageBox::StandardButton reply;
    void SaveMemberBasicInfo(QString ref_no);
    void EditMemberBasicInfo();
    void closeEvent(QCloseEvent *event);


private slots:
    void Exit();
    void IsRefNoValid();

};

#endif // ADDMEMBERSDIALOG_H
