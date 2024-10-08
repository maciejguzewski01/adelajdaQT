#include "listcontent.h"
#include "ui_listcontent.h"
#include <QGraphicsScene>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QStandardPaths>
#include <QFile>

ListContent::ListContent(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ListContent)
    , dataTransfer(DataTransfer::getInstance())
{
    ui->setupUi(this);
    ui->addNewWordButton->setStyleSheet("background-color: #00BA0C;");
    ui->goBackButton->setStyleSheet("background-color: #C0C0C0;");

    ui->listName->setText(dataTransfer.currentListName);
    ui->languageOneName->setText(dataTransfer.currentListLanguageOneName);
    ui->languageTwoName->setText(dataTransfer.currentListLanguageTwoName);

    setImage();
    goBack=false;

    getDataFromFile();
    setNamesOnWidgetList();

}


void ListContent::setImage()
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    QPixmap pixmap(":/new/prefix1/app_icon.jpeg");
    pixmap=pixmap.scaled(249,249,Qt::KeepAspectRatio);
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
    scene->addItem(item);
    scene->setSceneRect(pixmap.rect());
    ui->image->setScene(scene);
}


void ListContent::getDataFromFile()
{
    fileContent.clear();
    languageOneWords.clear();
    languageTwoWords.clear();
    comments.clear();
    colors.clear();

    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/data/";
    QString directory=dataPath+dataTransfer.currentListName;

    QFile file(directory);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        while(!file.atEnd()) fileContent.push_back(file.readLine());
    }
    file.close();

    for(int i=1;i<fileContent.size();++i)
    {
        if(fileContent.at(i)!="")
        {
            QStringList splitBuffer=fileContent.at(i).split(";");
            languageOneWords.push_back(splitBuffer[0]);
            languageTwoWords.push_back(splitBuffer[1]);
            comments.push_back(splitBuffer[2]);
            colors.push_back(splitBuffer[3]);
        }
    }
}


void ListContent::setNamesOnWidgetList()
{
    ui->listWidget->clear();

    for(int i=0;i<languageOneWords.size();++i)
    {
        ui->listWidget->addItem(languageOneWords.at(i)+" - "+languageTwoWords.at(i));
        ui->listWidget->item(i)->setTextAlignment(Qt::AlignCenter);
    }
    ui->listWidget->setStyleSheet("QListWidget::item { border-bottom: 1px solid gray; padding: 5px; }");
}


void ListContent::closeEvent(QCloseEvent *event)
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


void ListContent::on_goBackButton_clicked()
{
    goBack=true;
    this->close();
}


void ListContent::on_addNewWordButton_clicked()
{
    AddNewWord* w;
    w= new AddNewWord(this);
    w->setStyleSheet("background-color: #FFFF86;");
    w->setWindowTitle("Add new word");
    w->setFixedSize(800,600);

    connect(w, &AddNewWord::finished, this, &ListContent::addingWordFinished);
    this->hide();
    w->show();
}


void ListContent::addingWordFinished()
{
    if(dataTransfer.newWordAdded==true)
    {
        getDataFromFile();
        setNamesOnWidgetList();

        dataTransfer.newWordAdded=false;
    }
    this->show();
}


void ListContent::on_listWidget_itemClicked(QListWidgetItem *item)
{
    int number= ui->listWidget->row(item);

    dataTransfer.currentItemNumber=number;

    dataTransfer.currentWordLanguageOne=languageOneWords.at(number);
    dataTransfer.currentWordLanguageTwo=languageTwoWords.at(number);
    dataTransfer.currentComment=comments.at(number);
    dataTransfer.currentColor=colors.at(number);


    SingleWord* w;
    w= new SingleWord(this);
    w->setWindowTitle(item->text());
    w->setFixedSize(800,600);

    connect(w, &SingleWord::finished, this, &ListContent::singleWordFinished);
    this->hide();
    w->show();
}


void ListContent::singleWordFinished()
{
    if(dataTransfer.wordEdited==true)
    {
        getDataFromFile();
        setNamesOnWidgetList();
        dataTransfer.wordEdited=false;
    }
    if(dataTransfer.wordDeleted==true)
    {
        getDataFromFile();
        setNamesOnWidgetList();
        dataTransfer.wordDeleted=false;
    }
    this->show();
}


ListContent::~ListContent()
{
    delete ui;
}
