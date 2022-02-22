#include "mainwindow.h"

void MainWindow::clientProfile(QString record_book){
    changeMenuBar({{"Номинации","посмотреть свои номинации"},{"Другое","профиль участницы","настройки профиля участницы","выйти"}});
    palette->setColor(QPalette::Window,Qt::lightGray);
    setPalette(*palette);
    setWindowTitle("Участница конкурса красоты");
    setProperty("record_book",record_book);
    QAction* act=new QAction("профиль участницы");
    emit menuBar->triggered(act);
}

void MainWindow::changeInfoClient(){
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
//void MainWindow::slotChangeLesson(QPoint point){
//    intDeleteOrAdd=tableView->indexAt(point).row();
//    QMenu* menu=new QMenu(this);
//    QAction* action1=new QAction(tr("Отменить"),this);
//    QAction* action2=new QAction(tr("Отметить как прошедшее"),this);
//    connect(action1,SIGNAL(triggered()),this,SLOT(slotChangeLesson()));
//    menu->addAction(action1);
//    connect(action2,SIGNAL(triggered()),this,SLOT(slotChangeLesson()));
//    menu->addAction(action2);
//    menu->exec(QCursor::pos());
//}

//void MainWindow::slotChangeLesson(){
//    QAction* action=qobject_cast<QAction*>(sender());
//    if(action==nullptr) return;
//    if(action->text()==tr("Отменить")){
//        QSqlQuery q(db);
//        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента='"+property("telephone").toString()+"' AND az_lessons.Было_занятие=false;")){
//            getMessageBox("Ошибка открытия таблиц", true);
//            return;
//        }
//        q.seek(intDeleteOrAdd);
//        QString telephone=q.value(0).toString();
//        QString date=q.value(5).toString();
//        if(!q.exec("UPDATE az_lessons SET Телефон_клиента=NULL WHERE Телефон_репетитора='"+telephone+"' AND День='"+date+"';")){
//            getMessageBox("Ошибка открытия таблицы занятий", true);
//            return;
//        }
//        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента='"+property("telephone").toString()+"' AND az_lessons.Было_занятие=false;")){
//            getMessageBox("Ошибка открытия таблиц", true);
//            return;
//        }
//        model->setQuery(q);
//        tableView->reset();
//    }
//    if(action->text()==tr("Отметить как прошедшее")){
//        QSqlQuery q(db);
//        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента='"+property("telephone").toString()+"' AND az_lessons.Было_занятие=false;")){
//            getMessageBox("Ошибка открытия таблиц", true);
//            return;
//        }
//        q.seek(intDeleteOrAdd);
//        QString telephone=q.value(0).toString();
//        QString date=q.value(5).toString();
//        if(!q.exec("UPDATE az_lessons SET Было_занятие=true WHERE Телефон_репетитора='"+telephone+"' AND День='"+date+"';")){
//            getMessageBox("Ошибка открытия таблицы занятий", true);
//            return;
//        }
//        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента='"+property("telephone").toString()+"' AND az_lessons.Было_занятие=false;")){
//            getMessageBox("Ошибка открытия таблиц", true);
//            return;
//        }
//        model->setQuery(q);
//        tableView->reset();
//    }
//}

//void MainWindow::currentTextChanged1(const QString str){
//    if(str!="<Дисциплина>"){
//        QSqlQuery q(db);
//        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+str+"';")){
//            getMessageBox("Таблица не открывается",true);
//            return;
//         }
//        model->setQuery(q);
//        tableView->reset();

//        person2.region=new QComboBox(this);
//        person2.region->addItem("<Регион>");
//        q.first();
//        QStringList list;
//        do{
//            list<<q.value(3).toString();
//        }while(q.next());
//        list.removeDuplicates();
//        for(auto a:list){
//            person2.region->addItem(a);
//        }

//        QHBoxLayout* lay=new QHBoxLayout();
//        lay->addWidget(person2.nominations);
//        lay->addWidget(person2.region);
//        QVBoxLayout* layout=new QVBoxLayout();
//        layout->addWidget(tableView);
//        layout->addLayout(lay);
//        QWidget* w=new QWidget(this);
//        w->setLayout(layout);
//        setCentralWidget(w);
//        connect(person2.region,SIGNAL(currentTextChanged(const QString)),SLOT(currentTextChanged2(const QString)));
//    }
//    else{
//        QAction* act=new QAction(tr("выбрать занятие"),this);
//        emit menuBar->triggered(act);
//    }
//}

//void MainWindow::currentTextChanged2(const QString str){
//    if(str!="<Регион>"){
//        QSqlQuery q(db);
//        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+person2.nominations->currentText()+"' AND az_tutors.Регион='"+str+"';")){
//            getMessageBox("Таблица не открывается",true);
//            return;
//         }
//        model->setQuery(q);
//        tableView->reset();

//        person2.price=new QComboBox(this);
//        person2.price->addItem("<Цена>");
//        q.first();
//        QStringList list;
//        do{
//            list<<q.value(4).toString();
//        }while(q.next());
//        list.removeDuplicates();
//        for(auto a:list){
//            person2.price->addItem(a);
//        }

//        QHBoxLayout* lay=new QHBoxLayout();
//        lay->addWidget(person2.nominations);
//        lay->addWidget(person2.region);
//        lay->addWidget(person2.price);
//        QVBoxLayout* layout=new QVBoxLayout();
//        layout->addWidget(tableView);
//        layout->addLayout(lay);
//        QWidget* w=new QWidget(this);
//        w->setLayout(layout);
//        setCentralWidget(w);
//        connect(person2.price,SIGNAL(currentTextChanged(const QString)),SLOT(currentTextChanged3(const QString)));
//    }
//    else{
//        emit currentTextChanged1(person2.nominations->currentText());
//    }
//}

//void MainWindow::currentTextChanged3(const QString str){
//    if(str!="<Цена>"){
//        QSqlQuery q(db);
//        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+person2.nominations->currentText()+"' AND az_tutors.Регион='"+person2.region->currentText()+"' AND az_lessons.Цена="+str+";")){
//            getMessageBox("Таблица не открывается",true);
//            return;
//        }
//        model->setQuery(q);
//        tableView->reset();

//        person2.date=new QComboBox(this);
//        person2.date->addItem("<Дата>");
//        q.first();
//        QStringList list;
//        do{
//            list<<q.value(5).toString();
//        }while(q.next());
//        list.removeDuplicates();
//        for(auto a:list){
//            person2.date->addItem(a);
//        }

//        QHBoxLayout* lay=new QHBoxLayout();
//        lay->addWidget(person2.nominations);
//        lay->addWidget(person2.region);
//        lay->addWidget(person2.price);
//        lay->addWidget(person2.date);
//        QVBoxLayout* layout=new QVBoxLayout();
//        layout->addWidget(tableView);
//        layout->addLayout(lay);
//        QWidget* w=new QWidget(this);
//        w->setLayout(layout);
//        setCentralWidget(w);
//        connect(person2.date,SIGNAL(currentTextChanged(const QString)),SLOT(currentTextChanged4(const QString)));
//    }
//    else{
//        emit currentTextChanged2(person2.region->currentText());
//    }
//}

//void MainWindow::currentTextChanged4(const QString str){
//    if(str!="<Дата>"){
//        QSqlQuery q(db);
//        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+person2.nominations->currentText()+"' AND az_tutors.Регион='"+person2.region->currentText()+"' AND az_lessons.Цена="+person2.price->currentText()+"AND az_lessons.День='"+str+"';")){
//            getMessageBox("Таблица не открывается",true);
//            return;
//         }
//        model->setQuery(q);
//        tableView->reset();

//        person2.time=new QComboBox(this);
//        person2.time->addItem("<Время_начала>");
//        q.first();
//        QStringList list;
//        do{
//            list<<q.value(6).toString();
//        }while(q.next());
//        list.removeDuplicates();
//        for(auto a:list){
//            person2.time->addItem(a);
//        }

//        QHBoxLayout* lay=new QHBoxLayout();
//        lay->addWidget(person2.nominations);
//        lay->addWidget(person2.region);
//        lay->addWidget(person2.price);
//        lay->addWidget(person2.date);
//        lay->addWidget(person2.time);
//        QVBoxLayout* layout=new QVBoxLayout();
//        layout->addWidget(tableView);
//        layout->addLayout(lay);
//        QWidget* w=new QWidget(this);
//        w->setLayout(layout);
//        setCentralWidget(w);
//        connect(person2.time,SIGNAL(currentTextChanged(const QString)),SLOT(currentTextChanged5(const QString)));
//    }
//    else{
//        emit currentTextChanged3(person2.price->currentText());
//    }
//}

//void MainWindow::currentTextChanged5(const QString str){
//    if(str!="<Время_начала>"){
//        QSqlQuery q(db);
//        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+person2.nominations->currentText()+"' AND az_tutors.Регион='"+person2.region->currentText()+"' AND az_lessons.Цена="+person2.price->currentText()+"AND az_lessons.День='"+person2.date->currentText()+"'AND az_lessons.Время_начала='"+str+"';")){
//            getMessageBox("Таблица не открывается",true);
//            return;
//         }
//        model->setQuery(q);
//        tableView->reset();
//    }
//    else{
//        emit currentTextChanged4(person2.date->currentText());
//    }
//}

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
    q.seek(intDeleteOrAdd);
    QString name=q.value(0).toString();

    if(!q.exec("SELECT lnr_nominations.Название_номинации, lnr_participants.Номер_зачётки, lnr_participants.ФИО, lnr_competition.Баллы FROM lnr_competition, lnr_nominations, lnr_participants WHERE lnr_nominations.Название_номинации = '"+ QString(name) +"' AND lnr_nominations.\"ID\" = lnr_competition.Номинация AND lnr_participants.Номер_зачётки = lnr_competition.Участница ;")){
            getMessageBox("Таблица не открывается",true);
            return;
    }

    int a=0;                                 //!!!
    QString name_id=q.value(1).toString();
    if(!q.exec("UPDATE lnr_competition SET Баллы='"+QString::number(a)+"' WHERE lnr_competition.Номинация='"+name_id+"';")){
        getMessageBox("Ошибка открытия таблицы соревнования 888888", true);
        return;
    }
    if(!q.exec("SELECT lnr_nominations.Название_номинации, lnr_participants.Номер_зачётки, lnr_participants.ФИО, lnr_competition.Баллы FROM lnr_competition, lnr_nominations, lnr_participants WHERE lnr_nominations.Название_номинации = '"+ QString(name) +"' AND lnr_nominations.\"ID\" = lnr_competition.Номинация AND lnr_participants.Номер_зачётки = lnr_competition.Участница ;")){
        getMessageBox("Ошибка открытия таблиц", true);
        return;
    }
    model->setQuery(q);
    tableView->reset();
}
