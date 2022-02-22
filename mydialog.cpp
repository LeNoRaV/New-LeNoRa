#include "mydialog.h"
#include <QLayout>

void MyDialogScore::getMessageBox(const QString textError,bool error){
    QMessageBox msgBox;
    if(error) msgBox.setIcon(QMessageBox::Critical);
    else msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(textError);
    msgBox.exec();
}

MyDialogScore::MyDialogScore(QSqlDatabase* db1, QString* result1,QWidget *parent) : QDialog(parent)
{
    result=result1;
    db=db1;
    setModal(true);
    setWindowTitle("Ввод баллов");
    setWindowIcon(QIcon(":/MyPictures/pictures/enter.png"));

    score=new QLineEdit(this);
    score->setPlaceholderText("Баллы");
    QPushButton* enter=new QPushButton("&Подтвердить",this);

    QHBoxLayout* layout1=new QHBoxLayout();
    layout1->addWidget(score);
    layout1->addWidget(enter);
    setLayout(layout1);

    connect(enter,SIGNAL(pressed()),this,SLOT(slotEnter()));
    show();
}

void MyDialogScore::slotEnter(){
//    QSqlQuery query(*db);
//    if(!q.exec("SELECT * FROM lnr_competition;")){
//            getMessageBox("Таблица не открывается",true);
//            return;
//    }

//        if(name->text()==nullptr || password->text()==nullptr || record_book->text()==nullptr) {
//            getMessageBox("Ошибка ввода данных: не все обязательные (*) поля заполнены",true);
//            return;
//        }
//        if(!query.exec("INSERT INTO lnr_participants VALUES ('"+record_book->text()+"','"+name->text()+"','"+weight->text()+"','"+height->text()+"','"+age->text()+"','"+talant->text()+"','"+password->text()+"');")) {
//            getMessageBox("Ошибка: эта участница уже зарегистрирована",true);
//            return;
//        }
//        getMessageBox("Регистрация прошла успешно.",false);
//    close();

//    QSqlQuery q(db);
//    if(!q.exec("SELECT * FROM lnr_competition;")){
//            getMessageBox("Таблица не открывается",true);
//            return;
//    }

//    q.seek(intDeleteOrAdd);
//    int a;                                 //!!!
//    QString name_id=q.value(1).toString();
//    if(!q.exec("UPDATE lnr_competition SET Баллы='"+QString::number(a)+"' WHERE lnr_competition.Номинация='"+name_id+"';")){
//        getMessageBox("Ошибка открытия таблицы соревнования", true);
//        return;
//    }
//    if(!q.exec("SELECT * FROM lnr_competition;")){
//        getMessageBox("Ошибка открытия таблиц", true);
//        return;
//    }
//    model->setQuery(q);
//    tableView->reset();
}

void MyDialogEnter::getMessageBox(const QString textError,bool error){
    QMessageBox msgBox;
    if(error) msgBox.setIcon(QMessageBox::Critical);
    else msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(textError);
    msgBox.exec();
}

MyDialogEnter::MyDialogEnter(QSqlDatabase* db1, QString* result1,QWidget *parent) : QDialog(parent)
{
    result=result1;
    db=db1;
    setModal(true);
    setWindowTitle("Вход");
    setWindowIcon(QIcon(":/MyPictures/pictures/enter.png"));
    login=new QLineEdit(this);
    login->setPlaceholderText("Логин");
    password=new QLineEdit(this);
    password->setPlaceholderText("Пароль");
    password->setEchoMode(QLineEdit::Password);

    QPushButton* enter=new QPushButton("&Войти",this);
    QMenuBar* menuBar=new QMenuBar(this);
    QMenu* menu=new QMenu("&Регистрация",this);
    QAction* client=new QAction(tr("&Мисс Бауман 2022"));
    client->setIcon(QIcon(":/MyPictures/pictures/client.png"));
    menu->addAction(client);
    menuBar->addMenu(menu);

    QLabel* label=new QLabel("Войти как",this);
    box=new QComboBox(this);
    box->addItems({"Участница","Жюри","Администратор"});


    QHBoxLayout* layout1=new QHBoxLayout();
    layout1->addWidget(label);
    layout1->addWidget(box);
    QVBoxLayout* layout2=new QVBoxLayout();
    layout2->addWidget(login);
    layout2->addWidget(password);
    QHBoxLayout* layout3=new QHBoxLayout();
    layout3->setMenuBar(menuBar);
    layout3->addLayout(layout2);
    layout3->addWidget(enter);
    QVBoxLayout* layoutResult=new QVBoxLayout();
    layoutResult->setMenuBar(menuBar);
    layoutResult->addLayout(layout1);
    layoutResult->addLayout(layout3);
    setLayout(layoutResult);

    connect(enter,SIGNAL(pressed()),this,SLOT(slotEnter()));
    connect(menuBar,SIGNAL(triggered(QAction*)),this,SLOT(slotTriggeredMenuBar(QAction*)));
    show();
}

void MyDialogEnter::slotEnter(){
    if(box->currentText()=="Администратор"){
        QFile file("C:/Users/lesko/Desktop/C++/DB3-jul/New-LeNoRa/files/admin.txt");
        if(!file.open(QIODevice::ReadOnly)){
            getMessageBox("Вы не можете войти как администратор с этого устройства",true);
            return;
        }
        QString str=file.readAll();
        file.close();
        qDebug()<<str;
        if(login->text()==str.section(",",0,0) || password->text()==str.section(",",2,2)){
            (*result)="админ";
            close();
        }
        else getMessageBox("Ошибка входа: проверьте правильность логина и пароля",true);
    }
    if(box->currentText()=="Жюри"){
        QFile file("C:/Users/lesko/Desktop/C++/DB3-jul/New-LeNoRa/files/jury.txt");
        if(!file.open(QIODevice::ReadOnly)){
            getMessageBox("Вы не можете войти как жюри с этого устройства",true);
            return;
        }
        QString str=file.readAll();
        file.close();
        qDebug()<<str;
        if(login->text()==str.section(",",0,0) || password->text()==str.section(",",2,2)){
            (*result)="жюри";
            close();
        }
        else getMessageBox("Ошибка входа: проверьте правильность логина и пароля",true);
    }
    if(box->currentText()=="Участница"){
        QSqlQuery query(*db);
        if(!query.exec("SELECT * FROM lnr_participants;")){
            getMessageBox("Таблица с участницами не открывается",true);
            return;
         }
        do{
            if(query.value(0).toString()==login->text() && query.value(6).toString()==password->text()){
                (*result)="участница,"+login->text();
                close();
                return;
            }
        }while(query.next());
        getMessageBox("Ошибка входа: проверьте правильность логина и пароля",true);
    }
}

void MyDialogEnter::slotTriggeredMenuBar(QAction* action){
    if(action->text()==tr("&Мисс Бауман 2022")){
        MyDialogRegistration* dialogRegistration=new MyDialogRegistration(db);
        dialogRegistration->exec();
    }
}

void MyDialogRegistration::getMessageBox(const QString textError,bool error){
    QMessageBox msgBox;
    if(error) msgBox.setIcon(QMessageBox::Critical);
    else msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(textError);
    msgBox.exec();
}

MyDialogRegistration::MyDialogRegistration(QSqlDatabase* db1, QWidget *parent) : QDialog(parent)
{
    db=db1;
    setModal(true);

    name=new QLineEdit(this);
    name->setPlaceholderText("ФИО*");
    record_book=new QLineEdit(this);
    QRegularExpression regExp("[0-9]{3}");
    record_book->setPlaceholderText("Номер зачётки*");
    password=new QLineEdit(this);
    password->setPlaceholderText("Пароль*");
    password->setEchoMode(QLineEdit::Password);

    QVBoxLayout* layoutLineEdit=new QVBoxLayout();
    layoutLineEdit->addWidget(name);
    layoutLineEdit->addWidget(record_book);

    QPushButton* enter=new QPushButton("Регистрация");

    weight=new QLineEdit(this);
    weight->setPlaceholderText("Вес");
    layoutLineEdit->addWidget(weight);
    height=new QLineEdit(this);
    height->setPlaceholderText("Рост");
    layoutLineEdit->addWidget(height);
    age=new QLineEdit(this);
    age->setPlaceholderText("Возраст");
    layoutLineEdit->addWidget(age);
    talant=new QLineEdit(this);
    talant->setPlaceholderText("Талант");
    layoutLineEdit->addWidget(talant);

    weight->setValidator(new QRegularExpressionValidator(regExp,this));
    height->setValidator(new QRegularExpressionValidator(regExp,this));
    age->setValidator(new QRegularExpressionValidator(regExp,this));

    layoutLineEdit->addWidget(password);
    QHBoxLayout* layoutResult=new QHBoxLayout();
    layoutResult->addLayout(layoutLineEdit);
    layoutResult->addWidget(enter);
    setLayout(layoutResult);

    connect(enter,SIGNAL(pressed()),this,SLOT(slotEnter()));
    show();
}

void MyDialogRegistration::slotEnter(){
    QSqlQuery query(*db);
    if(!query.exec("SELECT * FROM lnr_participants;")) {
        getMessageBox("Таблица с участницами не открывается",true);
        return;
     }

        if(name->text()==nullptr || password->text()==nullptr || record_book->text()==nullptr) {
            getMessageBox("Ошибка ввода данных: не все обязательные (*) поля заполнены",true);
            return;
        }
        if(!query.exec("INSERT INTO lnr_participants VALUES ('"+record_book->text()+"','"+name->text()+"','"+weight->text()+"','"+height->text()+"','"+age->text()+"','"+talant->text()+"','"+password->text()+"');")) {
            getMessageBox("Ошибка: эта участница уже зарегистрирована",true);
            return;
        }
        getMessageBox("Регистрация прошла успешно.",false);
    close();
}
