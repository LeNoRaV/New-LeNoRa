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

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void changeMenuBar(const QList<QStringList>);

    void adminProfile();
    void tutorProfile(QString);
    void clientProfile(QString);

    void getMessageBox(const QString,bool);
private slots:
    void slotTriggeredMenuBar(QAction*);

    void slotDeleteDiscipline(QPoint);
    void slotAddDiscipline();
    void slotDeleteDiscipline();

    void slotAddScore();

    void slotDeleteTutor2(QPoint);
    void slotDeleteTutor2();

    void changeInfoAdmin();

    void slotStatistics(int);

    void changeInfoTutor();

    void slotDeleteLesson(QPoint);
    void slotDeleteLesson();
    void slotAddLesson();

    void changeInfoClient();

    void slotChooseDiscipline(QPoint);
    void slotChooseDiscipline();
};
#endif // MAINWINDOW_H
