#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QMessageBox>
#include "databasemanager.h"
#include "EmailingService.h"
#include "printingservice.h"
#include "addmembersdialog.h"
#include "ui_addmembersdialog.h"

#include <QTableView>

namespace Ui {
class MainWindow;
}

class newMember;
class DatabaseManager;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    DatabaseManager *mpDbManager;
    EmailingService *mpEmailingService;
    PrintingService *mpPrintingService;
    QSqlQueryModel *mpShowingModel;
    QSqlQueryModel *mpReturnedRecord;
    QSqlQueryModel *mpSearchModel;
    QSqlQueryModel *mpPaymentsToConfirmModel;
    QSqlQueryModel *mpMonthAlertModel;
    AddMembersDialog *mpAddMembersDialog;
    QModelIndexList mpSelectedRowsIndexesList;
    QMessageBox::StandardButton reply;

    int currentMonth;
    int userAction;
    int alertChoice;
    int pyamentTrackShowMode;
    int defaulMonthCount;
    int setViewEditChoice;
    QString selectedYear;

    void setCurrentMonth();
    int getCurrentMonth();
    void setUserAction(int action);
    int getUserAction();
    void setAlertChoice(int choice);
    int getAlertChoice();
    void setPaymentTrackShowMode(int mode);
    int getPaymentTrackShowMode();
    void setDefaultMonthCount(int monthCount);
    int getDefaultMonthCount();
    void setYear();
    QString getYear();

    void printThis(QTableView *tableToPrint);
    void setSearchItemsInvisible();
    void clearAddMemberFields();
    void DisnableEditRemoveBtn();
    void enableFeatures();
    void disableFeatures();
    void setConfCorrPayVisible();
    void setConfCorrPayInvisible();
    void setDisplayBtnsVisible();
    void setDisplayBtnsInvisible();
    void hideColumns();

private slots:
    void Exit();
    void displayAllInPaymentTab();
    void FindMember();
    void EditMemberBasicInfo();
    void EnableEditRemoveBtn();
    void SelectedRows();
    void DeleteMember();
    void ConfirmPayment();
    void displaySlectedMonthDefaults();
    void EnableBtns();
    void GetOneMonthAlerts();
    void GetTwoMonthsAlerts();
    void GetThreeMonthsAlerts();
    void showActiveMembers();
    void enableSearchFields();
    void displayAlertsOnGui(QSqlQueryModel *mMonthAlertModel);
    void displayAllmembersAllYear(QSqlQueryModel *monthAlertModel);
    void CorrectPayment();
    void GetHigherThanThreeAlerts();
    void displayTableOnGui(QSqlQueryModel *mpShowingModel);
    void printPaymentInfo();
    void printBasicInfo();
    void showPaymentDetails();
    void showPaymentHistory();
    void displayPaymentHistory(QSqlQueryModel *monthAlertModel);
    void sendEmail();
    void openAddMemberForm();
    void setSearchItemsVisible();
    void setPaymentsItemsInvislable();
    void enableGoSearchBtn();
    void disabledAddMemberFields();
    void enabledAddMemberFields();
    void startEditing();
    void startViewing();
    void startAddition();
    void checkYear();
    void displaySlectedMemberPayments();
    void showInactiveMembers();
    void deactivateMembership();
    void activateMembership();
    bool isMemStatusAct();
};

#endif // MAINWINDOW_H
