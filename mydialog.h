#ifndef MYDIALOG_H
#define MYDIALOG_H

#include "mainwindow.h"


//для ввода баллов
class MyDialogScore : public QDialog
{
    Q_OBJECT

private:
    QSqlDatabase* db;
    QLineEdit* score;

    QString* result;
public:
    explicit MyDialogScore(QSqlDatabase* db1, QString* result1, QWidget *parent = nullptr);

    void getMessageBox(const QString textError,bool error);
private slots:
    void slotEnter();
};

//Диалог входа
class MyDialogEnter : public QDialog
{
    Q_OBJECT

private:
    QSqlDatabase* db;
    QLineEdit* login;
    QLineEdit* password;
    QComboBox* box;

    QString* result;
public:
    explicit MyDialogEnter(QSqlDatabase* db1, QString* result1, QWidget *parent = nullptr);

    void getMessageBox(const QString textError,bool error);
private slots:
    void slotEnter();
    void slotTriggeredMenuBar(QAction* action);
};


//Диалог регистрации (и клиентов, и репетиторов)
class MyDialogRegistration : public QDialog
{
    Q_OBJECT

public:
    explicit MyDialogRegistration(QSqlDatabase* db1, QWidget *parent=nullptr);

    void getMessageBox(const QString textError,bool error);
private:
    QSqlDatabase* db;

    QLineEdit* record_book;
    QLineEdit* name;
    QLineEdit* weight;
    QLineEdit* height;
    QLineEdit* age;
    QLineEdit* talant;
    QLineEdit* password;

private slots:
    void slotEnter();
};

#endif // MYDIALOG_H
