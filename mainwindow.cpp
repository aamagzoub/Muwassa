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

#define MYNULL NULL

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("موآساة");

    mpDbManager = new DatabaseManager;
    mpEmailingService = new EmailingService;
    mpAddMembersDialog = new AddMembersDialog;

    ui->appStatus->setText(mpDbManager->db_invocation());
    ui->appStatus->setText(mpDbManager->db_table_creation());

    ui->oneMonthBtn->setFixedSize(30,32);
    ui->twoMonthsBtn->setFixedSize(30,32);
    ui->threeMonthsBtn->setFixedSize(30,32);
    ui->higherThanThreeBtn->setFixedSize(30,32);

    ui->label_3->setStyleSheet("QLabel { background-color : black; color : white}");
    ui->label_6->setStyleSheet("QLabel { background-color : black; color : white}");
    ui->label_9->setStyleSheet("QLabel { background-color : black; color : white}");
    ui->label_13->setStyleSheet("QLabel { background-color : black; color : white}");

    ui->appStatus->setStyleSheet("QLabel { background-color :white ; color : #c0392b; border-radius: 10px}");
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
    connect(ui->sendEmailBtn,SIGNAL(clicked()),this,SLOT(sendEmail()));
    connect(ui->sendGenralEmailBtn,SIGNAL(clicked()),this,SLOT(sendEmail()));
    connect(ui->addMemberBtn,SIGNAL(clicked()),this,SLOT(startAddition()));
    connect(ui->searchInput,SIGNAL(textEdited(QString)),this,SLOT(enableGoSearchBtn()));
    connect(ui->selectMethodSB,SIGNAL(currentIndexChanged(QString)),this,SLOT(enableGoSearchBtn()));
    connect(ui->selectYearSb,SIGNAL(valueChanged(QString)),this,SLOT(checkYear()));
    connect(ui->showAllMemberInfoBtn,SIGNAL(clicked()),this,SLOT(showActiveMembers()));
    connect(ui->inactiveMemBtn,SIGNAL(clicked()),this,SLOT(showInactiveMembers()));
    connect(ui->deactivateMemBtn,SIGNAL(clicked()),this,SLOT(deactivateMembership()));
    connect(ui->activateMemBtn,SIGNAL(clicked()),this,SLOT(activateMembership()));


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
        } else{
            ui->appStatus->setText("   تم المسح بنجاح ");
            showActiveMembers();
            showCurrentPayments();
        }
    }
    DisnableEditRemoveBtn();
}


void MainWindow::startEditing(){
    enabledAddMemberFields();
    EditMemberBasicInfo();
}

void MainWindow::startViewing(){
    disabledAddMemberFields();
    EditMemberBasicInfo();
}

void MainWindow::startAddition(){
    enabledAddMemberFields();
    openAddMemberForm();
}


void MainWindow::EditMemberBasicInfo(){

    clearAddMemberFields();

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
    int payment;
    QSqlQueryModel *modelToShow;

    int row_index= ui->basicInfoTV->currentIndex().row();
    QModelIndex ref_no_value = mpShowingModel->index(row_index, 0);
    QString ref_no_string = ref_no_value.data().toString();

    mpReturnedRecord = mpDbManager->getRecordForEditOrFind(ref_no_string, getUserAction(),MYNULL);

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

    mpAddMembersDialog->ui->refNo->setText(refNo);
    mpAddMembersDialog->ui->name->setText(name);
    mpAddMembersDialog->ui->mother->setText(mother);
    mpAddMembersDialog->ui->tel->setText(tel);
    mpAddMembersDialog->ui->email->setText(email);
    mpAddMembersDialog->ui->postCode->setText(postcode);
    mpAddMembersDialog->ui->address->setText(address);
    mpAddMembersDialog->ui->spouse->setText(spouse);
    mpAddMembersDialog->ui->motherInLaw->setText(mother_in_law);
    mpAddMembersDialog->ui->childern->setText(childern);
    mpAddMembersDialog->ui->memebershipDate->setDate(membership_Date);
    mpAddMembersDialog->ui->paymentMethod->setCurrentText(payment_method);
    mpAddMembersDialog->ui->payment->setValue(payment);

    mpAddMembersDialog->readSettings();
    mpAddMembersDialog->exec();

    showActiveMembers();

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
    QString ref_no_string;
    setUserAction(1);
    ui->editBtn->setEnabled(false);
    ui->removeBtn->setEnabled(false);
    ui->activateMemBtn->setEnabled(false);
    ui->deactivateMemBtn->setEnabled(false);
    ui->showAllMemberInfoBtn->setVisible(false);
    ui->inactiveMemBtn->setVisible(true);
    mpShowingModel = mpDbManager->getRecordForEditOrFind(ref_no_string, getUserAction(),MYNULL);
    displayTableOnGui(mpShowingModel);
}

void MainWindow::showInactiveMembers(){
    ui->editBtn->setEnabled(false);
    ui->removeBtn->setEnabled(false);
    ui->activateMemBtn->setEnabled(false);
    ui->deactivateMemBtn->setEnabled(false);
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

void MainWindow::openAddMemberForm(){

    clearAddMemberFields();
    mpAddMembersDialog->ui->statusLbl->clear();

    mpAddMembersDialog->readSettings();
    mpAddMembersDialog->exec();

    if (!mpAddMembersDialog->cancelValue == 1){
        showActiveMembers();
    }
    DisnableEditRemoveBtn();
    showCurrentPayments();
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
    mpAddMembersDialog->ui->refNo->setEnabled(false);
    mpAddMembersDialog->ui->name->setEnabled(false);
    mpAddMembersDialog->ui->mother->setEnabled(false);
    mpAddMembersDialog->ui->motherInLaw->setEnabled(false);
    mpAddMembersDialog->ui->spouse->setEnabled(false);
    mpAddMembersDialog->ui->childern->setEnabled(false);
    mpAddMembersDialog->ui->tel->setEnabled(false);
    mpAddMembersDialog->ui->email->setEnabled(false);
    mpAddMembersDialog->ui->postCode->setEnabled(false);
    mpAddMembersDialog->ui->address->setEnabled(false);
    mpAddMembersDialog->ui->memebershipDate->setEnabled(false);
    mpAddMembersDialog->ui->paymentMethod->setEnabled(false);
    mpAddMembersDialog->ui->payment->setEnabled(false);
    mpAddMembersDialog->ui->saveBtn->setEnabled(false);
}

void MainWindow::enabledAddMemberFields(){
    mpAddMembersDialog->ui->refNo->setEnabled(true);
    mpAddMembersDialog->ui->name->setEnabled(true);
    mpAddMembersDialog->ui->mother->setEnabled(true);
    mpAddMembersDialog->ui->motherInLaw->setEnabled(true);
    mpAddMembersDialog->ui->spouse->setEnabled(true);
    mpAddMembersDialog->ui->childern->setEnabled(true);
    mpAddMembersDialog->ui->tel->setEnabled(true);
    mpAddMembersDialog->ui->email->setEnabled(true);
    mpAddMembersDialog->ui->postCode->setEnabled(true);
    mpAddMembersDialog->ui->address->setEnabled(true);
    mpAddMembersDialog->ui->memebershipDate->setEnabled(true);
    mpAddMembersDialog->ui->paymentMethod->setEnabled(true);
    mpAddMembersDialog->ui->payment->setEnabled(true);
    mpAddMembersDialog->ui->saveBtn->setEnabled(true);
}

void MainWindow::clearAddMemberFields(){
    mpAddMembersDialog->ui->refNo->clear();
    mpAddMembersDialog->ui->name->clear();
    mpAddMembersDialog->ui->mother->clear();
    mpAddMembersDialog->ui->motherInLaw->clear();
    mpAddMembersDialog->ui->spouse->clear();
    mpAddMembersDialog->ui->childern->clear();
    mpAddMembersDialog->ui->tel->clear();
    mpAddMembersDialog->ui->email->clear();
    mpAddMembersDialog->ui->postCode->clear();
    mpAddMembersDialog->ui->address->clear();
    mpAddMembersDialog->ui->memebershipDate->setDate(QDate::currentDate());
    mpAddMembersDialog->ui->paymentMethod->setCurrentIndex(0);
    mpAddMembersDialog->ui->payment->setValue(5);
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

    if(isMemStatusAct()){
        ui->activateMemBtn->setEnabled(false);
        ui->deactivateMemBtn->setEnabled(true);
    } else {
        ui->activateMemBtn->setEnabled(true);
        ui->deactivateMemBtn->setEnabled(false);
    }
}

bool MainWindow::isMemStatusAct(){
    int row_index= ui->basicInfoTV->currentIndex().row();
    QModelIndex index = mpShowingModel->index(row_index, 0);
    QString ref_no = index.data().toString();
    index.data().clear();
    qDebug() << ref_no;
    bool status = mpDbManager->isMemStatusAct(ref_no);
    qDebug() << status;
    return mpDbManager->isMemStatusAct(ref_no);
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
        ui->sendEmailBtn->setVisible(true);

        ui->oneMonthBtn->setVisible(true);
        ui->twoMonthsBtn->setVisible(true);
        ui->threeMonthsBtn->setVisible(true);
        ui->higherThanThreeBtn->setVisible(true);
    }
}

void MainWindow::disableFeatures(){
    ui->sendEmailBtn->setVisible(false);
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




