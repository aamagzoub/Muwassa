#include "addmembersdialog.h"
#include "ui_addmembersdialog.h"
#include <QMessageBox>

//static const char * ksOrganization{"DevSuda"};
//static const char * ksApp         {"Muwassa"};
//static const char * ksKey         {"geometery"};

AddMembersDialog::AddMembersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMembersDialog)
{
    ui->setupUi(this);
    setWindowTitle("البيانات الشخصية");

    mpDbManager = new DatabaseManager;
    mpDbManager->setCurrentYear(QDate::currentDate().year());

    ui->refNo->setFixedSize(94,22);
    ui->memebershipDate->setDate(QDate::currentDate());
    ui->memebershipDate->setMaximumDate(QDate::currentDate());
    ui->refNo->setValidator(new QIntValidator(1,200,this));
    connect(ui->saveBtn,SIGNAL(clicked()),this,SLOT(IsRefNoValid()));

    connect(ui->closeFormBtn,SIGNAL(clicked()),this,SLOT(Exit()));
}

AddMembersDialog::~AddMembersDialog()
{
    delete ui;
}

void AddMembersDialog::IsRefNoValid(){
    QString ref_no;
    if (!ui->refNo->text().isEmpty()){
        ref_no = ui->refNo->text();
        SaveMemberBasicInfo(ref_no);
    } else {
        ui->statusLbl->setText("   الرجاء ادخال الرقم المتسلسل ...");
    }
}

void AddMembersDialog::SaveMemberBasicInfo(QString ref_no){

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

            ui->statusLbl->setText("   تم الحفظ بنجاح ");

        }

    } else {

        reply = QMessageBox::question(this, "تحديث",
                                      "هل انت متأكد؟  ",
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
            }

        } else {

        }
    }
}


void AddMembersDialog::Exit()
{
    close();
}

void AddMembersDialog::closeEvent(QCloseEvent *event){
    QSettings settings("DevSuda", "Muwassa");
    settings.setValue("geometery", saveGeometry());
    QDialog::closeEvent(event);
}

void AddMembersDialog::readSettings(){
    QSettings settings("DevSuda", "Muwassa");
    restoreGeometry(settings.value("geometery").toByteArray());
}
