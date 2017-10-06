#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_addmembersdialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QtSql>
#include <QCheckBox>
#include <QPrinter.h>
#include <QPrintDialog>
#include <QTextDocument>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("موآساة");

    mpDbManager = new DatabaseManager;
    mpEmailingService = new EmailingService;

    ui->appStatus->setText(mpDbManager->db_invocation());
    ui->appStatus->setText(mpDbManager->db_table_creation());

    ui->label_3->setStyleSheet("QLabel { background-color : black; color : white}");
    ui->label_6->setStyleSheet("QLabel { background-color : black; color : white}");
    ui->label_9->setStyleSheet("QLabel { background-color : black; color : white}");
    ui->label_10->setStyleSheet("QLabel { background-color : black; color : white}");
    ui->label_13->setStyleSheet("QLabel { background-color : black; color : white}");

    ui->appStatus->setStyleSheet("QLabel { background-color :white ; color : #000000; border-radius: 10px}");
    ui->appStatus_2->setStyleSheet("QLabel { background-color : darkCyan; color : white; border-radius: 10px}");
    ui->appStatus_3->setStyleSheet("QLabel { background-color : darkCyan; color : white; border-radius: 10px}");
    ui->appStatus_6->setStyleSheet("QLabel { background-color : darkCyan; color : white; border-radius: 10px}");
    ui->exitBtn->setStyleSheet("QPushButton {background-color: black; color: white; "
                               "border-color: darkCyan; border-radius: 10px}"
                               "QPushButton:pressed { background-color : darkCyan; color:white }");
    qApp->setStyleSheet("QTableView {alternate-background-color: #ecf0f1; background-color: "
                        "white;selection-background-color: darkCyan}");

    ui->basicInfoTV->setStyleSheet("QHeaderView::section { background-color :#E5E4E2 ; color : black }");
    ui->paymentsTV->setStyleSheet("QHeaderView::section { background-color :#E5E4E2 ; color : black }");

    ui->selectYearSb->setMinimum(2014);
    ui->selectYearSb->setMaximum((QDate::currentDate().year()));
    ui->selectYearSb->setValue((QDate::currentDate().year()));
    ui->selectMethodSB->setVisible(false);
    ui->basicInfoTV->setTextElideMode(Qt::ElideMiddle);
    ui->memebershipDate->setDate(QDate::currentDate());

    setCurrentMonth();

    setYear();
    setSearchItemsInvisible();
    setPaymentsItemsInvislable();
    showActiveMembers();
    showCurrentPayments();

    connect(ui->searchBy,SIGNAL(currentIndexChanged(QString)),this,SLOT(enableSearchFields()));
    connect(ui->findBtn,SIGNAL(clicked()),this,SLOT(FindMember()));
    connect(ui->findBtn_2,SIGNAL(clicked()),this,SLOT(setSearchItemsVisible()));
    connect(ui->editBtn,SIGNAL(clicked()),this,SLOT(startEditing()));
    connect(ui->removeBtn,SIGNAL(clicked()),this,SLOT(DeleteMember()));
    connect(ui->basicInfoTV,SIGNAL(clicked(QModelIndex)),this,SLOT(EnableEditRemoveBtn()));
    connect(ui->basicInfoTV,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(startViewing()));
    connect(ui->paymentsTV,SIGNAL(clicked(QModelIndex)),this,SLOT(SelectedRows()));
    connect(ui->confirmPaymentBtn,SIGNAL(clicked()),this,SLOT(ConfirmPayment()));
    connect(ui->showAllBtn,SIGNAL(clicked()),this,SLOT(displayAllInPaymentTab()));
    connect(ui->defaultsPaymentBtn,SIGNAL(clicked()),this,SLOT(displaySlectedMonthDefaults()));
    connect(ui->monthToPayCb,SIGNAL(currentIndexChanged(int)),this,SLOT(EnableBtns()));
    connect(ui->oneMonthBtn,SIGNAL(clicked()),this,SLOT(GetOneMonthAlerts()));
    connect(ui->twoMonthsBtn,SIGNAL(clicked()),this,SLOT(GetTwoMonthsAlerts()));
    connect(ui->threeMonthsBtn,SIGNAL(clicked()),this,SLOT(GetThreeMonthsAlerts()));
    connect(ui->higherThanThreeBtn,SIGNAL(clicked()),this,SLOT(GetHigherThanThreeAlerts()));
    connect(ui->correctPaymentBtn,SIGNAL(clicked()),this,SLOT(CorrectPayment()));
    connect(ui->exitBtn,SIGNAL(clicked()),this,SLOT(Exit()));
    connect(ui->basicInfoPrintBtn,SIGNAL(clicked()),this,SLOT(printBasicInfo()));
    connect(ui->paymentsPrintBtn,SIGNAL(clicked()),this,SLOT(printPaymentInfo()));
    connect(ui->showPaymentsDetailsBtn,SIGNAL(clicked()),this,SLOT(showCurrentPayments()));
    connect(ui->addMemberBtn,SIGNAL(clicked()),this,SLOT(startAddition()));
    connect(ui->searchInput,SIGNAL(textEdited(QString)),this,SLOT(enableGoSearchBtn()));
    connect(ui->selectMethodSB,SIGNAL(currentIndexChanged(QString)),this,SLOT(enableGoSearchBtn()));
    connect(ui->selectYearSb,SIGNAL(valueChanged(QString)),this,SLOT(checkYear()));
    connect(ui->showAllMemberInfoBtn,SIGNAL(clicked()),this,SLOT(showActiveMembers()));
    connect(ui->inactiveMemBtn,SIGNAL(clicked()),this,SLOT(showInactiveMembers()));
    connect(ui->deactivateMemBtn,SIGNAL(clicked()),this,SLOT(deactivateMembership()));
    connect(ui->activateMemBtn,SIGNAL(clicked()),this,SLOT(activateMembership()));
    connect(ui->saveBtn,SIGNAL(clicked()),this,SLOT(IsRefNoValid()));


    switch(getCurrentMonth()){
    case(1):
        ui->oneMonthBtn->setEnabled(false);
        ui->twoMonthsBtn->setEnabled(false);
        ui->threeMonthsBtn->setEnabled(false);
        ui->higherThanThreeBtn->setEnabled(false);
        break;
    case(2):
        ui->twoMonthsBtn->setEnabled(false);
        ui->threeMonthsBtn->setEnabled(false);
        ui->higherThanThreeBtn->setEnabled(false);
        break;
    case(3):
        ui->threeMonthsBtn->setEnabled(false);
        ui->higherThanThreeBtn->setEnabled(false);
        break;
    }
}

void MainWindow::DisableAddMemebrBtn(){
    ui->addMemberBtn->setEnabled(false);
}

void MainWindow::startAddition(){
    setQMessageActionVaue(false);
    enabledAddMemberFields();
    clearAddMemberUiFields();
}

void MainWindow::IsRefNoValid(){
    QString ref_no;
    if (!ui->refNo->text().isEmpty()){
        ref_no = ui->refNo->text();
        SaveMemberBasicInfo(ref_no);
    } else {
        ui->statusLbl->setText("   الرجاء ادخال الرقم المتسلسل ...");
    }
}

void MainWindow::SaveMemberBasicInfo(QString ref_no){

    QString name = ui->name->text();
    QString mother = ui->mother->text();
    QString tel = ui->tel->text();
    QString email = ui->email->text();
    QString post_code = ui->postCode->text();
    QString address = ui->address->text();
    QString spouse = ui->spouse->text();
    QString mother_in_law = ui->motherInLaw->text();
    QString childern = ui->childern->text();
    QString membership_Date = ui->memebershipDate->text();
    QString payment = ui->payment->text();
    QString payment_method = ui->paymentMethod->currentText();
    QString status = ui->mem_state->text();

    if(!mpDbManager->checkRefNoExist(ref_no)){

        status = "نشط";

        if(!mpDbManager->db_data_insertion(ref_no,name,mother,tel,email,post_code,address,
                                           spouse,mother_in_law,childern,
                                           membership_Date,payment_method,payment,status)){
            ui->statusLbl->setText("Error: data insertion ...");
        } else {
            ui->refNo->clear();
            ui->name->clear();
            ui->mother->clear();
            ui->tel->clear();
            ui->email->clear();
            ui->postCode->clear();
            ui->address->clear();
            ui->spouse->clear();
            ui->motherInLaw->clear();
            ui->childern->clear();
            ui->memebershipDate->setDate(QDate::currentDate());

            ui->statusLbl->setText("   تم الحفظ المشترك رقم:  "+ ref_no + " بنجاح");
            if(getShowActOrInact()){
                showActiveMembers();
            } else {
                showInactiveMembers();
            }
            showCurrentPayments();
            setQMessageActionVaue(false);

        }

    } else {

        if(!getQMessageActionVaue()){
            reply = QMessageBox::information(this, "تحذير",
                                          " هذا الرقم المتسلسل موجود مسبقاً !!   ",
                                          QMessageBox::Ok);
        } else {

            bool currentStatus = mpDbManager->isMemStatusAct(ref_no);
            if(currentStatus){
                status = "نشط";
            } else {
                status = "غير نشط";
            }

            reply = QMessageBox::question(this, "تحديث",
                                          " هل فعلاً تريد/تريدين تحديث بيانات هذا المشترك ؟     ",
                                          QMessageBox::Yes|QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                if(!mpDbManager->db_data_update(ref_no,name,mother,tel,email,post_code,address,
                                                spouse,mother_in_law,childern,
                                                membership_Date,payment_method,payment,status)){
                    ui->statusLbl->setText("Error: data update ...");
                } else {
                    ui->refNo->clear();
                    ui->name->clear();
                    ui->mother->clear();
                    ui->tel->clear();
                    ui->email->clear();
                    ui->postCode->clear();
                    ui->address->clear();
                    ui->spouse->clear();
                    ui->motherInLaw->clear();
                    ui->childern->clear();
                    ui->memebershipDate->setDate(QDate::currentDate());

                    ui->statusLbl->setText("   تم التحديث بنجاح ");
                    if(getShowActOrInact()){
                        showActiveMembers();
                    } else {
                        showInactiveMembers();
                    }
                    showCurrentPayments();
                    setQMessageActionVaue(false);
                }
            } else {

            }

        }

    }
}

void MainWindow::setQMessageActionVaue(bool value){
    qMessageBoxActionValue = value;
}

bool MainWindow::getQMessageActionVaue(){
    return qMessageBoxActionValue;
}

void MainWindow::FindMember(){
    setUserAction(2);

    int search_creteria = ui->searchBy->currentIndex();

    switch (search_creteria){
    case 0:
        //search by reference no.
        mpSearchModel = mpDbManager->getRecordForEditOrFind(ui->searchInput->text(),getUserAction(),0);
        break;
    case 1:
        //search by name
        mpSearchModel = mpDbManager->getRecordForEditOrFind(ui->searchInput->text(),getUserAction(),1);
        break;
    case 2:
        //search by telephone No.
        mpSearchModel = mpDbManager->getRecordForEditOrFind(ui->searchInput->text(),getUserAction(),2);
        break;
    case 3:
        //search by payment method
        mpSearchModel = mpDbManager->getRecordForEditOrFind(ui->selectMethodSB->currentText(),getUserAction(),3);
        break;
    case 4:
        //search by monthly payment
        mpSearchModel = mpDbManager->getRecordForEditOrFind(ui->searchInput->text(),getUserAction(),4);
        break;
    }

    displayTableOnGui(mpSearchModel);
    ui->findBtn->setEnabled(false);
    ui->searchBy->setCurrentIndex(0);
    setSearchItemsInvisible();
}

void MainWindow::DeleteMember(){

    int row_index= ui->basicInfoTV->currentIndex().row();
    QModelIndex index = mpShowingModel->index(row_index, 0);
    QString ref_no = index.data().toString();

    reply = QMessageBox::question(this, "مسح","هل انت متأكد؟  ", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if(!mpDbManager->db_data_deletion(ref_no)){
            ui->appStatus->setText("Error: data deletion ...");
            showActiveMembers();
        } else{
            ui->appStatus->setText("   تم المسح بنجاح ");
            if(getShowActOrInact()){
                showActiveMembers();
            } else {
                showInactiveMembers();
            }
            showCurrentPayments();
        }
    }
    DisnableEditRemoveBtn();
}

void MainWindow::startViewing(){
    disabledAddMemberFields();
    fillMemberBasicInfoUi4EditOrView();
}

void MainWindow::startEditing(){
    setShowActOrInact(false);
    setQMessageActionVaue(true);
    enabledAddMemberFields();
    fillMemberBasicInfoUi4EditOrView();
}

void MainWindow::fillMemberBasicInfoUi4EditOrView(){

    clearAddMemberUiFields();

    QString refNo;
    QString name;
    QString mother;
    QString tel;
    QString email;
    QString postcode;
    QString address;
    QString spouse;
    QString mother_in_law;
    QString childern;
    QDate membership_Date;
    QString payment_method;
    QString status;
    int payment;
    QSqlQueryModel *modelToShow;

    int row_index= ui->basicInfoTV->currentIndex().row();
    QModelIndex ref_no_value = mpShowingModel->index(row_index, 0);
    QString ref_no_string = ref_no_value.data().toString();

    mpReturnedRecord = mpDbManager->getRecordForEditOrFind(ref_no_string, getUserAction(),-1);

    if (getUserAction() == 1){
        modelToShow = mpReturnedRecord;
    } else {
        modelToShow = mpSearchModel;
    }

    refNo = modelToShow->record(row_index).value(0).toString();
    name = modelToShow->record(row_index).value(1).toString();
    mother = modelToShow->record(row_index).value(2).toString();
    tel = modelToShow->record(row_index).value(3).toString();
    email = modelToShow->record(row_index).value(4).toString();
    postcode =  modelToShow->record(row_index).value(5).toString();
    address = modelToShow->record(row_index).value(6).toString();
    spouse = modelToShow->record(row_index).value(7).toString();
    mother_in_law = modelToShow->record(row_index).value(8).toString();
    childern = modelToShow->record(row_index).value(9).toString();
    QString date_temp = modelToShow->record(row_index).value(10).toString();
    membership_Date = QDate::fromString(date_temp,"yyyy/MM/dd");
    payment_method = modelToShow->record(row_index).value(11).toString();
    payment = modelToShow->record(row_index).value(12).toInt();
    status = modelToShow->record(row_index).value(13).toString();

    ui->refNo->setText(refNo);
    ui->name->setText(name);
    ui->mother->setText(mother);
    ui->tel->setText(tel);
    ui->email->setText(email);
    ui->postCode->setText(postcode);
    ui->address->setText(address);
    ui->spouse->setText(spouse);
    ui->motherInLaw->setText(mother_in_law);
    ui->childern->setText(childern);
    ui->memebershipDate->setDate(membership_Date);
    ui->paymentMethod->setCurrentText(payment_method);
    ui->payment->setValue(payment);
    ui->mem_state->setText(status);


    if(getShowActOrInact()){
        showActiveMembers();
    } else {
        showInactiveMembers();
    }

    showCurrentPayments();
    DisnableEditRemoveBtn();
}

void MainWindow::SelectedRows(){
    if(ui->defaultsPaymentBtn->isVisible() | ui->showAllBtn->isVisible()){
        setConfCorrPayVisible();
        ui->showPaymentsDetailsBtn->setEnabled(true);
    }
    mpSelectedRowsIndexesList = ui->paymentsTV->selectionModel()->selectedRows();

}

void MainWindow::ConfirmPayment(){

    QList<QString> refNosList;
    for (int i = 0; i < mpSelectedRowsIndexesList.size(); i++){
        refNosList << mpSelectedRowsIndexesList[i].data().toString();
    }
    reply = QMessageBox::question(this, "تأكيد الدفع","هل انت متأكد؟  ",QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if(!mpDbManager->confirm_payment(refNosList, ui->monthToPayCb->currentIndex())){
            ui->appStatus->setText("Error: payment are not confirmed ...");
        } else {
            ui->appStatus->setText("  تم تأكيد الدفع ");
            displayAllInPaymentTab();
        }
    } else {
        ui->appStatus->setText("  تم إلغاء تأكيد الدفع ");

    }
}

void MainWindow::CorrectPayment(){
    QList<QString> refNosList;
    for (int i = 0; i < mpSelectedRowsIndexesList.size(); i++){
        refNosList << mpSelectedRowsIndexesList[i].data().toString();
    }

    reply = QMessageBox::question(this, "تصحيح الدفع","هل انت متأكد؟  ",QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if(!mpDbManager->correct_payment(refNosList, ui->monthToPayCb->currentIndex())){
            ui->appStatus->setText("Error: payment are not confirmed ...");
        } else {
            ui->appStatus->setText("  تم تصحيح الدفع ");
            displayAllInPaymentTab();
        }
    } else {
        ui->appStatus->setText("  تم إلغاء تصحيح الدفع ");
    }
}



void MainWindow::GetOneMonthAlerts(){
    ui->showPaymentsDetailsBtn->setEnabled(true);
    setPaymentTrackShowMode(1);
    setAlertChoice(1);
    switch(getCurrentMonth()){
    case(2):
        setDefaultMonthCount(11);
        break;
    case(3):
        setDefaultMonthCount(10);
        break;
    case(4):
        setDefaultMonthCount(9);
        break;
    case(5):
        setDefaultMonthCount(8);
        break;
    case(6):
        setDefaultMonthCount(7);
        break;
    case(7):
        setDefaultMonthCount(6);
        break;
    case(8):
        setDefaultMonthCount(5);
        break;
    case(9):
        setDefaultMonthCount(4);
        break;
    case(10):
        setDefaultMonthCount(3);
        break;
    case(11):
        setDefaultMonthCount(2);
        break;
    case(12):
        setDefaultMonthCount(1);
        break;
    }

    mpMonthAlertModel = mpDbManager->getAlerts(getDefaultMonthCount(), getAlertChoice());
    displayAlertsOnGui(mpMonthAlertModel);
}

void MainWindow::GetTwoMonthsAlerts(){
    ui->showPaymentsDetailsBtn->setEnabled(true);
    setPaymentTrackShowMode(1);
    setAlertChoice(2);
    switch(getCurrentMonth()){
    case(3):
        setDefaultMonthCount(11);
        break;
    case(4):
        setDefaultMonthCount(10);
        break;
    case(5):
        setDefaultMonthCount(9);
        break;
    case(6):
        setDefaultMonthCount(8);
        break;
    case(7):
        setDefaultMonthCount(7);
        break;
    case(8):
        setDefaultMonthCount(6);
        break;
    case(9):
        setDefaultMonthCount(5);
        break;
    case(10):
        setDefaultMonthCount(4);
        break;
    case(11):
        setDefaultMonthCount(3);
        break;
    case(12):
        setDefaultMonthCount(2);
        break;
    }

    mpMonthAlertModel = mpDbManager->getAlerts(getDefaultMonthCount(), getAlertChoice());
    displayAlertsOnGui(mpMonthAlertModel);
}

void MainWindow::GetThreeMonthsAlerts(){
    ui->showPaymentsDetailsBtn->setEnabled(true);
    setPaymentTrackShowMode(1);
    setAlertChoice(3);
    switch(getCurrentMonth()){
    case(4):
        setDefaultMonthCount(11);
        break;
    case(5):
        setDefaultMonthCount(10);
        break;
    case(6):
        setDefaultMonthCount(9);
        break;
    case(7):
        setDefaultMonthCount(8);
        break;
    case(8):
        setDefaultMonthCount(7);
        break;
    case(9):
        setDefaultMonthCount(6);
        break;
    case(10):
        setDefaultMonthCount(5);
        break;
    case(11):
        setDefaultMonthCount(4);
        break;
    case(12):
        setDefaultMonthCount(3);
        break;
    }

    mpMonthAlertModel = mpDbManager->getAlerts(getDefaultMonthCount(), getAlertChoice());
    displayAlertsOnGui(mpMonthAlertModel);
}

void MainWindow::GetHigherThanThreeAlerts(){
    ui->showPaymentsDetailsBtn->setEnabled(true);
    setPaymentTrackShowMode(1);
    setAlertChoice(4);
    switch(getCurrentMonth()){
    case(4):
        setDefaultMonthCount(11);
        break;
    case(5):
        setDefaultMonthCount(10);
        break;
    case(6):
        setDefaultMonthCount(9);
        break;
    case(7):
        setDefaultMonthCount(8);
        break;
    case(8):
        setDefaultMonthCount(7);
        break;
    case(9):
        setDefaultMonthCount(6);
        break;
    case(10):
        setDefaultMonthCount(5);
        break;
    case(11):
        setDefaultMonthCount(4);
        break;
    case(12):
        setDefaultMonthCount(3);
        break;
    }

    mpMonthAlertModel = mpDbManager->getAlerts(getDefaultMonthCount(), getAlertChoice());
    displayAlertsOnGui(mpMonthAlertModel);
}

void MainWindow::showActiveMembers(){
    ui->deactivateMemBtn->show();
    ui->activateMemBtn->hide();
    setShowActOrInact(true);
    QString ref_no_string;
    setUserAction(1);
    ui->editBtn->setEnabled(false);
    ui->removeBtn->setEnabled(false);
    ui->activateMemBtn->setEnabled(true);
    ui->deactivateMemBtn->setEnabled(true);
    ui->showAllMemberInfoBtn->setVisible(false);
    ui->inactiveMemBtn->setVisible(true);
    mpShowingModel = mpDbManager->getRecordForEditOrFind(ref_no_string, getUserAction(),-1);
    displayTableOnGui(mpShowingModel);
}

void MainWindow::showInactiveMembers(){
    ui->deactivateMemBtn->hide();
    ui->activateMemBtn->show();
    setShowActOrInact(false);
    ui->editBtn->setEnabled(false);
    ui->removeBtn->setEnabled(false);
    ui->activateMemBtn->setEnabled(true);
    ui->deactivateMemBtn->setEnabled(true);
    ui->showAllMemberInfoBtn->setVisible(true);
    ui->inactiveMemBtn->setVisible(false);
    mpShowingModel = mpDbManager->getInactiveMembers();
    displayTableOnGui(mpShowingModel);
}

void MainWindow::showCurrentPayments(){
    isItHistory = false;
    ui->showPaymentsDetailsBtn->setEnabled(false);
    ui->monthToPayCb->setCurrentIndex(0);
    setPaymentsItemsInvislable();
    setPaymentTrackShowMode(0);
    mpDbManager->setCurrentYear(QDate::currentDate().year());
    mpShowingModel = mpDbManager->getAllPaymentsInfo();
    displayPaymentsModel(mpShowingModel);
}

void MainWindow::showPaymentsHistory(){
    isItHistory = true;
    ui->showPaymentsDetailsBtn->setEnabled(false);
    ui->monthToPayCb->setCurrentIndex(0);
    setPaymentsItemsInvislable();
    setPaymentTrackShowMode(0);
    mpShowingModel = mpDbManager->getPaymentsHistory(getYear());
    displayPaymentsModel(mpShowingModel);
}

void MainWindow::displayAlertsOnGui(QSqlQueryModel *mMonthAlertModel){
    ui->paymentsTV->setModel(mMonthAlertModel);
    ui->paymentsTV->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->paymentsTV->verticalHeader()->hide();
    ui->paymentsTV->setAlternatingRowColors(true);
    ui->paymentsTV->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->paymentsTV->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->paymentsTV->hideColumn(15);
    hideColumns();
    ui->paymentsTV->show();
}

void MainWindow::displayAllmembersAllYear(QSqlQueryModel *monthAlertModel){
    ui->paymentsTV->setModel(monthAlertModel);
    ui->paymentsTV->showColumn(15);
    ui->paymentsTV->resizeColumnsToContents();
    ui->paymentsTV->verticalHeader()->hide();
    ui->paymentsTV->setAlternatingRowColors(true);
    ui->paymentsTV->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->paymentsTV->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->paymentsTV->show();
}

void MainWindow::displayPaymentsModel(QSqlQueryModel *monthAlertModel){
    ui->paymentsTV->setModel(monthAlertModel);
    ui->paymentsTV->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->paymentsTV->verticalHeader()->hide();
    ui->paymentsTV->setAlternatingRowColors(true);
    ui->paymentsTV->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->paymentsTV->setSelectionMode(QAbstractItemView::SingleSelection);
    if(!isItHistory){
        hideColumns();
    }else {
        showColumns();
    }
    ui->paymentsTV->show();
}

void MainWindow::displaySlectedMonthDefaults(){

    setConfCorrPayInvisible();
    ui->defaultsPaymentBtn->setVisible(false);
    ui->showAllBtn->setVisible(true);
    mpMonthAlertModel = mpDbManager->showSlectedMonthDefaults(ui->monthToPayCb->currentIndex());
    ui->paymentsTV->setModel(mpMonthAlertModel);
    ui->paymentsTV->verticalHeader()->hide();
    ui->paymentsTV->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->paymentsTV->setAlternatingRowColors(true);
    ui->paymentsTV->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->paymentsTV->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->paymentsTV->show();
}

void MainWindow::displaySlectedMemberPayments(){
    setConfCorrPayInvisible();
    ui->defaultsPaymentBtn->setVisible(false);
    ui->showAllBtn->setVisible(true);

    mpMonthAlertModel = mpDbManager->showSlectedMonthDefaults(ui->monthToPayCb->currentIndex());

    ui->paymentsTV->setModel(mpMonthAlertModel);
    ui->paymentsTV->verticalHeader()->hide();
    ui->paymentsTV->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->paymentsTV->setAlternatingRowColors(true);
    ui->paymentsTV->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->paymentsTV->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->paymentsTV->show();
}

void MainWindow::displayAllInPaymentTab(){
    setConfCorrPayInvisible();
    ui->showAllBtn->setVisible(false);
    ui->defaultsPaymentBtn->setVisible(true);
    ui->showPaymentsDetailsBtn->setEnabled(false);
    mpPaymentsToConfirmModel = mpDbManager->getAllPayInfoForSelectedMonth(ui->monthToPayCb->currentIndex());
    ui->paymentsTV->setModel(mpPaymentsToConfirmModel);
    ui->paymentsTV->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->paymentsTV->setAlternatingRowColors(true);
    ui->paymentsTV->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->paymentsTV->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->paymentsTV->show();
}

void MainWindow::displayTableOnGui(QSqlQueryModel *model){
    ui->basicInfoTV->setModel(model);
    ui->basicInfoTV->hideColumn(13);
    ui->basicInfoTV->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->basicInfoTV->verticalHeader()->hide();
    ui->basicInfoTV->setAlternatingRowColors(true);
    ui->basicInfoTV->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->basicInfoTV->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->basicInfoTV->show();
}

void MainWindow::Exit()
{
    close();
    qApp->quit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setShowActOrInact(bool value)
{
    showActOrInact = value;
}

bool MainWindow::getShowActOrInact() const
{
    return showActOrInact;
}

void MainWindow::printBasicInfo(){
    printThis(ui->basicInfoTV);
}

void MainWindow::printPaymentInfo(){
    printThis(ui->paymentsTV);
}

void MainWindow::printThis(QTableView *tableToPrint){
    mpPrintingService->printThis(tableToPrint);
}


void MainWindow::sendEmail(){

}


void MainWindow::setSearchItemsInvisible(){
    ui->findBtn_2->setVisible(true);
    ui->searchBy->setVisible(false);
    ui->searchInput->setVisible(false);
    ui->selectMethodSB->setVisible(false);
    ui->findBtn->setVisible(false);
    ui->findBtn->setEnabled(false);

    ui->searchInput->clear();
    ui->selectMethodSB->setCurrentIndex(0);
}

void MainWindow::setSearchItemsVisible(){
    ui->findBtn_2->setVisible(false);
    ui->searchBy->setVisible(true);
    ui->searchInput->setVisible(true);
    ui->findBtn->setVisible(true);
    ui->findBtn->setEnabled(false);
}

void MainWindow::enableSearchFields(){

    if(ui->searchBy->currentIndex()== 3){
        ui->selectMethodSB->setVisible(true);
        ui->searchInput->setVisible(false);
    } else {
        ui->selectMethodSB->setVisible(false);
        ui->searchInput->setVisible(true);
    }
}

void MainWindow::enableGoSearchBtn(){
    ui->findBtn->setEnabled(true);
}

void MainWindow::disabledAddMemberFields(){

    ui->refNo->setEnabled(false);
    ui->name->setEnabled(false);
    ui->mother->setEnabled(false);
    ui->motherInLaw->setEnabled(false);
    ui->spouse->setEnabled(false);
    ui->childern->setEnabled(false);
    ui->tel->setEnabled(false);
    ui->email->setEnabled(false);
    ui->postCode->setEnabled(false);
    ui->address->setEnabled(false);
    ui->memebershipDate->setEnabled(false);
    ui->paymentMethod->setEnabled(false);
    ui->payment->setEnabled(false);
    ui->saveBtn->setEnabled(false);
}

void MainWindow::enabledAddMemberFields(){
    ui->refNo->setEnabled(true);
    ui->name->setEnabled(true);
    ui->mother->setEnabled(true);
    ui->motherInLaw->setEnabled(true);
    ui->spouse->setEnabled(true);
    ui->childern->setEnabled(true);
    ui->tel->setEnabled(true);
    ui->email->setEnabled(true);
    ui->postCode->setEnabled(true);
    ui->address->setEnabled(true);
    ui->memebershipDate->setEnabled(true);
    ui->paymentMethod->setEnabled(true);
    ui->payment->setEnabled(true);
    ui->saveBtn->setEnabled(true);
}

void MainWindow::clearAddMemberUiFields(){
    ui->refNo->clear();
    ui->name->clear();
    ui->mother->clear();
    ui->motherInLaw->clear();
    ui->spouse->clear();
    ui->childern->clear();
    ui->tel->clear();
    ui->email->clear();
    ui->postCode->clear();
    ui->address->clear();
    ui->memebershipDate->setDate(QDate::currentDate());
    ui->paymentMethod->setCurrentIndex(0);
    ui->payment->setValue(5);
}

void MainWindow::EnableBtns(){
    ui->showPaymentsDetailsBtn->setEnabled(true);
    setDisplayBtnsVisible();
    displayAllInPaymentTab();
    ui->showAllBtn->setVisible(false);
}

void MainWindow::EnableEditRemoveBtn(){
    ui->editBtn->setEnabled(true);
    ui->removeBtn->setEnabled(true);

    int row_index= ui->basicInfoTV->currentIndex().row();
    QModelIndex index = mpShowingModel->index(row_index, 0);
    QString ref_no = index.data().toString();
    index.data().clear();

    bool status = mpDbManager->isMemStatusAct(ref_no);
    qDebug() << status << ref_no;

    if(status == true){
        ui->activateMemBtn->setEnabled(false);
        ui->deactivateMemBtn->setEnabled(true);
    } else {
        ui->activateMemBtn->setEnabled(true);
        ui->deactivateMemBtn->setEnabled(true);
    }
}

void MainWindow::deactivateMembership(){
    int row_index= ui->basicInfoTV->currentIndex().row();
    QModelIndex index = mpShowingModel->index(row_index, 0);
    QString ref_no = index.data().toString();

    reply = QMessageBox::question(this, "تجميد","هل انت متأكد؟  ", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if(!mpDbManager->deactivateMembership(ref_no)){
            ui->appStatus->setText("Error: membership deactivation ...");
        } else{
            ui->appStatus->setText("   تم التجميد بنجاح ");
            showActiveMembers();
            showCurrentPayments();
        }
    }
}

void MainWindow::activateMembership(){
    int row_index= ui->basicInfoTV->currentIndex().row();
    QModelIndex index = mpShowingModel->index(row_index, 0);
    QString ref_no = index.data().toString();

    reply = QMessageBox::question(this, "تنشيط","هل انت متأكد؟  ", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if(!mpDbManager->activateMembership(ref_no)){
            ui->appStatus->setText("Error: membership activation ...");
        } else{
            ui->appStatus->setText("   تم التنشيط بنجاح ");
            showInactiveMembers();
            showCurrentPayments();
        }
    }
}

void MainWindow::DisnableEditRemoveBtn(){
    ui->editBtn->setEnabled(false);
    ui->removeBtn->setEnabled(false);
}


void MainWindow::setCurrentMonth(){
    currentMonth = QDate::currentDate().month();
}

int MainWindow::getCurrentMonth(){
    return currentMonth;
}

void MainWindow::setUserAction(int action){
    userAction = action;
}

int MainWindow::getUserAction(){
    return userAction;
}

void MainWindow::setAlertChoice(int choice){
    alertChoice = choice;
}

int MainWindow::getAlertChoice(){
    return alertChoice;
}

void MainWindow::setPaymentTrackShowMode(int mode){
    pyamentTrackShowMode = mode;
}

int MainWindow::getPaymentTrackShowMode(){
    return pyamentTrackShowMode;
}

void MainWindow::setDefaultMonthCount(int monthCount){
    defaulMonthCount = monthCount;
}

int MainWindow::getDefaultMonthCount(){
    return defaulMonthCount;
}

void MainWindow::setYear(){
    selectedYear = ui->selectYearSb->text();
}

QString MainWindow::getYear(){
    return selectedYear;
}


void MainWindow::checkYear(){
    setYear();
    if(ui->selectYearSb->text() == QString::number(QDate::currentDate().year())){
        enableFeatures();
        showCurrentPayments();
    } else {
        disableFeatures();
        showPaymentsHistory();
    }
}

void MainWindow::enableFeatures(){
    if(!ui->monthToPayCb->isVisible()){
        ui->showPaymentsDetailsBtn->setVisible(true);
        ui->monthToPayCb->setVisible(true);
        setDisplayBtnsVisible();
        setConfCorrPayVisible();
        ui->label_2->setVisible(true);
        ui->label_4->setVisible(true);
        ui->groupBox_15->setVisible(true);
        ui->oneMonthBtn->setVisible(true);
        ui->twoMonthsBtn->setVisible(true);
        ui->threeMonthsBtn->setVisible(true);
        ui->higherThanThreeBtn->setVisible(true);
    }
}

void MainWindow::disableFeatures(){
    ui->showPaymentsDetailsBtn->setVisible(false);
    ui->monthToPayCb->setVisible(false);
    ui->label_2->setVisible(false);
    ui->label_4->setVisible(false);
    ui->groupBox_15->setVisible(false);
    ui->oneMonthBtn->setVisible(false);
    ui->twoMonthsBtn->setVisible(false);
    ui->threeMonthsBtn->setVisible(false);
    ui->higherThanThreeBtn->setVisible(false);
    setPaymentsItemsInvislable();
    setConfCorrPayVisible();

}

void MainWindow::setPaymentsItemsInvislable(){
    setDisplayBtnsInvisible();
    setConfCorrPayInvisible();
}

void MainWindow::setConfCorrPayVisible(){
    ui->correctPaymentBtn->setVisible(true);
    ui->confirmPaymentBtn->setVisible(true);
}

void MainWindow::setConfCorrPayInvisible(){
    ui->correctPaymentBtn->setVisible(false);
    ui->confirmPaymentBtn->setVisible(false);
}

void MainWindow::setDisplayBtnsVisible(){
    ui->defaultsPaymentBtn->setVisible(true);
    ui->showAllBtn->setVisible(true);
}

void MainWindow::setDisplayBtnsInvisible(){
    ui->defaultsPaymentBtn->setVisible(false);
    ui->showAllBtn->setVisible(false);
}

void MainWindow::hideColumns(){
    switch(getCurrentMonth()){
    case(1):
        ui->paymentsTV->hideColumn(4);
        ui->paymentsTV->hideColumn(5);
        ui->paymentsTV->hideColumn(6);
        ui->paymentsTV->hideColumn(7);
        ui->paymentsTV->hideColumn(8);
        ui->paymentsTV->hideColumn(9);
        ui->paymentsTV->hideColumn(10);
        ui->paymentsTV->hideColumn(11);
        ui->paymentsTV->hideColumn(12);
        ui->paymentsTV->hideColumn(13);
        ui->paymentsTV->hideColumn(14);
        break;
    case(2):
        ui->paymentsTV->hideColumn(5);
        ui->paymentsTV->hideColumn(6);
        ui->paymentsTV->hideColumn(7);
        ui->paymentsTV->hideColumn(8);
        ui->paymentsTV->hideColumn(9);
        ui->paymentsTV->hideColumn(10);
        ui->paymentsTV->hideColumn(11);
        ui->paymentsTV->hideColumn(12);
        ui->paymentsTV->hideColumn(13);
        ui->paymentsTV->hideColumn(14);
        break;
    case(3):
        ui->paymentsTV->hideColumn(6);
        ui->paymentsTV->hideColumn(7);
        ui->paymentsTV->hideColumn(8);
        ui->paymentsTV->hideColumn(9);
        ui->paymentsTV->hideColumn(10);
        ui->paymentsTV->hideColumn(11);
        ui->paymentsTV->hideColumn(12);
        ui->paymentsTV->hideColumn(13);
        ui->paymentsTV->hideColumn(14);
        break;
    case(4):
        ui->paymentsTV->hideColumn(7);
        ui->paymentsTV->hideColumn(8);
        ui->paymentsTV->hideColumn(9);
        ui->paymentsTV->hideColumn(10);
        ui->paymentsTV->hideColumn(11);
        ui->paymentsTV->hideColumn(12);
        ui->paymentsTV->hideColumn(13);
        ui->paymentsTV->hideColumn(14);
        break;
    case(5):
        ui->paymentsTV->hideColumn(8);
        ui->paymentsTV->hideColumn(9);
        ui->paymentsTV->hideColumn(10);
        ui->paymentsTV->hideColumn(11);
        ui->paymentsTV->hideColumn(12);
        ui->paymentsTV->hideColumn(13);
        ui->paymentsTV->hideColumn(14);
        break;
    case(6):
        ui->paymentsTV->hideColumn(9);
        ui->paymentsTV->hideColumn(10);
        ui->paymentsTV->hideColumn(11);
        ui->paymentsTV->hideColumn(12);
        ui->paymentsTV->hideColumn(13);
        ui->paymentsTV->hideColumn(14);
        break;
    case(7):
        ui->paymentsTV->hideColumn(10);
        ui->paymentsTV->hideColumn(11);
        ui->paymentsTV->hideColumn(12);
        ui->paymentsTV->hideColumn(13);
        ui->paymentsTV->hideColumn(14);
        break;
    case(8):
        ui->paymentsTV->hideColumn(11);
        ui->paymentsTV->hideColumn(12);
        ui->paymentsTV->hideColumn(13);
        ui->paymentsTV->hideColumn(14);
        break;
    case(9):
        ui->paymentsTV->hideColumn(12);
        ui->paymentsTV->hideColumn(13);
        ui->paymentsTV->hideColumn(14);
        break;
    case(10):
        ui->paymentsTV->hideColumn(13);
        ui->paymentsTV->hideColumn(14);
        break;
    case(11):
        ui->paymentsTV->hideColumn(14);
        break;
    }

}

void MainWindow::showColumns(){
    ui->paymentsTV->setColumnHidden(4, false);
    ui->paymentsTV->setColumnHidden(5, false);
    ui->paymentsTV->setColumnHidden(6, false);
    ui->paymentsTV->setColumnHidden(7, false);
    ui->paymentsTV->setColumnHidden(8, false);
    ui->paymentsTV->setColumnHidden(9, false);
    ui->paymentsTV->setColumnHidden(10, false);
    ui->paymentsTV->setColumnHidden(11, false);
    ui->paymentsTV->setColumnHidden(12, false);
    ui->paymentsTV->setColumnHidden(13, false);
    ui->paymentsTV->setColumnHidden(14, false);
}




