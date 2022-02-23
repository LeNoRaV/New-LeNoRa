#include "mainwindow.h"

void MainWindow::participantProfile(QString record_book){
    changeMenuBar({{"Номинации","посмотреть свои номинации"},{"Другое","профиль участницы","настройки профиля участницы","выйти"}});
    palette->setColor(QPalette::Window,Qt::darkGray);
    setPalette(*palette);
    setWindowTitle("Участница конкурса красоты");
    setProperty("record_book",record_book);
    QAction* act=new QAction("профиль участницы");
    emit menuBar->triggered(act);
}

void MainWindow::changeInfoparticipant(){
    if(person.name->text()==nullptr ||person.password->text()==nullptr){
        getMessageBox("Нельзя оставлять поля пустыми!",true);
        return;
    }
    QSqlQuery q(db);
    if(!q.exec("UPDATE lnr_participants SET Номер_зачётки='"+person.login->text()
               +"', ФИО='"+person.name->text()
               +"', Пароль='"+person.password->text()
               +"', Вес='"+person.weight->text()
               +"', Рост='"+person.height->text()
               +"', Возраст='"+person.age->text()
               +"', Талант='"+person.talant->text()
               +"' WHERE Номер_зачётки='"+property("record_book").toString()+"';")){
        getMessageBox("Эта участница уже зарегистрирована",true);
        return;
    }
    setProperty("record_book",person.login->text());
}

static int intDeleteOrAdd;

void MainWindow::slotChooseDiscipline(QPoint point){
    intDeleteOrAdd=tableView->indexAt(point).row();
    QMenu* menu=new QMenu(this);
    QAction* action1=new QAction(tr("Выбрать"),this);
    connect(action1,SIGNAL(triggered()),this,SLOT(slotChooseDiscipline()));
    menu->addAction(action1);
    menu->exec(QCursor::pos());
}

void MainWindow::slotChooseDiscipline(){
    QSqlQuery q(db);

    if(!q.exec("SELECT * FROM lnr_nominations")){
            getMessageBox("Таблица не открывается",true);
            return;
    }

    q.seek(intDeleteOrAdd);
    QString name0=q.value(0).toString();

    QPushButton* add=new QPushButton("&Выставить баллы",this);
    lineEdit=new QLineEdit(this);
    lineEdit->setPlaceholderText("Номер зачётки");
    lineEdit2=new QLineEdit(this);
    lineEdit2->setPlaceholderText("Кол-во баллов");
    QVBoxLayout* layout1=new QVBoxLayout();
    layout1->addWidget(lineEdit);
    layout1->addWidget(lineEdit2);
    layout1->addWidget(add);
    layout1->addStretch(1);
    QHBoxLayout* layout2=new QHBoxLayout();
    layout2->addWidget(tableView);
    layout2->addLayout(layout1);
    QWidget* widget=new QWidget(this);
    widget->setLayout(layout2);
    setCentralWidget(widget);

    connect(add,SIGNAL(pressed()),this,SLOT(slotAddScore()));

    if(!q.exec("SELECT lnr_nominations.Название_номинации, lnr_participants.Номер_зачётки, lnr_participants.ФИО, lnr_competition.Баллы FROM lnr_competition, lnr_nominations, lnr_participants WHERE lnr_nominations.Название_номинации = '"+ QString(name0) +"' AND lnr_nominations.\"ID\" = lnr_competition.Номинация AND lnr_participants.Номер_зачётки = lnr_competition.Участница ;")){
            getMessageBox("Таблица не открывается",true);
            return;
    }
    model->setQuery(q);
    tableView->reset();
}

void MainWindow::slotAddScore(){
    QSqlQuery q(db);
    if(!q.exec("SELECT * FROM lnr_nominations")){
            getMessageBox("Таблица не открывается",true);
            return;
    }

    q.seek(intDeleteOrAdd);
    int name0=q.value(1).toInt();
    getMessageBox(QString::number(name0),true);

    if(!q.exec("UPDATE lnr_competition SET Баллы = "+lineEdit2->text()+" WHERE Участница = '"+lineEdit->text()+"' AND Номинация ="+QString::number(name0)+";")){
        qDebug()<<q.lastError();
        getMessageBox("Не удаётся добавить",true);
    }

    model->setQuery(q);
    tableView->reset();
}
