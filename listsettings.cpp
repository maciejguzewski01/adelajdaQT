#include "listsettings.h"
#include "ui_listsettings.h"
#include <QVector>
#include <QStandardPaths>
#include <filesystem>
#include <QVBoxLayout>
#include <QFile>
#include <QRegularExpression>
ListSettings::ListSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ListSettings)
    , dataTransfer(DataTransfer::getInstance())
{
    ui->setupUi(this);
    ui->goBackButton->setStyleSheet("background-color: #C0C0C0;");
    ui->editNamesButton->setStyleSheet("background-color: #00BA0C;");
    ui->deleteListButton->setStyleSheet("background-color: #FF0000;");

    ui->editTextListName->setPlainText(dataTransfer.currentListName);
    ui->editTextLanguageOneName->setPlainText(dataTransfer.currentListLanguageOneName);
    ui->editTextLanguageTwoName->setPlainText(dataTransfer.currentListLanguageTwoName);
    goBackToOptions=false;
}


void ListSettings::closeEvent(QCloseEvent *event)
{
    if(goBackToOptions==true)
    {
        emit finished(0);
    }
    else
    {
        QApplication::quit();
    }
}


void ListSettings::on_goBackButton_clicked()
{
    goBackToOptions=true;
    this->close();
}


void ListSettings::on_editNamesButton_clicked()
{
    listName=ui->editTextListName->toPlainText();
    languageOneName=ui->editTextLanguageOneName->toPlainText();
    languageTwoName=ui->editTextLanguageTwoName->toPlainText();

    if(checkIfNamesCorrect()==true) editList();
    else displayInfo();
}


bool ListSettings::checkIfNamesCorrect() const
{
    if((listName=="")||(languageOneName=="")||(languageTwoName=="")) return false;
    if((checkIfNameCorrect(listName)==false)||(checkIfNameCorrect(languageOneName)==false)||(checkIfNameCorrect(languageTwoName)==false)) return false;
    if(checkIfNameUnique()==false) return false;

    return true;
}


bool ListSettings::checkIfNameCorrect(const QString string) const
{
    QRegularExpression regex("^[\\p{L}\\p{N}_ ]+$");
    return regex.match(string).hasMatch();
}


bool ListSettings::checkIfNameUnique() const
{
    if(dataTransfer.currentListName==listName) return true;

    std::vector<QString> buffer;
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/data";

    for(const auto & i: std::filesystem::directory_iterator(dataPath.toStdString()))
    {

        QString fileBuffer=QString::fromStdString(i.path().filename().string());
        buffer.push_back(fileBuffer);
    }

    if(std::find(buffer.begin(),buffer.end(),listName)==buffer.end()) return true;
    return false;
}


void ListSettings::displayInfo()
{
    QDialog dialog(this);

    dialog.setWindowTitle("Wrong names");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *label = new QLabel("All names can consist only of alphanumeric signs, spaces and underscores. They cannot be empty. Name of list must be unique.");
    layout->addWidget(label);

    QPushButton *okButton = new QPushButton("Ok");
    layout->addWidget(okButton);
    okButton->setStyleSheet("background-color: #C0C0C0;");
    connect(okButton, &QPushButton::clicked,&dialog, [&dialog](){dialog.close();} );

    dialog.exec();
}


void ListSettings::editList()
{

    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/data/";
    QString directory=dataPath+dataTransfer.currentListName;

    QVector<QString> buffer;

    QFile file(directory);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        while(!file.atEnd()) buffer.push_back(file.readLine());
    }
    file.close();

    if(listName!=dataTransfer.currentListName)
    {
        file.remove();
        dataTransfer.listNameModified=true;
    }

    buffer[0]=languageOneName+";"+languageTwoName;

    QString newDirectory=dataPath+listName;
    QFile newFile(newDirectory);
    if(newFile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QTextStream out(&newFile);
        for(int i=0;i<buffer.size();++i)
        {
            out<<buffer[i];
        }
    }
    newFile.close();

    goBackToOptions=true;
    dataTransfer.currentListName=listName;
    dataTransfer.currentListLanguageOneName=languageOneName;
    dataTransfer.currentListLanguageTwoName=languageTwoName;
    this->close();
}


void ListSettings::on_deleteListButton_clicked()
{
    QDialog dialog(this);

    dialog.setWindowTitle("Delete list");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *label = new QLabel("This action cannont be undone. Are you sure you want to delete this list?");
    layout->addWidget(label);

    QPushButton *cancelButton = new QPushButton("Cancel");
    layout->addWidget(cancelButton);
    cancelButton->setStyleSheet("background-color: #00BA0C;");
    connect(cancelButton, &QPushButton::clicked,&dialog, [&dialog](){dialog.close();} );


    QPushButton *yesButton = new QPushButton("Yes");
    layout->addWidget(yesButton);
    yesButton->setStyleSheet("background-color: #FF0000;");
    connect(yesButton, &QPushButton::clicked,&dialog, [this, &dialog](){
        deleteList();
        dialog.close();
    } );

    dialog.exec();
}


void ListSettings::deleteList()
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/data/";
    std::string directory=dataPath.toStdString()+dataTransfer.currentListName.toStdString();
    const char* fileChar=directory.c_str();
    remove(fileChar);

    dataTransfer.listDeleted=true;
    goBackToOptions=true;
    this->close();
}


ListSettings::~ListSettings()
{
    delete ui;
}
