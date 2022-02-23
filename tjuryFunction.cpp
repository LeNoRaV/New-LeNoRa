#include "mainwindow.h"

void MainWindow::juryProfile(QString telephone){
    changeMenuBar({{"Поставить баллы", "Мисс Бауман 2022"},{"Другое","профиль жюри","настройки профиля жюри","выйти"}});
    palette->setColor(QPalette::Window,Qt::darkGray);
    setPalette(*palette);
    setWindowTitle("Жюри");
    setProperty("telephone",telephone);
    QAction* act=new QAction("профиль жюри");
    emit menuBar->triggered(act);
}

void MainWindow::changeInfojury(){
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
        getMessageBox("Не открылась таблица с номинациями",true);
        return;
    }
    q.seek(intDeleteOrAdd);
    int str=q.value(0).toInt();
    if(!q.exec("DELETE FROM lnr_competition WHERE Участница='"+property("record_book").toString()+"' AND Номинация='"+QString::number(str)+"';")){
        getMessageBox("Не удалось удалить номинацию",true);
        return;
    }
    if(!q.exec("SELECT lnr_nominations.Название_номинации, lnr_competition.Баллы FROM lnr_nominations, lnr_competition WHERE lnr_competition.Номинация = lnr_nominations.\"ID\" AND lnr_competition.Участница = '"+property("record_book").toString()+"';")){
        getMessageBox("Не открылась таблица с номинациями",true);
        return;
    }
    model->setQuery(q);
    tableView->reset();
}

void MainWindow::slotAddLesson(){
    QSqlQuery q(db);
    QSqlQuery query(db);
    if(!q.exec("SELECT кол_во_номинаций FROM lnr_participants WHERE '"+property("record_book").toString()+"' = Номер_зачётки;")) {
        getMessageBox("Не открылась таблица с номинациями",true);
        return;
    };
    q.first();
    if (q.value(0).toInt() < 3){

        if(!q.exec("SELECT * FROM lnr_nominations WHERE '"+box->currentText()+"' = Название_номинации;")) {
            getMessageBox("Не открылась таблица с номинациями",true);
            return;
        };
        q.first();
    if(!query.exec("INSERT INTO lnr_competition VALUES ('"+property("record_book").toString()
               +"',"+QString::number(q.value(1).toInt())+");")){
        qDebug()<<query.lastError();
        getMessageBox("Не удаётся добавить номинацию",true);
    }
    model->setQuery(query);
    tableView->reset();
    }
    else {
        getMessageBox("Можно записаться не более чем на 3 номинации",true);
        return;
    }
}

