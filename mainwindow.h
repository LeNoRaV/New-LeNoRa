#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QtSql>
#include <QLayout>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "mydialog.h"

struct Person{
    QLineEdit* login;
    QLineEdit* name;
    QLineEdit* weight;
    QLineEdit* height;
    QLineEdit* age;
    QLineEdit* talant;
    QLineEdit* password;
};

struct Person2{
    QComboBox* nominations;
    QComboBox* date;
    QComboBox* time;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QSqlDatabase db;
    QTableView* tableView;
    QSqlQueryModel* model;
    QMenuBar* menuBar;

    QLineEdit* lineEdit;
    QLineEdit* lineEdit2;
    Person person;
    Person2 person2;
    QPalette* palette;
    QComboBox* box;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void changeMenuBar(const QList<QStringList>);

    void adminProfile();
    void juryProfile(QString);
    void participantProfile(QString);

    void getMessageBox(const QString,bool);
private slots:
    void slotTriggeredMenuBar(QAction*);

    void slotDeleteDiscipline(QPoint);
    void slotAddDiscipline();
    void slotDeleteDiscipline();

    void slotAddScore();

    void slotDeletejury2(QPoint);
    void slotDeletejury2();

    void changeInfoAdmin();

    void slotStatistics(int);

    void changeInfojury();

    void slotDeleteLesson(QPoint);
    void slotDeleteLesson();
    void slotAddLesson();

    void changeInfoparticipant();

    void slotChooseDiscipline(QPoint);
    void slotChooseDiscipline();
};
#endif // MAINWINDOW_H
