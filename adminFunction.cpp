#include "mainwindow.h"

void MainWindow::adminProfile(){
    changeMenuBar({{"Добавить/удалить","удалить участницу","добавить или удалить номинацию"},{"Другое", "топ","профиль администратора","настройки профиля администратора","выйти"}});

    palette->setColor(QPalette::Window,Qt::darkGray);
    setPalette(*palette);
    setWindowTitle("Администратор");
    QAction* act=new QAction("профиль администратора");
    emit menuBar->triggered(act);
}

static int intDeleteOrAdd;
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
    if(!q.exec("SELECT * FROM lnr_nominations;")){
        getMessageBox("Не открылась таблица с номинациями",true);
        return;
    }
    q.seek(intDeleteOrAdd);
    QString str=q.value(0).toString();
    if(!q.exec("DELETE FROM lnr_nominations WHERE Название_номинации='"+str+"';")){
        getMessageBox("Не удалось удалить номинацию",true);
        return;
    }
    if(!q.exec("SELECT * FROM lnr_nominations;")){
        getMessageBox("Не открылась таблица с номинациями",true);
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

void MainWindow::slotDeletejury2(QPoint point){  // delete participant
    intDeleteOrAdd=tableView->indexAt(point).row();
    QMenu* menu=new QMenu(this);
    QAction* deletejury=new QAction(tr("Удалить"),this);
    connect(deletejury,SIGNAL(triggered()),this,SLOT(slotDeletejury2()));
    menu->addAction(deletejury);
    menu->exec(QCursor::pos());
}

void MainWindow::slotDeletejury2(){
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

void MainWindow::changeInfoAdmin(){
    if(person.name->text()==nullptr ||person.password->text()==nullptr){
        getMessageBox("Нельзя оставлять поля пустыми!",true);
        return;
    }
    QFile file("C:/Users/lesko/Desktop/C++/DB3-jul/New-LeNoRa/files/admin.txt");
    if(!file.open(QIODevice::WriteOnly)){
        getMessageBox("Не открывается файл",true);
        return;
    }
    QString str=person.login->text()+","+person.name->text()+","+person.password->text();
    file.write(str.toUtf8());
}

void MainWindow::slotStatistics(int numberSort){
    QSqlQuery q(db);
    if(numberSort==0){
        if(!q.exec("SELECT Номер_зачётки, ФИО, кол_во_номинаций, Общие_баллы FROM lnr_participants ORDER BY Общие_баллы DESC;")){
            qDebug()<<q.lastError();
                getMessageBox("Не открылась таблица с участницами",true);
                return;
        }
        model->setQuery(q);
        tableView->setModel(model);
        tableView->reset();
    }
    if(numberSort==1){
        if(!q.exec("SELECT DISTINCT lnr_nominations.Название_номинации, lnr_participants.Номер_зачётки, lnr_participants.ФИО, lnr_competition.Баллы  FROM lnr_nominations, lnr_participants, lnr_competition WHERE lnr_competition.Баллы = (SELECT MAX(lnr_competition.Баллы) FROM lnr_competition,lnr_participants WHERE lnr_participants.Номер_зачётки = lnr_competition.Участница AND lnr_nominations.\"ID\" = lnr_competition.Номинация) And lnr_participants.Номер_зачётки = lnr_competition.Участница")){
            qDebug()<<q.lastError();
                getMessageBox("Не открылась таблица с номинациями",true);
                return;
        }

        model->setQuery(q);
        tableView->setModel(model);
        tableView->reset();
    }
}
