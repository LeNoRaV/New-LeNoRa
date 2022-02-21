#include "mainwindow.h"

void MainWindow::clientProfile(QString telephone){
    changeMenuBar({{"Занятия","выбрать занятие","выбранные занятия","прошедшие занятия"},{"Другое","профиль клиента","настройки профиля клиента","выйти"}});
    palette->setColor(QPalette::Window,Qt::lightGray);
    setPalette(*palette);
    setWindowTitle("Клиент");
    setProperty("telephone",telephone);
    QAction* act=new QAction("профиль клиента");
    emit menuBar->triggered(act);
}

void MainWindow::changeInfoClient(){
    if(person.name->text()==nullptr ||person.password->text()==nullptr){
        getMessageBox("Нельзя оставлять поля пустыми!",true);
        return;
    }
    if(person.telephone->text().count()!=11){
        getMessageBox("В номере телефона недостаточно цифр! Перепроверьте его.",true);
        return;
    }
    QSqlQuery q(db);
    if(!q.exec("UPDATE az_clients SET Телефон='"+person.telephone->text()
               +"', ФИО='"+person.name->text()
               +"', Пароль='"+person.password->text()
               +"' WHERE Телефон='"+property("telephone").toString()+"';")){
        getMessageBox("На этот телефон уже зарегистрирован клиент",true);
        return;
    }
    setProperty("telephone",person.telephone->text());
}

static int intDeleteOrAdd;
void MainWindow::slotChangeLesson(QPoint point){
    intDeleteOrAdd=tableView->indexAt(point).row();
    QMenu* menu=new QMenu(this);
    QAction* action1=new QAction(tr("Отменить"),this);
    QAction* action2=new QAction(tr("Отметить как прошедшее"),this);
    connect(action1,SIGNAL(triggered()),this,SLOT(slotChangeLesson()));
    menu->addAction(action1);
    connect(action2,SIGNAL(triggered()),this,SLOT(slotChangeLesson()));
    menu->addAction(action2);
    menu->exec(QCursor::pos());
}

void MainWindow::slotChangeLesson(){
    QAction* action=qobject_cast<QAction*>(sender());
    if(action==nullptr) return;
    if(action->text()==tr("Отменить")){
        QSqlQuery q(db);
        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента='"+property("telephone").toString()+"' AND az_lessons.Было_занятие=false;")){
            getMessageBox("Ошибка открытия таблиц", true);
            return;
        }
        q.seek(intDeleteOrAdd);
        QString telephone=q.value(0).toString();
        QString date=q.value(5).toString();
        if(!q.exec("UPDATE az_lessons SET Телефон_клиента=NULL WHERE Телефон_репетитора='"+telephone+"' AND День='"+date+"';")){
            getMessageBox("Ошибка открытия таблицы занятий", true);
            return;
        }
        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента='"+property("telephone").toString()+"' AND az_lessons.Было_занятие=false;")){
            getMessageBox("Ошибка открытия таблиц", true);
            return;
        }
        model->setQuery(q);
        tableView->reset();
    }
    if(action->text()==tr("Отметить как прошедшее")){
        QSqlQuery q(db);
        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента='"+property("telephone").toString()+"' AND az_lessons.Было_занятие=false;")){
            getMessageBox("Ошибка открытия таблиц", true);
            return;
        }
        q.seek(intDeleteOrAdd);
        QString telephone=q.value(0).toString();
        QString date=q.value(5).toString();
        if(!q.exec("UPDATE az_lessons SET Было_занятие=true WHERE Телефон_репетитора='"+telephone+"' AND День='"+date+"';")){
            getMessageBox("Ошибка открытия таблицы занятий", true);
            return;
        }
        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента='"+property("telephone").toString()+"' AND az_lessons.Было_занятие=false;")){
            getMessageBox("Ошибка открытия таблиц", true);
            return;
        }
        model->setQuery(q);
        tableView->reset();
    }
}

void MainWindow::currentTextChanged1(const QString str){
    if(str!="<Дисциплина>"){
        QSqlQuery q(db);
        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+str+"';")){
            getMessageBox("Таблица не открывается",true);
            return;
         }
        model->setQuery(q);
        tableView->reset();

        person2.region=new QComboBox(this);
        person2.region->addItem("<Регион>");
        q.first();
        QStringList list;
        do{
            list<<q.value(3).toString();
        }while(q.next());
        list.removeDuplicates();
        for(auto a:list){
            person2.region->addItem(a);
        }

        QHBoxLayout* lay=new QHBoxLayout();
        lay->addWidget(person2.discipline);
        lay->addWidget(person2.region);
        QVBoxLayout* layout=new QVBoxLayout();
        layout->addWidget(tableView);
        layout->addLayout(lay);
        QWidget* w=new QWidget(this);
        w->setLayout(layout);
        setCentralWidget(w);
        connect(person2.region,SIGNAL(currentTextChanged(const QString)),SLOT(currentTextChanged2(const QString)));
    }
    else{
        QAction* act=new QAction(tr("выбрать занятие"),this);
        emit menuBar->triggered(act);
    }
}

void MainWindow::currentTextChanged2(const QString str){
    if(str!="<Регион>"){
        QSqlQuery q(db);
        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+person2.discipline->currentText()+"' AND az_tutors.Регион='"+str+"';")){
            getMessageBox("Таблица не открывается",true);
            return;
         }
        model->setQuery(q);
        tableView->reset();

        person2.price=new QComboBox(this);
        person2.price->addItem("<Цена>");
        q.first();
        QStringList list;
        do{
            list<<q.value(4).toString();
        }while(q.next());
        list.removeDuplicates();
        for(auto a:list){
            person2.price->addItem(a);
        }

        QHBoxLayout* lay=new QHBoxLayout();
        lay->addWidget(person2.discipline);
        lay->addWidget(person2.region);
        lay->addWidget(person2.price);
        QVBoxLayout* layout=new QVBoxLayout();
        layout->addWidget(tableView);
        layout->addLayout(lay);
        QWidget* w=new QWidget(this);
        w->setLayout(layout);
        setCentralWidget(w);
        connect(person2.price,SIGNAL(currentTextChanged(const QString)),SLOT(currentTextChanged3(const QString)));
    }
    else{
        emit currentTextChanged1(person2.discipline->currentText());
    }
}

void MainWindow::currentTextChanged3(const QString str){
    if(str!="<Цена>"){
        QSqlQuery q(db);
        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+person2.discipline->currentText()+"' AND az_tutors.Регион='"+person2.region->currentText()+"' AND az_lessons.Цена="+str+";")){
            getMessageBox("Таблица не открывается",true);
            return;
        }
        model->setQuery(q);
        tableView->reset();

        person2.date=new QComboBox(this);
        person2.date->addItem("<Дата>");
        q.first();
        QStringList list;
        do{
            list<<q.value(5).toString();
        }while(q.next());
        list.removeDuplicates();
        for(auto a:list){
            person2.date->addItem(a);
        }

        QHBoxLayout* lay=new QHBoxLayout();
        lay->addWidget(person2.discipline);
        lay->addWidget(person2.region);
        lay->addWidget(person2.price);
        lay->addWidget(person2.date);
        QVBoxLayout* layout=new QVBoxLayout();
        layout->addWidget(tableView);
        layout->addLayout(lay);
        QWidget* w=new QWidget(this);
        w->setLayout(layout);
        setCentralWidget(w);
        connect(person2.date,SIGNAL(currentTextChanged(const QString)),SLOT(currentTextChanged4(const QString)));
    }
    else{
        emit currentTextChanged2(person2.region->currentText());
    }
}

void MainWindow::currentTextChanged4(const QString str){
    if(str!="<Дата>"){
        QSqlQuery q(db);
        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+person2.discipline->currentText()+"' AND az_tutors.Регион='"+person2.region->currentText()+"' AND az_lessons.Цена="+person2.price->currentText()+"AND az_lessons.День='"+str+"';")){
            getMessageBox("Таблица не открывается",true);
            return;
         }
        model->setQuery(q);
        tableView->reset();

        person2.time=new QComboBox(this);
        person2.time->addItem("<Время_начала>");
        q.first();
        QStringList list;
        do{
            list<<q.value(6).toString();
        }while(q.next());
        list.removeDuplicates();
        for(auto a:list){
            person2.time->addItem(a);
        }

        QHBoxLayout* lay=new QHBoxLayout();
        lay->addWidget(person2.discipline);
        lay->addWidget(person2.region);
        lay->addWidget(person2.price);
        lay->addWidget(person2.date);
        lay->addWidget(person2.time);
        QVBoxLayout* layout=new QVBoxLayout();
        layout->addWidget(tableView);
        layout->addLayout(lay);
        QWidget* w=new QWidget(this);
        w->setLayout(layout);
        setCentralWidget(w);
        connect(person2.time,SIGNAL(currentTextChanged(const QString)),SLOT(currentTextChanged5(const QString)));
    }
    else{
        emit currentTextChanged3(person2.price->currentText());
    }
}

void MainWindow::currentTextChanged5(const QString str){
    if(str!="<Время_начала>"){
        QSqlQuery q(db);
        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+person2.discipline->currentText()+"' AND az_tutors.Регион='"+person2.region->currentText()+"' AND az_lessons.Цена="+person2.price->currentText()+"AND az_lessons.День='"+person2.date->currentText()+"'AND az_lessons.Время_начала='"+str+"';")){
            getMessageBox("Таблица не открывается",true);
            return;
         }
        model->setQuery(q);
        tableView->reset();
    }
    else{
        emit currentTextChanged4(person2.date->currentText());
    }
}

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
    if(person2.discipline->currentText()!="<Дисциплина>"){
        if(person2.region->currentText()!="<Регион>"){
            if(person2.price->currentText()!="<Цена>"){
                if(person2.date->currentText()!="<День>"){
                    if(person2.time->currentText()!="<Время_начала>"){
                        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+person2.discipline->currentText()+"' AND az_tutors.Регион='"+person2.region->currentText()+"' AND az_lessons.Цена="+person2.price->currentText()+"AND az_lessons.День='"+person2.date->currentText()+"'AND az_lessons.Время_начала='"+person2.time->currentText()+"';")){
                            getMessageBox("Таблица не открывается",true);
                            return;
                        }
                    }
                    else{
                        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+person2.discipline->currentText()+"' AND az_tutors.Регион='"+person2.region->currentText()+"' AND az_lessons.Цена="+person2.price->currentText()+"AND az_lessons.День='"+person2.date->currentText()+"';")){
                            getMessageBox("Таблица не открывается",true);
                            return;
                        }
                    }
                }
                else{
                    if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+person2.discipline->currentText()+"' AND az_tutors.Регион='"+person2.region->currentText()+"' AND az_lessons.Цена="+person2.price->currentText()+";")){
                        getMessageBox("Таблица не открывается",true);
                        return;
                    }
                }
            }
            else{
                if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+person2.discipline->currentText()+"' AND az_tutors.Регион='"+person2.region->currentText()+"';")){
                    getMessageBox("Таблица не открывается",true);
                    return;
                }
            }
        }
        else{
            if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL AND az_tutors.Дисциплина='"+person2.discipline->currentText()+"';")){
                getMessageBox("Таблица не открывается",true);
                return;
            }
        }
    }
    else{
        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL;")){
            getMessageBox("Таблица не открывается",true);
            return;
        }
    }
    q.seek(intDeleteOrAdd);
    QString telephone=q.value(0).toString();
    QString date=q.value(5).toString();
    if(!q.exec("UPDATE az_lessons SET Телефон_клиента='"+property("telephone").toString()+"' WHERE Телефон_репетитора='"+telephone+"' AND День='"+date+"';")){
        getMessageBox("Ошибка открытия таблицы занятий", true);
        return;
    }
    if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL;")){
        getMessageBox("Ошибка открытия таблиц", true);
        return;
    }
    model->setQuery(q);
    tableView->reset();
}
