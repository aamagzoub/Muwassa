#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtSql>



class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    QString db_invocation();
    QString db_table_creation();
    QSqlQueryModel *getRecordForEditOrFind(QString searchValue, int selectionMode, int switchChoise);
    QSqlQueryModel *getAllPayInfoForSelectedMonth(int selectedMonthInGui);

    bool db_data_deletion(QString ref_no);
    bool db_data_insertion(QString ref_no, QString name,
                           QString mother, QString tel, QString email, QString post_code,
                           QString address, QString spouse,
                           QString mother_in_law, QString childern,
                           QString membership_Date, QString payment_method, QString payment);
    bool db_data_update(QString ref_no, QString name,
                        QString mother, QString tel, QString email, QString post_code,
                        QString address, QString spouse,
                        QString mother_in_law, QString childern,
                        QString membership_Date, QString payment_method, QString payment);
    QSqlQueryModel *showSlectedMonthDefaults(int selectedMonthInGui);
    //QSqlQueryModel *showOneMonthAlerts(QString selectedMonth);
    QSqlQueryModel *showTwoMonthsAlerts(QString selectedMonth);
    QSqlQueryModel *showThreeMonthsAlerts(QString selectedMonth);
    bool confirm_payment(QList<QString> refNosList, int currentMonthFromGui);
    bool correct_payment(QList<QString> refNosList, int currentMonthFromGui);
    bool checkRefNoExist(QString ref_no);
    QSqlQueryModel *getAlerts(int alertChoice, int defaultsChoice);
    QSqlQueryModel *getAllPaymentsInfo();
    QSqlQueryModel *getPaymentsHistory(QString selectedYear);

public:
    QSqlDatabase db;
    void setCurrentYear(int currentYear);


private:
    QString current_year;
    int current_month;

    QList<QString> allRefNo;
    QString getCurrentYear();
    QString decideMonthName(int monthAsInt);
    QString selectedMonthFromGui(int selectedMonth);
    void setCurrentMonth(int month);
    QString getCurrentmonth();
    void setAllRefNoList(QList<QString> allRefNoList);
    QList<QString> getAllRefNoList();
};

#endif // DATABASEMANAGER_H
