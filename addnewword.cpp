#include "addnewword.h"
#include "ui_addnewword.h"
#include <QStandardPaths>
#include <iostream>
#include <QVBoxLayout>
#include <QFile>

AddNewWord::AddNewWord(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddNewWord)
    , dataTransfer(DataTransfer::getInstance())
{
    ui->setupUi(this);

    ui->cancelButton->setStyleSheet("background-color: #FF0000;");
    ui->addWordButton->setStyleSheet("background-color: #00BA0C;");

    ui->listName->setText(dataTransfer.currentListName);
    ui->languageOneName->setText(dataTransfer.currentListLanguageOneName);
    ui->languageTwoName->setText(dataTransfer.currentListLanguageTwoName);

    goBack=false;
}


void AddNewWord::closeEvent(QCloseEvent *event)
{
    if(goBack==true)
    {
        emit finished(0);
    }
    else
    {
        QApplication::quit();
    }
}


void AddNewWord::on_cancelButton_clicked()
{
    goBack=true;
    this->close();
}


void AddNewWord::on_addWordButton_clicked()
{
    languageOneWord=ui->languageOneEditText->toPlainText();
    languageTwoWord=ui->languageTwoEditText->toPlainText();
    comment=ui->commentEditText->toPlainText();

    if(checkIfCorrect()==true) addWord();
    else displayInfo();
}


bool AddNewWord::checkIfCorrect() const
{
    if((languageOneWord=="")||(languageTwoWord=="")) return false;
    if(languageOneWord.contains(';')||languageTwoWord.contains(';')||comment.contains(";")) return false;
    return true;
}


void AddNewWord::addWord()
{
    color="beige";
    if(ui->redButton->isChecked()) color="red";
    else if(ui->greenButton->isChecked()) color="green";
    else if(ui->yellowButton->isChecked()) color="yellow";
    else if(ui->orangeButton->isChecked()) color="orange";
    else if(ui->blueButton->isChecked()) color="blue";
    else if(ui->whiteButton->isChecked()) color="white";
    else if(ui->purpleButton->isChecked()) color="purple";
    else if(ui->pinkButton->isChecked()) color="pink";
    else if(ui->grayButton->isChecked()) color="gray";

    QString newLine=languageOneWord+";"+languageTwoWord+";"+comment+";"+color;

    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/data/";
    QString directory=dataPath+dataTransfer.currentListName;

    QFile file(directory);
    if(file.open(QIODevice::Append|QIODevice::Text))
    {
        QTextStream out(&file);
        out<<newLine<<"\n";
    }
    file.close();

    dataTransfer.newWordAdded=true;
    goBack=true;
    this->close();
}


void AddNewWord::displayInfo()
{
    QDialog dialog(this);

    dialog.setWindowTitle("Wrong names");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *label = new QLabel("Word fields cannot be empty. None of field can contain ; sign.");
    layout->addWidget(label);

    QPushButton *okButton = new QPushButton("Ok");
    layout->addWidget(okButton);
    okButton->setStyleSheet("background-color: #C0C0C0;");
    connect(okButton, &QPushButton::clicked,this, [&dialog](){dialog.close();} );

    dialog.exec();
}


AddNewWord::~AddNewWord()
{
    delete ui;
}

