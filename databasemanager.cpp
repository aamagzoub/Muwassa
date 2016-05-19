#include "databasemanager.h"
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QtSql>
#include <QSqlTableModel>
#include <QDesktopServices>

QString DatabaseManager::db_invocation()
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    appDataPath.append("/Muwassa");
    QDir dir;
    dir.mkdir(appDataPath);

    db = QSqlDatabase::addDatabase("QSQLITE");
    qDebug() << appDataPath;

    db.setDatabaseName(appDataPath+"/muwassa_db");
    if (!db.open()){
        return db.lastError().text();
    }

    setCurrentYear(QDate::currentDate().year());

    int current_month = 0;
    setCurrentMonth(current_month);

    QList<QString> all_refNo_list;
    setAllRefNoList(all_refNo_list);

    return "Database is open";
}

QString DatabaseManager::db_table_creation(){

    QSqlQuery query;
    QString qryTxt;

    query.prepare("CREATE TABLE IF NOT EXISTS basic_info"
                  "(refNo INTEGER UNIQUE PRIMARY KEY,"
                  "name VARCHAR(30), "
                  "mother VARCHAR(30), "
                  "tel VARCHAR(30), "
                  "email VARCHAR(30), "
                  "postCode VARCHAR(30), "
                  "address VARCHAR(30), "
                  "spouse VARCHAR(30), "
                  "motherInLaw VARCHAR(30), "
                  "childern  VARCHAR(30), "
                  "Date VARCHAR(30), "
                  "method VARCHAR(30), "
                  "payment VARCHAR(30), "
                  "status VARCHAR(30))");
    if(!query.exec()) {
        qDebug() << "Error" << query.lastError().text();
        return query.lastError().text();
    }


    qryTxt = "CREATE TABLE IF NOT EXISTS payments_" + getCurrentYear() + " "
                                                                         "(refNo INTEGER UNIQUE PRIMARY KEY,"
                                                                         "January VARCHAR(30), "
                                                                         "February VARCHAR(30), "
                                                                         "March VARCHAR(30), "
                                                                         "April VARCHAR(30), "
                                                                         "May VARCHAR(30), "
                                                                         "June VARCHAR(30), "
                                                                         "July VARCHAR(30), "
                                                                         "August VARCHAR(30), "
                                                                         "September VARCHAR(30), "
                                                                         "October VARCHAR(30), "
                                                                         "November VARCHAR(30), "
                                                                         "December  VARCHAR(30));";
    query.prepare(qryTxt);
    if(!query.exec()) {
        qDebug() << "Error" << query.lastError().text();
        return query.lastError().text();
    }

    qryTxt = "CREATE TABLE IF NOT EXISTS late_payments"
             "(refNo INTEGER UNIQUE PRIMARY KEY,"
             "unpaid_count VARCHAR(30))";
    query.prepare(qryTxt);
    if(!query.exec()) {
        qDebug() << "Error" << query.lastError().text();
        return query.lastError().text();
    }

    qryTxt = "INSERT INTO payments_" + getCurrentYear() + " (refNo) SELECT refNo FROM basic_info";
    query.prepare(qryTxt);
    query.exec();

    return "   قاعدة البيانات جاهزة";

}

bool DatabaseManager::db_data_insertion(QString refNo,QString name,
                                        QString mother,QString tel,QString email,QString postCode,
                                        QString address,QString spouse,
                                        QString motherInLaw,QString childern,QString Date,
                                        QString method, QString payment, QString status){
    QSqlQuery query;

    query.prepare("INSERT INTO basic_info (refNo, name, mother, "
                  "tel, email, postCode, address, "
                  "spouse, motherInLaw, childern, "
                  "Date, method, payment, status) "
                  "VALUES (:refNo, :name, :mother, "
                  ":tel, :email, :postCode, :address, "
                  ":spouse, :motherInLaw, :childern, "
                  ":Date, :method, :payment, :status)");
    query.bindValue(":refNo", refNo);
    query.bindValue(":name", name);
    query.bindValue(":mother", mother);
    query.bindValue(":tel", tel);
    query.bindValue(":email", email);
    query.bindValue(":postCode", postCode);
    query.bindValue(":address", address);
    query.bindValue(":spouse", spouse);
    query.bindValue(":motherInLaw", motherInLaw);
    query.bindValue(":childern", childern);
    query.bindValue(":Date", Date);
    query.bindValue(":method", method);
    query.bindValue(":payment", payment);
    query.bindValue(":status", status);
    if (!query.exec())
    {
        qDebug() << "Error" << query.lastError().text();
        return false;
    }

    query.prepare("INSERT INTO payments_" + getCurrentYear() + " (refNo) VALUES (:refNo)");
    query.bindValue(":refNo", refNo);
    if (!query.exec())
    {
        qDebug() << "Error" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::db_data_update(QString refNo,QString name,
                                     QString mother,QString tel,QString email,QString postCode,
                                     QString address,QString spouse,
                                     QString motherInLaw,QString childern,QString Date,
                                     QString method, QString payment, QString status){
    QSqlQuery query;

    query.prepare("UPDATE basic_info SET name = :name, "
                  "mother = :mother, tel = :tel, email = :email, postCode = :postCode, address = :address, "
                  "spouse = :spouse, motherInLaw = :motherInLaw, "
                  "childern = :childern, "
                  "Date = :Date, method = :method, payment = :payment, status = :status "
                  "WHERE refNo = :refNo");
    query.bindValue(":refNo", refNo);
    query.bindValue(":name", name);
    query.bindValue(":mother", mother);
    query.bindValue(":tel", tel);
    query.bindValue(":email", email);
    query.bindValue(":postCode", postCode);
    query.bindValue(":address", address);
    query.bindValue(":spouse", spouse);
    query.bindValue(":motherInLaw", motherInLaw);
    query.bindValue(":childern", childern);
    query.bindValue(":Date", Date);
    query.bindValue(":method", method);
    query.bindValue(":payment", payment);
    query.bindValue(":status", status);
    if (!query.exec())
    {
        qDebug() << "Error" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::db_data_deletion(QString refNo){

        QSqlQuery query;

        query.prepare("DELETE FROM payments_" + getCurrentYear() + " WHERE refNo = (:refNo)");
        query.bindValue(":refNo",refNo);
        if (!query.exec())
        {
            qDebug() << "Error" << query.lastError().text();
            return false;
        }
        return true;

}

bool DatabaseManager::checkRefNoExist(QString refNo){
    QSqlQuery query;
    query.prepare("SELECT * FROM basic_info WHERE refNo = :refNo");
    query.bindValue(":refNo", refNo);
    if (!query.exec())
    {
        qDebug() << "Error" << query.lastError().text();
        return false;
    } else {
        if(query.first()){
            return true;
        } else {
            return false;
        }
    }
}

QSqlQueryModel *DatabaseManager::getRecordForEditOrFind(QString searchValue, int selectionMode, int switchSearchCretieria){
    QSqlQuery query;
    QSqlQueryModel *model = new QSqlQueryModel;
    QString queryTxt;


    if (selectionMode == 1)
    {
        query.prepare("SELECT * FROM basic_info WHERE status = 'نشط' ORDER BY refNo");
        query.exec();
        model->setQuery(query);

        model->setHeaderData(0, Qt::Horizontal, tr(" ر . م "));
        model->setHeaderData(1, Qt::Horizontal, tr("الإسم"));
        model->setHeaderData(2, Qt::Horizontal, tr("إسم الوالدة"));
        model->setHeaderData(3, Qt::Horizontal, tr("التلفون"));
        model->setHeaderData(4, Qt::Horizontal, tr("البريد الإلكتروني"));
        model->setHeaderData(5, Qt::Horizontal, tr("الرمز البريدي"));
        model->setHeaderData(6, Qt::Horizontal, tr("العنوان"));
        model->setHeaderData(7, Qt::Horizontal, tr("إسم الزوج"));
        model->setHeaderData(8, Qt::Horizontal, tr("إسم والدة الزوج"));
        model->setHeaderData(9, Qt::Horizontal, tr("أسماء الأطفال"));
        model->setHeaderData(10, Qt::Horizontal, tr("تاريخ الإشتراك"));
        model->setHeaderData(11, Qt::Horizontal, tr("طريقة الدفع"));
        model->setHeaderData(12, Qt::Horizontal, tr("القيمة"));
        model->setHeaderData(13, Qt::Horizontal, tr("الاشتراك"));

    } else if (selectionMode == 2) {

        switch (switchSearchCretieria){
        case(0):
            //search by refNo
            queryTxt = "SELECT * FROM basic_info WHERE refNo = " + searchValue ;
            model->setQuery(queryTxt);
            break;
        case(1):
            //search by name
            queryTxt = "SELECT * FROM basic_info WHERE name LIKE '%" + searchValue + "%' ORDER BY refNo";
            model->setQuery(queryTxt);
            break;
        case(2):
            //search by tel
            queryTxt = "SELECT * FROM basic_info WHERE tel = \"" + searchValue + "\" ORDER BY refNo" ;
            model->setQuery(queryTxt);
            break;
        case(3):
            //search by method
            queryTxt = "SELECT * FROM basic_info WHERE method LIKE '%" + searchValue + "%' ORDER BY refNo";
            model->setQuery(queryTxt);
            break;
        case(4):
            //search by method
            queryTxt = "SELECT * FROM basic_info WHERE payment = \"" + searchValue + "\" ORDER BY refNo" ;
            model->setQuery(queryTxt);
            break;
        }

        model->setHeaderData(0, Qt::Horizontal, tr(" ر . م "));
        model->setHeaderData(1, Qt::Horizontal, tr("الإسم"));
        model->setHeaderData(2, Qt::Horizontal, tr("إسم الوالدة"));
        model->setHeaderData(3, Qt::Horizontal, tr("التلفون"));
        model->setHeaderData(4, Qt::Horizontal, tr("البريد الإلكتروني"));
        model->setHeaderData(5, Qt::Horizontal, tr("الرمز البريدي"));
        model->setHeaderData(6, Qt::Horizontal, tr("العنوان"));
        model->setHeaderData(7, Qt::Horizontal, tr("إسم الزوج"));
        model->setHeaderData(8, Qt::Horizontal, tr("إسم والدة الزوج"));
        model->setHeaderData(9, Qt::Horizontal, tr("أسماء الأطفال"));
        model->setHeaderData(10, Qt::Horizontal, tr("تاريخ الإشتراك"));
        model->setHeaderData(11, Qt::Horizontal, tr("طريقة الدفع"));
        model->setHeaderData(12, Qt::Horizontal, tr("القيمة"));
        model->setHeaderData(13, Qt::Horizontal, tr("الاشتراك"));
    }
    return model;
}

QSqlQueryModel *DatabaseManager::showSlectedMonthDefaults(int selectedMonthInGui){
    QSqlQueryModel *model = new QSqlQueryModel;
    QString selectedMonth = selectedMonthFromGui(selectedMonthInGui);

    QString qryTxt = "SELECT payments_" + getCurrentYear() + ".refNo, "+
            "Date, name, "+
            "payments_" + getCurrentYear() + "." + selectedMonth +
            ",method, payment "+
            "FROM payments_" + getCurrentYear() + ", basic_info "+
            "ON payments_" + getCurrentYear() + ".refNo = basic_info.refNo "+
            "WHERE payments_" + getCurrentYear() + "." + selectedMonth + " IS NULL "+
            "AND basic_info.status = 'نشط' "+
            "ORDER BY payments_" + getCurrentYear() + ".refNo";

    model->setQuery(qryTxt);

    model->setHeaderData(0, Qt::Horizontal, tr(" ر . م "));
    model->setHeaderData(1, Qt::Horizontal, tr("تاريخ الإشتراك"));
    model->setHeaderData(2, Qt::Horizontal, tr("الإسم"));
    model->setHeaderData(3, Qt::Horizontal, tr("حالة الدفع"));
    model->setHeaderData(4, Qt::Horizontal, tr("طريقة الدفع"));
    model->setHeaderData(5, Qt::Horizontal, tr("القيمة"));

    return model;
}

QSqlQueryModel *DatabaseManager::getAllPayInfoForSelectedMonth(int selectedMonthInGui){

    QSqlQueryModel *model = new QSqlQueryModel;
    QSqlQuery query;
    QString selectedMonth = selectedMonthFromGui(selectedMonthInGui);
    query.prepare("SELECT payments_" + getCurrentYear() + ".refNo, "+
                  "Date, name, "+
                  "payments_" + getCurrentYear() + "." + selectedMonth +
                  ",method, payment "+
                  "FROM payments_" + getCurrentYear() + ", basic_info "+
                  "WHERE payments_" + getCurrentYear() + ".refNo = basic_info.refNo "+
                  "AND basic_info.status = 'نشط' "+
                  "ORDER BY basic_info.refNo");
    query.exec();
    model->setQuery(query);

    model->setHeaderData(0, Qt::Horizontal, tr(" ر . م "));
    model->setHeaderData(1, Qt::Horizontal, tr("تاريخ الإشتراك"));
    model->setHeaderData(2, Qt::Horizontal, tr("الإسم"));
    model->setHeaderData(3, Qt::Horizontal, tr("حالة الدفع"));
    model->setHeaderData(4, Qt::Horizontal, tr("طريقة الدفع"));
    model->setHeaderData(5, Qt::Horizontal, tr("القيمة"));

    return model;

}

bool DatabaseManager::confirm_payment(QList<QString> refNosList, int selectedMonth){

    QSqlQuery query;
    QString qryTxt;
    QString month = selectedMonthFromGui(selectedMonth);

    for (int i = 0; i < refNosList.size(); i++){
        qryTxt = "UPDATE payments_" + getCurrentYear() +
                " SET " + month + " = 'تم الدفع' WHERE refNo = " + refNosList[i];
        query.prepare(qryTxt);
        if (!query.exec())
        {
            qDebug() << "Error" << query.lastError().text();
            return false;
        }
    }
    return true;
}

bool DatabaseManager::correct_payment(QList<QString> refNosList, int selectedMonth){

    QSqlQuery query;
    QString qryTxt;
    QString month = selectedMonthFromGui(selectedMonth);

    for (int i = 0; i < refNosList.size(); i++){
        qryTxt = "UPDATE payments_" + getCurrentYear() +
                " SET " + month + " = '' WHERE refNo = " + refNosList[i];
        query.prepare(qryTxt);
        if (!query.exec())
        {
            qDebug() << "Error" << query.lastError().text();
            return false;
        }
    }
    return true;
}

QSqlQueryModel *DatabaseManager::getAlerts(int alertChoice, int defaultsChoice){

    QSqlQueryModel *model = new QSqlQueryModel;
    QSqlQuery query;
    QString qryTxt;

    qryTxt = "SELECT payments_" + getCurrentYear() + ".refNo, "+
            "basic_info.Date, "+
            "basic_info.name, "+
            "payments_"+getCurrentYear()+".January, "+
            "payments_"+getCurrentYear()+".February, "+
            "payments_"+getCurrentYear()+".March, "+
            "payments_"+getCurrentYear()+".April, "+
            "payments_"+getCurrentYear()+".May, "+
            "payments_"+getCurrentYear()+".June, "+
            "payments_"+getCurrentYear()+".July, "+
            "payments_"+getCurrentYear()+".August, "+
            "payments_"+getCurrentYear()+".September, "+
            "payments_"+getCurrentYear()+".October, "+
            "payments_"+getCurrentYear()+".November, "+
            "payments_"+getCurrentYear()+".December, "+
            "((ifnull(payments_" + getCurrentYear() + ".January, '') != 'تم الدفع') + "+
            "(ifnull(payments_" + getCurrentYear() + ".February, '') != 'تم الدفع') + "+
            "(ifnull(payments_" + getCurrentYear() + ".March, '') != 'تم الدفع') + "+
            "(ifnull(payments_" + getCurrentYear() + ".April, '') != 'تم الدفع') + "+
            "(ifnull(payments_" + getCurrentYear() + ".May, '') != 'تم الدفع') + "+
            "(ifnull(payments_" + getCurrentYear() + ".June, '') != 'تم الدفع') + "+
            "(ifnull(payments_" + getCurrentYear() + ".July, '') != 'تم الدفع') + "+
            "(ifnull(payments_" + getCurrentYear() + ".August, '') != 'تم الدفع') + "+
            "(ifnull(payments_" + getCurrentYear() + ".September, '') != 'تم الدفع') + "+
            "(ifnull(payments_" + getCurrentYear() + ".October, '') != 'تم الدفع') + "+
            "(ifnull(payments_" + getCurrentYear() + ".November, '') != 'تم الدفع') + "+
            "(ifnull(payments_" + getCurrentYear() + ".December, '') != 'تم الدفع')) AS unpaid_count "+
            "FROM payments_" + getCurrentYear() + ", basic_info ";

    if (defaultsChoice == 4) {
        qryTxt = qryTxt + "WHERE unpaid_count > " + QString::number(alertChoice) + " "+
                "AND payments_" + getCurrentYear() + ".refNo = basic_info.refNo "+
                "AND basic_info.status = 'نشط' "+
                "GROUP BY payments_" + getCurrentYear() + ".refNo "+
                "ORDER BY unpaid_count";
    } else {
        qryTxt = qryTxt + "WHERE unpaid_count = " + QString::number(alertChoice) + " "+
                "AND payments_" + getCurrentYear() + ".refNo = basic_info.refNo "+
                "AND basic_info.status = 'نشط' "+
                "GROUP BY payments_" + getCurrentYear() + ".refNo "+
                "ORDER BY payments_" + getCurrentYear() + ".refNo";
    }
    query.prepare(qryTxt);
    if (!query.exec())
    {
        qDebug() << "Error" << query.lastError().text();
    }

    model->setQuery(query);

    model->setHeaderData(0, Qt::Horizontal, tr(" ر . م "));
    model->setHeaderData(1, Qt::Horizontal, tr("تاريخ الإشتراك"));
    model->setHeaderData(2, Qt::Horizontal, tr("الإسم"));
    model->setHeaderData(3, Qt::Horizontal, tr("  يناير  "));
    model->setHeaderData(4, Qt::Horizontal, tr("  فبراير  "));
    model->setHeaderData(5, Qt::Horizontal, tr("  مارس  "));
    model->setHeaderData(6, Qt::Horizontal, tr("  إبريل  "));
    model->setHeaderData(7, Qt::Horizontal, tr("  مايو  "));
    model->setHeaderData(8, Qt::Horizontal, tr("  يونيو  "));
    model->setHeaderData(9, Qt::Horizontal, tr("  يوليو  "));
    model->setHeaderData(10, Qt::Horizontal, tr("  أغسطس  "));
    model->setHeaderData(11, Qt::Horizontal, tr("  سبتمبر  "));
    model->setHeaderData(12, Qt::Horizontal, tr("  أكتوبر  "));
    model->setHeaderData(13, Qt::Horizontal, tr("  نوفمبر  "));
    model->setHeaderData(14, Qt::Horizontal, tr("  ديسمبر  "));


    return model;
}

void DatabaseManager::setAllRefNoList(QList<QString> all_refNo_list){

    QSqlQueryModel *model = new QSqlQueryModel;

    model->setQuery("SELECT * FROM basic_info");

    for(int i=0; i<=(model->rowCount(QModelIndex())-1);i++){
        all_refNo_list << model->record(i).value(0).toString();
    }

    allRefNo = all_refNo_list;
}

QList<QString> DatabaseManager::getAllRefNoList(){
    return allRefNo;
}



QSqlQueryModel *DatabaseManager::getAllPaymentsInfo(){
    QSqlQueryModel *model = new QSqlQueryModel;

    model->setQuery("SELECT payments_"+getCurrentYear()+".refNo, "
                    "basic_info.Date, "+
                    "basic_info.name, "+
                    "payments_"+getCurrentYear()+".January, "+
                    "payments_"+getCurrentYear()+".February, "+
                    "payments_"+getCurrentYear()+".March, "+
                    "payments_"+getCurrentYear()+".April, "+
                    "payments_"+getCurrentYear()+".May, "+
                    "payments_"+getCurrentYear()+".June, "+
                    "payments_"+getCurrentYear()+".July, "+
                    "payments_"+getCurrentYear()+".August, "+
                    "payments_"+getCurrentYear()+".September, "+
                    "payments_"+getCurrentYear()+".October, "+
                    "payments_"+getCurrentYear()+".November, "+
                    "payments_"+getCurrentYear()+".December "+
                    "FROM payments_"+getCurrentYear()+", basic_info "+
                    "ON payments_"+getCurrentYear()+".refNo = basic_info.refNo "+
                    "WHERE payments_"+getCurrentYear()+".refNo = basic_info.refNo " +
                    "AND status = 'نشط'");


    model->setHeaderData(0, Qt::Horizontal, tr(" ر . م "));
    model->setHeaderData(1, Qt::Horizontal, tr("تاريخ الإشتراك"));
    model->setHeaderData(2, Qt::Horizontal, tr("الإسم"));
    model->setHeaderData(3, Qt::Horizontal, tr("  يناير  "));
    model->setHeaderData(4, Qt::Horizontal, tr("  فبراير  "));
    model->setHeaderData(5, Qt::Horizontal, tr("  مارس  "));
    model->setHeaderData(6, Qt::Horizontal, tr("  إبريل  "));
    model->setHeaderData(7, Qt::Horizontal, tr("  مايو  "));
    model->setHeaderData(8, Qt::Horizontal, tr("  يونيو  "));
    model->setHeaderData(9, Qt::Horizontal, tr("  يوليو  "));
    model->setHeaderData(10, Qt::Horizontal, tr("  أغسطس  "));
    model->setHeaderData(11, Qt::Horizontal, tr("  سبتمبر  "));
    model->setHeaderData(12, Qt::Horizontal, tr("  أكتوبر  "));
    model->setHeaderData(13, Qt::Horizontal, tr("  نوفمبر  "));
    model->setHeaderData(14, Qt::Horizontal, tr("  ديسمبر  "));

    return model;
}

QSqlQueryModel *DatabaseManager::getPaymentsHistory(QString selectedYear){
    QSqlQueryModel *model = new QSqlQueryModel;

    model->setQuery("SELECT payments_"+selectedYear+".refNo, "
                    "basic_info.membership_Dat, "+
                    "basic_info.name, "+
                    "payments_"+selectedYear+".January, "+
                    "payments_"+selectedYear+".February, "+
                    "payments_"+selectedYear+".March, "+
                    "payments_"+selectedYear+".April, "+
                    "payments_"+selectedYear+".May, "+
                    "payments_"+selectedYear+".June, "+
                    "payments_"+selectedYear+".July, "+
                    "payments_"+selectedYear+".August, "+
                    "payments_"+selectedYear+".September, "+
                    "payments_"+selectedYear+".October, "+
                    "payments_"+selectedYear+".November, "+
                    "payments_"+selectedYear+".December "+
                    "FROM payments_"+selectedYear+", basic_info "+
                    "ON payments_"+selectedYear+".refNo = basic_info.refNo "+
                    "WHERE payments_"+selectedYear+".refNo = basic_info.refNo");


    model->setHeaderData(0, Qt::Horizontal, tr(" ر . م "));
    model->setHeaderData(1, Qt::Horizontal, tr("تاريخ الإشتراك"));
    model->setHeaderData(2, Qt::Horizontal, tr("الإسم"));
    model->setHeaderData(3, Qt::Horizontal, tr("  يناير  "));
    model->setHeaderData(4, Qt::Horizontal, tr("  فبراير  "));
    model->setHeaderData(5, Qt::Horizontal, tr("  مارس  "));
    model->setHeaderData(6, Qt::Horizontal, tr("  إبريل  "));
    model->setHeaderData(7, Qt::Horizontal, tr("  مايو  "));
    model->setHeaderData(8, Qt::Horizontal, tr("  يونيو  "));
    model->setHeaderData(9, Qt::Horizontal, tr("  يوليو  "));
    model->setHeaderData(10, Qt::Horizontal, tr("  أغسطس  "));
    model->setHeaderData(11, Qt::Horizontal, tr("  سبتمبر  "));
    model->setHeaderData(12, Qt::Horizontal, tr("  أكتوبر  "));
    model->setHeaderData(13, Qt::Horizontal, tr("  نوفمبر  "));
    model->setHeaderData(14, Qt::Horizontal, tr("  ديسمبر  "));


    return model;
}

void DatabaseManager::setCurrentYear(int currentYear){
    current_year = QString::number(currentYear);
}

QString DatabaseManager::getCurrentYear(){
    return current_year;
}

void DatabaseManager::setCurrentMonth(int month){
    month = QDate::currentDate().month();
    current_month = month;
}

QString DatabaseManager::getCurrentmonth(){

    QString monthAsQString;
    int monthAsInt;

    monthAsInt = current_month;
    monthAsQString = decideMonthName(monthAsInt);
    return monthAsQString;
}

QString DatabaseManager::selectedMonthFromGui(int monthIndexOnGui){

    QString monthAsQString;
    int monthAsInt;

    monthAsInt = monthIndexOnGui;
    monthAsQString = decideMonthName(monthAsInt);
    return monthAsQString;
}

QString DatabaseManager::decideMonthName(int monthAsInt){

    QString monthAsQString;
    switch(monthAsInt){
    case(1):
        monthAsQString = "January";
        break;
    case(2):
        monthAsQString = "February";
        break;
    case(3):
        monthAsQString = "March";
        break;
    case(4):
        monthAsQString = "April";
        break;
    case(5):
        monthAsQString = "May";
        break;
    case(6):
        monthAsQString = "June";
        break;
    case(7):
        monthAsQString = "July";
        break;
    case(8):
        monthAsQString = "August";
        break;
    case(9):
        monthAsQString = "September";
        break;
    case(10):
        monthAsQString = "October";
        break;
    case(11):
        monthAsQString = "November";
        break;
    case(12):
        monthAsQString = "December";
        break;
    }
    return monthAsQString;
}

QSqlQueryModel *DatabaseManager::getInactiveMembers(){
    QSqlQuery query;
    QSqlQueryModel *model = new QSqlQueryModel;

        query.prepare("SELECT * FROM basic_info WHERE status = 'غير نشط' ORDER BY refNo");
        query.exec();
        model->setQuery(query);

        model->setHeaderData(0, Qt::Horizontal, tr(" ر . م "));
        model->setHeaderData(1, Qt::Horizontal, tr("الإسم"));
        model->setHeaderData(2, Qt::Horizontal, tr("إسم الوالدة"));
        model->setHeaderData(3, Qt::Horizontal, tr("التلفون"));
        model->setHeaderData(4, Qt::Horizontal, tr("البريد الإلكتروني"));
        model->setHeaderData(5, Qt::Horizontal, tr("الرمز البريدي"));
        model->setHeaderData(6, Qt::Horizontal, tr("العنوان"));
        model->setHeaderData(7, Qt::Horizontal, tr("إسم الزوج"));
        model->setHeaderData(8, Qt::Horizontal, tr("إسم والدة الزوج"));
        model->setHeaderData(9, Qt::Horizontal, tr("أسماء الأطفال"));
        model->setHeaderData(10, Qt::Horizontal, tr("تاريخ الإشتراك"));
        model->setHeaderData(11, Qt::Horizontal, tr("طريقة الدفع"));
        model->setHeaderData(12, Qt::Horizontal, tr("القيمة"));
        model->setHeaderData(13, Qt::Horizontal, tr("الاشتراك"));

    return model;
}

bool DatabaseManager::deactivateMembership(QString refNo){
    QSqlQuery query;
    query.prepare("UPDATE basic_info SET status = 'غير نشط' WHERE refNo = :refNo");
    query.bindValue(":refNo", refNo);
    if(!query.exec()) {
        qDebug() << "Error" << query.lastError().text();
        return false;
    } else {
        return true;
    }

}

bool DatabaseManager::activateMembership(QString refNo){
    QSqlQuery query;
    query.prepare("UPDATE basic_info SET status = 'نشط' WHERE refNo = :refNo");
    query.bindValue(":refNo", refNo);
    if(!query.exec()) {
        qDebug() << "Error" << query.lastError().text();
        return false;
    } else {
        return true;
    }
}

bool DatabaseManager::isMemStatusAct(QString refNo){
    QSqlQuery query;
    QSqlQueryModel *model = new QSqlQueryModel;

    query.prepare("SELECT status FROM basic_info WHERE refNo = :refNo");
    query.bindValue(":refNo", refNo);
    query.exec();
    model->setQuery(query);

    if(model->record(0).value(0).toString()== "نشط"){
        return true;
    } else {
        return false;
    }

}
