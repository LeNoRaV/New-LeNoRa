#include "mainwindow.h"

void MainWindow::tutorProfile(QString telephone){
    changeMenuBar({{"Добавить/удалить занятие"},{"Прошедшие занятия"},{"Другое","профиль жюри","настройки профиля репетитора","выйти"}});
    palette->setColor(QPalette::Window,Qt::lightGray);
    setPalette(*palette);
    setWindowTitle("Жюри");
    setProperty("telephone",telephone);
    QAction* act=new QAction("профиль жюри");
    emit menuBar->triggered(act);
}

void MainWindow::changeInfoTutor(){
    if(person.name->text()==nullptr ||person.password->text()==nullptr ||person.price->text()==nullptr){
        getMessageBox("Нельзя оставлять поля пустыми!",true);
        return;
    }
    if(person.telephone->text().count()!=11){
        getMessageBox("В номере телефона недостаточно цифр! Перепроверьте его.",true);
        return;
    }
    QSqlQuery q(db);
    if(!q.exec("UPDATE az_tutors SET Телефон='"+person.telephone->text()
               +"', ФИО='"+person.name->text()
               +"', Регион='"+person.region->currentText()
               +"', Цена='"+person.price->text()
               +"', Пароль='"+person.password->text()
               +"' WHERE Телефон='"+property("telephone").toString()+"';")){
        qDebug()<<q.lastError();
        getMessageBox("На этот телефон уже зарегистрирован репетитор",true);
        return;
    }
    setProperty("telephone",person.telephone->text());
    setProperty("price",person.price->text());

}

static int intDeleteOrAdd;
void MainWindow::slotDeleteLesson(QPoint point){
    intDeleteOrAdd=tableView->indexAt(point).row();
    QMenu* menu=new QMenu(this);
    QAction* deleteLesson=new QAction(tr("Удалить"),this);
    connect(deleteLesson,SIGNAL(triggered()),this,SLOT(slotDeleteLesson()));
    menu->addAction(deleteLesson);
    menu->exec(QCursor::pos());
}

void MainWindow::slotDeleteLesson(){
    QSqlQuery q(db);
    if(!q.exec("SELECT День FROM az_lessons WHERE Телефон_репетитора='"+property("telephone").toString()+"' AND Было_занятие=false;")){
        getMessageBox("Не открылась таблица с занятиями",true);
        return;
    }
    q.seek(intDeleteOrAdd);
    QString str=q.value(0).toString();
    if(!q.exec("DELETE FROM az_lessons WHERE Телефон_репетитора='"+property("telephone").toString()+"' AND День='"+str+"';")){
        getMessageBox("Не удалось удалить занятие",true);
        return;
    }
    if(!q.exec("SELECT Цена,День,Время_начала,Телефон_клиента FROM az_lessons WHERE Телефон_репетитора='"+property("telephone").toString()+"' AND Было_занятие=false;")){
        getMessageBox("Не открылась таблица с занятиями",true);
        return;
    }
    model->setQuery(q);
    tableView->reset();
}

void MainWindow::slotAddLesson(){
    QSqlQuery q(db);
    if(!q.exec("INSERT INTO az_clients VALUES('1','1','1');")) qDebug()<<"ERROR1111";
    if(!q.exec("INSERT INTO az_lessons VALUES ('"+property("telephone").toString()
               +"',"+property("price").toString()
               +",'"+lineEdit->text()
               +"','"+lineEdit2->text()
               +"','1',false);")){
        qDebug()<<q.lastError();
        getMessageBox("Не удаётся добавить занятие (скорее всего, в этот день у Вас уже есть занятие)",true);
    }
    if(!q.exec("DELETE FROM az_clients WHERE Телефон='1';")) qDebug()<<"ERROR1111";
    if(!q.exec("SELECT Цена,День,Время_начала,Телефон_клиента FROM az_lessons WHERE Телефон_репетитора='"+property("telephone").toString()+"' AND Было_занятие=false;")){
        getMessageBox("Не открылась таблица с занятиями",true);
        return;
    }
    model->setQuery(q);
    tableView->reset();
}
