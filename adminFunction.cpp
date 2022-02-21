#include "mainwindow.h"

void MainWindow::adminProfile(){
    changeMenuBar({{"Добавить/удалить","добавить/удалить репетитора (из регистрирующихся)","удалить репетитора (из зарегистрированных)","добавить/удалить дисциплину","добавить/удалить регион", "удалить клиента"},{"Другое", "статистика","профиль администратора","настройки профиля администратора","выйти"}});

    palette->setColor(QPalette::Window,Qt::lightGray);
    setPalette(*palette);
    setWindowTitle("Администратор");
    QAction* act=new QAction("профиль администратора");
    emit menuBar->triggered(act);

    QSqlQuery q(db);
    if(!q.exec("SELECT COUNT(*) FROM az_reg_tutors;")){
        getMessageBox("Не открылась таблица с репетиторами на регистрацию",true);
        return;
    }
    q.first();
    if(q.value(0).toInt()!=0){
        QMessageBox* box=new QMessageBox(QMessageBox::Question,"Напоминание","У Вас есть незарегистрированные репетиторы. Вы хотите перейти к ним?");
        QPushButton* button1=box->addButton(tr("Да"),QMessageBox::YesRole);
        QPushButton* button2=box->addButton(tr("Нет"),QMessageBox::NoRole);
        box->exec();
        if(box->clickedButton()==button1){
            QAction* act=new QAction(tr("добавить/удалить репетитора (из регистрирующихся)"),this);
            emit menuBar->triggered(act);
        }
    }
}

static int intDeleteOrAdd;
void MainWindow::slotPlusAndDeleteTutor(QPoint point){
    intDeleteOrAdd=tableView->indexAt(point).row();
    QMenu* menu=new QMenu(this);
    QAction* add=new QAction(tr("Добавить"),this);
    QAction* deleteTutor=new QAction(tr("Удалить"),this);
    connect(add,SIGNAL(triggered()),this,SLOT(slotAddTutor()));
    connect(deleteTutor,SIGNAL(triggered()),this,SLOT(slotDeleteTutor()));
    menu->addAction(add);
    menu->addAction(deleteTutor);
    menu->exec(QCursor::pos());
}

void MainWindow::slotAddTutor(){
    QSqlQuery q(db);
    if(!q.exec("SELECT * FROM az_reg_tutors;")){
        getMessageBox("Не открылась таблица с репетиторами на регистрацию",true);
        return;
    }
    q.seek(intDeleteOrAdd);
    QString str=q.value(0).toString();
    if(!q.exec("INSERT INTO az_tutors VALUES ('"+str+"','"+q.value(1).toString()
               +"','"+q.value(2).toString()+"','"+q.value(3).toString()
               +"',"+QString::number(q.value(4).toInt())+",'"+q.value(5).toString()+"');")){
        getMessageBox("На этот номер уже был зарегистрирован репетитор",true);
        ++intDeleteOrAdd;
        return;
    }
    if(!q.exec("DELETE FROM az_reg_tutors WHERE Телефон='"+str+"';")){
        getMessageBox("Не удалось удалить из списка репетиторов на регистрацию",true);
        return;
    }
    if(!q.exec("SELECT * FROM az_reg_tutors;")){
        getMessageBox("Не открылась таблица с репетиторами на регистрацию",true);
        return;
    }
    model->setQuery(q);
    tableView->reset();
}

void MainWindow::slotDeleteTutor(){
    QSqlQuery q(db);
    if(!q.exec("SELECT * FROM az_reg_tutors;")){
        getMessageBox("Не открылась таблица с репетиторами на регистрацию",true);
        return;
    }
    q.seek(intDeleteOrAdd);
    QString str=q.value(0).toString();
    if(!q.exec("DELETE FROM az_reg_tutors WHERE Телефон='"+str+"';")){
        getMessageBox("Не удалось удалить из списка репетиторов на регистрацию",true);
        return;
    }
    if(!q.exec("SELECT * FROM az_reg_tutors;")){
        getMessageBox("Не открылась таблица с репетиторами на регистрацию",true);
        return;
    }
    model->setQuery(q);
    tableView->reset();
}

void MainWindow::slotAddAllTutors(){
    intDeleteOrAdd=0;
    QSqlQuery q(db);
    if(!q.exec("SELECT COUNT(*) FROM az_reg_tutors;")){
        getMessageBox("Не открылась таблица с репетиторами на регистрацию",true);
        return;
    }
    q.first();
    for(int i=q.value(0).toInt();i>0;--i){
        slotAddTutor();
    }
}

void MainWindow::slotDeleteAllTutors(){
    intDeleteOrAdd=0;
    QSqlQuery q(db);
    if(!q.exec("SELECT COUNT(*) FROM az_reg_tutors;")){
        getMessageBox("Не открылась таблица с репетиторами на регистрацию",true);
        return;
    }
    q.first();
    for(int i=q.value(0).toInt();i>0;--i){
        slotDeleteTutor();
    }
}


void MainWindow::slotDeleteDiscipline(QPoint point){
    intDeleteOrAdd=tableView->indexAt(point).row();
    QMenu* menu=new QMenu(this);
    QAction* deleteDiscipline=new QAction(tr("Удалить"),this);
    connect(deleteDiscipline,SIGNAL(triggered()),this,SLOT(slotDeleteDiscipline()));
    menu->addAction(deleteDiscipline);
    menu->exec(QCursor::pos());
}

void MainWindow::slotDeleteDiscipline(){
    QSqlQuery q(db);
    if(!q.exec("SELECT * FROM az_disciplines;")){
        getMessageBox("Не открылась таблица с дисциплинами",true);
        return;
    }
    q.seek(intDeleteOrAdd);
    QString str=q.value(0).toString();
    if(!q.exec("DELETE FROM az_disciplines WHERE Дисциплина='"+str+"';")){
        getMessageBox("Не удалось удалить дисциплину (есть репетиторы, ведущие данную дисциплину)",true);
        return;
    }
    if(!q.exec("SELECT * FROM az_disciplines;")){
        getMessageBox("Не открылась таблица с дисциплинами",true);
        return;
    }
    model->setQuery(q);
    tableView->reset();
}

void MainWindow::slotAddDiscipline(){
    QSqlQuery q(db);
    if(lineEdit->text()==nullptr){
        getMessageBox("Введите название номинации",true);
        return;
    }
    if(!q.exec("INSERT INTO lnr_nominations VALUES ('"+lineEdit->text()+"');")){
        getMessageBox("Не удаётся добавить номинацию",true);
        return;
    }
    if(!q.exec("SELECT * FROM lnr_nominations;")){
        getMessageBox("Не открылась таблица с номинациями",true);
        return;
    }
    model->setQuery(q);
    tableView->reset();
}

void MainWindow::slotDeleteRegion(QPoint point){
    intDeleteOrAdd=tableView->indexAt(point).row();
    QMenu* menu=new QMenu(this);
    QAction* deleteRegion=new QAction(tr("Удалить"),this);
    connect(deleteRegion,SIGNAL(triggered()),this,SLOT(slotDeleteRegion()));
    menu->addAction(deleteRegion);
    menu->exec(QCursor::pos());
}

void MainWindow::slotDeleteRegion(){
    QSqlQuery q(db);
    if(!q.exec("SELECT * FROM az_regions;")){
        getMessageBox("Не открылась таблица с регионами",true);
        return;
    }
    q.seek(intDeleteOrAdd);
    QString str=q.value(0).toString();
    if(!q.exec("DELETE FROM az_regions WHERE Регион='"+str+"';")){
        getMessageBox("Не удалось удалить регион (есть репетитор, проживающий в данном регионе)",true);
        return;
    }
    if(!q.exec("SELECT * FROM az_regions;")){
        getMessageBox("Не открылась таблица с регионами",true);
        return;
    }
    model->setQuery(q);
    tableView->reset();
}

void MainWindow::slotAddRegion(){
    QSqlQuery q(db);
    if(lineEdit->text()==nullptr){
        getMessageBox("Нельзя добавить то, чего нет",true);
        return;
    }
    if(!q.exec("INSERT INTO az_regions VALUES ('"+lineEdit->text()+"');")){
        getMessageBox("Не удаётся добавить регион (скорее всего, такой регион уже есть)",true);
        return;
    }
    if(!q.exec("SELECT * FROM az_regions;")){
        getMessageBox("Не открылась таблица с регионами",true);
        return;
    }
    model->setQuery(q);
    tableView->reset();
}

void MainWindow::slotDeleteTutor2(QPoint point){
    intDeleteOrAdd=tableView->indexAt(point).row();
    QMenu* menu=new QMenu(this);
    QAction* deleteTutor=new QAction(tr("Удалить"),this);
    connect(deleteTutor,SIGNAL(triggered()),this,SLOT(slotDeleteTutor2()));
    menu->addAction(deleteTutor);
    menu->exec(QCursor::pos());
}

void MainWindow::slotDeleteTutor2(){
    QSqlQuery q(db);
    if(!q.exec("SELECT * FROM lnr_participants;")){
        getMessageBox("Не открылась таблица с участницами",true);
        return;
    }
    q.seek(intDeleteOrAdd);
    QString str=q.value(0).toString();
    if(!q.exec("DELETE FROM lnr_participants WHERE Номер_зачётки='"+str+"';")){
        getMessageBox("Не удалось удалить из списка участниц",true);
        return;
    }
    if(!q.exec("SELECT * FROM lnr_participants;")){
        getMessageBox("Не открылась таблица с участницами",true);
        return;
    }
    model->setQuery(q);
    tableView->reset();
}

void MainWindow::slotDeleteClient(QPoint point){
    intDeleteOrAdd=tableView->indexAt(point).row();
    QMenu* menu=new QMenu(this);
    QAction* deleteTutor=new QAction(tr("Удалить"),this);
    connect(deleteTutor,SIGNAL(triggered()),this,SLOT(slotDeleteClient()));
    menu->addAction(deleteTutor);
    menu->exec(QCursor::pos());
}

void MainWindow::slotDeleteClient(){
    QSqlQuery q(db);
    if(!q.exec("SELECT * FROM az_clients;")){
        getMessageBox("Не открылась таблица с клиентами",true);
        return;
    }
    q.seek(intDeleteOrAdd);
    QString str=q.value(0).toString();
    if(!q.exec("DELETE FROM az_clients WHERE Телефон='"+str+"';")){
        getMessageBox("Не удалось удалить из списка клиентов",true);
        return;
    }
    if(!q.exec("SELECT * FROM az_clients;")){
        getMessageBox("Не открылась таблица с клиентами",true);
        return;
    }
    model->setQuery(q);
    tableView->reset();
}

void MainWindow::changeInfoAdmin(){
    if(person.name->text()==nullptr ||person.password->text()==nullptr){
        getMessageBox("Нельзя оставлять поля пустыми!",true);
        return;
    }
    if(person.telephone->text().count()!=11){
        getMessageBox("В номере телефона недостаточно цифр! Перепроверьте его.",true);
        return;
    }
    QFile file("C:/Users/lesko/Desktop/C++/DB3-jul/New-LeNoRa/files/admin.txt");
    if(!file.open(QIODevice::WriteOnly)){
        getMessageBox("Не открывается файл",true);
        return;
    }
    QString str=person.telephone->text()+","+person.name->text()+","+person.password->text();
    file.write(str.toUtf8());
}

void MainWindow::slotStatistics(int numberSort){
    QSqlQuery q(db);
    if(numberSort==0){
        if(!q.exec("SELECT SUM(Цена), Телефон_репетитора FROM az_lessons WHERE Было_занятие=true GROUP BY Телефон_репетитора ORDER BY SUM(Цена) DESC;")){
            qDebug()<<q.lastError();
                getMessageBox("Не открылась таблица с занятиями",true);
                return;
        }
        model->setQuery(q);
        tableView->setModel(model);
        tableView->reset();

        q.first();
        int sum=0;
        int kol=0;
        do{
            sum+=q.value(0).toInt();
            ++kol;
        }while(q.next());
        label->setText("Средний заработок репетиторов: "+QString::number(sum/kol)+"руб.");
    }
    if(numberSort==1){
        if(!q.exec("SELECT t2.Телефон_репетитора,t2.Кол_во_провед,t1.Общ_кол_во,CAST(t2.Кол_во_провед as FLOAT) / CAST(t1.Общ_кол_во as FLOAT) as Отношение FROM (SELECT COUNT(*) as Общ_кол_во,Телефон_репетитора FROM az_lessons GROUP BY Телефон_репетитора) t1 INNER  JOIN (SELECT COUNT(*) as Кол_во_провед,Телефон_репетитора FROM az_lessons WHERE Было_занятие=true GROUP BY Телефон_репетитора)t2 ON t1.Телефон_репетитора=t2.Телефон_репетитора ORDER BY Отношение DESC;")){
            qDebug()<<q.lastError();
                getMessageBox("Не открылась таблица с занятиями",true);
                return;
        }
        model->setQuery(q);
        tableView->setModel(model);
        tableView->reset();

        q.first();
        double sum=0;
        int kol=0;
        do{
            sum+=q.value(3).toDouble();
            ++kol;
        }while(q.next());
        label->setText("Среднее отношение проведённого количества занятий к общему числу: "+QString::number(sum/kol,'f',5));
    }
    if(numberSort==2){
        if(!q.exec("SELECT Телефон_клиента,COUNT(*) as Кол_во_занятий FROM az_lessons WHERE Было_занятие=true GROUP BY Телефон_клиента ORDER BY Кол_во_занятий DESC;")){
            qDebug()<<q.lastError();
                getMessageBox("Не открылась таблица с занятиями",true);
                return;
        }
        model->setQuery(q);
        tableView->setModel(model);
        tableView->reset();

        q.first();
        int sum=0;
        int kol=0;
        do{
            sum+=q.value(1).toInt();
            ++kol;
        }while(q.next());
        label->setText("Среднее количество занятий у клиентов: "+QString::number(sum/kol));
    }
    if(numberSort==3){
        if(!q.exec("SELECT t1.Дисциплина, t1.Кол_во_реп, t2.Кол_во_клиентов,CAST(t2.Кол_во_клиентов as FLOAT) / CAST(t1.Кол_во_реп as FLOAT) as Отношение FROM "
                   "(SELECT COUNT(*) as Кол_во_реп, Дисциплина FROM az_tutors GROUP BY Дисциплина) t1 INNER JOIN "
                   "(SELECT COUNT(*) as Кол_во_клиентов, az_tutors.Дисциплина as Дисциплина FROM az_tutors INNER JOIN az_lessons ON az_tutors.Телефон=az_lessons.Телефон_репетитора WHERE Телефон_клиента is NOT NULL GROUP BY Дисциплина) t2 "
                   "ON t1.Дисциплина=t2.Дисциплина ORDER BY Отношение DESC;")){
            qDebug()<<q.lastError();
                getMessageBox("Не открылась таблица с занятиями",true);
                return;
        }
        model->setQuery(q);
        tableView->setModel(model);
        tableView->reset();

        q.first();
        double sum=0;
        int kol=0;
        do{
            sum+=q.value(3).toDouble();
            ++kol;
        }while(q.next());
        label->setText("Средний значение отношения: "+QString::number(sum/kol,'f',5));
    }
    if(numberSort==4){
        if(!q.exec("SELECT t1.Регион, t1.Кол_во_реп, t2.Кол_во_клиентов,CAST(t2.Кол_во_клиентов as FLOAT) / CAST(t1.Кол_во_реп as FLOAT) as Отношение FROM "
                   "(SELECT COUNT(*) as Кол_во_реп, Регион FROM az_tutors GROUP BY Регион) t1 INNER JOIN "
                   "(SELECT COUNT(*) as Кол_во_клиентов, az_tutors.Регион as Регион FROM az_tutors INNER JOIN az_lessons ON az_tutors.Телефон=az_lessons.Телефон_репетитора WHERE Телефон_клиента is NOT NULL GROUP BY Регион) t2 "
                   "ON t1.Регион=t2.Регион ORDER BY Отношение DESC;")){
            qDebug()<<q.lastError();
                getMessageBox("Не открылась таблица с занятиями",true);
                return;
        }
        model->setQuery(q);
        tableView->setModel(model);
        tableView->reset();

        q.first();
        double sum=0;
        int kol=0;
        do{
            sum+=q.value(3).toDouble();
            ++kol;
        }while(q.next());
        label->setText("Средний значение отношения: "+QString::number(sum/kol,'f',5));
    }
    if(numberSort==5){
        if(!q.exec("SELECT SUM(Цена), Телефон_клиента FROM az_lessons WHERE Было_занятие=true GROUP BY Телефон_клиента ORDER BY SUM(Цена) DESC;")){
            qDebug()<<q.lastError();
                getMessageBox("Не открылась таблица с занятиями",true);
                return;
        }
        model->setQuery(q);
        tableView->setModel(model);
        tableView->reset();

        q.first();
        int sum=0;
        int kol=0;
        do{
            sum+=q.value(0).toInt();
            ++kol;
        }while(q.next());
        label->setText("Средние траты клиентов: "+QString::number(sum/kol)+"руб.");
    }
}
