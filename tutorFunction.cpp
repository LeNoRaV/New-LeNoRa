#include "mainwindow.h"

void MainWindow::tutorProfile(QString telephone){
    changeMenuBar({{"Поставить баллы", "Мисс Бауман 2022"},{"Другое","профиль жюри","настройки профиля жюри","выйти"}});
    palette->setColor(QPalette::Window,Qt::lightGray);
    setPalette(*palette);
    setWindowTitle("Жюри");
    setProperty("telephone",telephone);
    QAction* act=new QAction("профиль жюри");
    emit menuBar->triggered(act);
}

void MainWindow::changeInfoTutor(){
    if(person.name->text()==nullptr ||person.password->text()==nullptr){
        getMessageBox("Нельзя оставлять поля пустыми!",true);
        return;
    }
    QFile file("C:/Users/lesko/Desktop/C++/DB3-jul/New-LeNoRa/files/jury.txt");
    if(!file.open(QIODevice::WriteOnly)){
        getMessageBox("Не открывается файл",true);
        return;
    }
    QString str=person.login->text()+","+person.name->text()+","+person.password->text();
    file.write(str.toUtf8());
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
    if(!q.exec("SELECT lnr_nominations.\"ID\", lnr_competition.Баллы FROM lnr_nominations, lnr_competition WHERE lnr_competition.Номинация = lnr_nominations.\"ID\" AND lnr_competition.Участница = '"+property("record_book").toString()+"';")){
        getMessageBox("Не открылась таблица с занятиями",true);
        return;
    }
    q.seek(intDeleteOrAdd);
    int str=q.value(0).toInt();
    if(!q.exec("DELETE FROM lnr_competition WHERE Участница='"+property("record_book").toString()+"' AND Номинация='"+QString::number(str)+"';")){
        getMessageBox("Не удалось удалить занятие",true);
        return;
    }
    if(!q.exec("SELECT lnr_nominations.Название_номинации, lnr_competition.Баллы FROM lnr_nominations, lnr_competition WHERE lnr_competition.Номинация = lnr_nominations.\"ID\" AND lnr_competition.Участница = '"+property("record_book").toString()+"';")){
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

//void MainWindow::slotAddScore(){
//    QSqlQuery q(db);
//    if(lineEdit->text()==nullptr){
//        getMessageBox("Введите название номинации",true);
//        return;
//    }
//    if(!q.exec("INSERT INTO lnr_nominations VALUES ('"+lineEdit->text()+"');")){
//        getMessageBox("Не удаётся добавить номинацию",true);
//        return;
//    }
//    if(!q.exec("SELECT * FROM lnr_nominations;")){
//        getMessageBox("Не открылась таблица с номинациями",true);
//        return;
//    }
//    model->setQuery(q);
//    tableView->reset();
//}
