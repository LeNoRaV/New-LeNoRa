#include "mainwindow.h"

//для получения MessageBox
void MainWindow::getMessageBox(const QString textError,bool error){
    QMessageBox msgBox;
    if(error) msgBox.setIcon(QMessageBox::Critical);
    else msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(textError);
    msgBox.exec();
}

//Слот для изменения menuBar
void MainWindow::changeMenuBar(const QList<QStringList> listAction){
    menuBar->clear();
    for(auto a:listAction){
        if(a.count()==1) menuBar->addAction(a.at(0));
        else{
            QMenu* menu=new QMenu(a.at(0),this);
            for(int i=1;i<a.count();++i){
                menu->addAction(a.at(i));
            }
            menuBar->addMenu(menu);
        }
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
//    setWindowIcon(QIcon(":/MyPictures/pictures/mainIcon.jpg"));
    setWindowTitle(tr("Главная страница"));
    tableView=new QTableView();
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    model=new QSqlTableModel();

    //Настройка картинки на главной странице
    QPixmap bkgnd(":/MyPictures/pictures/mainPicture.jfif");
    setFixedSize(QSize(bkgnd.width(),bkgnd.height()));
    palette=new QPalette();
    palette->setBrush(QPalette::Window, bkgnd);
    setPalette(*palette);

    menuBar=new QMenuBar(this);
    changeMenuBar({{tr("&Войти")},{tr("&Участницы")}});

    setMenuBar(menuBar);
    connect(menuBar,SIGNAL(triggered(QAction*)),this,SLOT(slotTriggeredMenuBar(QAction*)));

    //работа с бд
    db=QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("fn1132_2021");
    db.setPort(5432);
    db.setUserName("student");
    db.setPassword("bmstu");
    db.setHostName("195.19.32.74");

    if (!db.open()){
        getMessageBox("Не был получен доступ к базе данных",true);
        return;
    }
}

MainWindow::~MainWindow()
{
    delete tableView;
    delete model;
    delete palette;
}

//слот всевозможных кнопок меню
void MainWindow::slotTriggeredMenuBar(QAction* action){
    //Меню до входа
    if(action->text()==tr("&Войти")){
        QString whoAndPhone;
        MyDialogEnter(&db,&whoAndPhone).exec();
        if(whoAndPhone.section(",",0,0)=="админ"){
            adminProfile();
        }
        if(whoAndPhone.section(",",0,0)=="жюри"){
            tutorProfile(whoAndPhone.section(",",1,1));
        }
        if(whoAndPhone.section(",",0,0)=="участница"){
            clientProfile(whoAndPhone.section(",",1,1));
        }
    }
    if(action->text()==tr("&Участницы")){
        setWindowTitle(tr("Участницы"));
        QSqlQuery query (db);
        if(!query.exec("SELECT Номер_зачётки, ФИО, Вес, Рост, Возраст FROM lnr_participants;")){
            getMessageBox("Таблица с участницами не открылась",true);
            return;
        }
        model->setQuery(query);
        tableView->setModel(model);
        setCentralWidget(tableView);
        changeMenuBar({{tr("&Войти")}});
    }
//    if(action->text()==tr("Вернуться на главную")){
//        setWindowTitle(tr("Главная страница"));
//        delete tableView;
//        tableView=new QTableView();
//        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//        changeMenuBar({{tr("&Войти")},{tr("&Репетиторы")}});
//    }

    //меню админа
//    if(action->text()==tr("добавить/удалить репетитора (из регистрирующихся)")){
//        QSqlQuery query(db);
//        if(!query.exec("SELECT * FROM az_reg_tutors;")){
//            getMessageBox("Таблица с репетиторами не открылась",true);
//            return;
//        }
//        delete centralWidget();
//        tableView=new QTableView();
//        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//        model->setQuery(query);
//        tableView->setModel(model);
//        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
//        connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotPlusAndDeleteTutor(QPoint)));

//        QPushButton* addAll=new QPushButton("&Добавить всех",this);
//        QPushButton* deleteAll=new QPushButton("&Удалить всех",this);
//        QHBoxLayout* layout1=new QHBoxLayout();
//        layout1->addWidget(addAll);
//        layout1->addWidget(deleteAll);
//        QVBoxLayout* layout2=new QVBoxLayout();
//        layout2->addWidget(tableView);
//        layout2->addLayout(layout1);
//        QWidget* widget=new QWidget(this);
//        widget->setLayout(layout2);
//        setCentralWidget(widget);

//        connect(addAll,SIGNAL(pressed()),this,SLOT(slotAddAllTutors()));
//        connect(deleteAll,SIGNAL(pressed()),this,SLOT(slotDeleteAllTutors()));
//    }
    if(action->text()==tr("удалить репетитора (из зарегистрированных)")){
        QSqlQuery query(db);
        if(!query.exec("SELECT * FROM az_tutors;")){
            getMessageBox("Таблица с репетиторами не открылась",true);
            return;
        }
        delete centralWidget();
        tableView=new QTableView();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        model->setQuery(query);
        tableView->setModel(model);
        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotDeleteTutor2(QPoint)));
        setCentralWidget(tableView);
    }
    if(action->text()==tr("добавить/удалить дисциплину")){
        QSqlQuery query(db);
        if(!query.exec("SELECT * FROM az_disciplines;")){
            getMessageBox("Таблица с дисциплинами не открылась",true);
            return;
        }
        delete centralWidget();
        tableView=new QTableView();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        model->setQuery(query);
        tableView->setModel(model);
        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotDeleteDiscipline(QPoint)));

        QPushButton* add=new QPushButton("&Добавить",this);
        lineEdit=new QLineEdit(this);
        lineEdit->setPlaceholderText("Введите дисциплину");
        QVBoxLayout* layout1=new QVBoxLayout();
        layout1->addWidget(lineEdit);
        layout1->addWidget(add);
        layout1->addStretch(1);
        QHBoxLayout* layout2=new QHBoxLayout();
        layout2->addWidget(tableView);
        layout2->addLayout(layout1);
        QWidget* widget=new QWidget(this);
        widget->setLayout(layout2);
        setCentralWidget(widget);

        connect(add,SIGNAL(pressed()),this,SLOT(slotAddDiscipline()));
    }
    if(action->text()==tr("добавить/удалить регион")){
        QSqlQuery query(db);
        if(!query.exec("SELECT * FROM az_regions;")){
            getMessageBox("Таблица с регионами не открылась",true);
            return;
        }
        delete centralWidget();
        tableView=new QTableView();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        model->setQuery(query);
        tableView->setModel(model);
        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotDeleteRegion(QPoint)));

        QPushButton* add=new QPushButton("&Добавить",this);
        lineEdit=new QLineEdit(this);
        lineEdit->setPlaceholderText("Введите регион");
        QVBoxLayout* layout1=new QVBoxLayout();
        layout1->addWidget(lineEdit);
        layout1->addWidget(add);
        layout1->addStretch(1);
        QHBoxLayout* layout2=new QHBoxLayout();
        layout2->addWidget(tableView);
        layout2->addLayout(layout1);
        QWidget* widget=new QWidget(this);
        widget->setLayout(layout2);
        setCentralWidget(widget);

        connect(add,SIGNAL(pressed()),this,SLOT(slotAddRegion()));
    }
    if(action->text()==tr("удалить клиента")){
        QSqlQuery query(db);
        if(!query.exec("SELECT * FROM az_clients;")){
            getMessageBox("Таблица с клиентами не открылась",true);
            return;
        }
        delete centralWidget();
        tableView=new QTableView();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        model->setQuery(query);
        tableView->setModel(model);
        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotDeleteClient(QPoint)));
        setCentralWidget(tableView);
    }
    if(action->text()==tr("статистика")){
        QLabel* label1=new QLabel("Статистика по",this);
        label=new QLabel("Средний результат",this);
        person2.date=new QComboBox(this);
        person2.date->addItems({"зп репетиторов","количеству занятий у репетиторов","количеству занятий у клиентов","популярности предметов у репетиторов/клиентов","популярности регионов у репетиторов/клиентов","тратам клиентов"});
        connect(person2.date,SIGNAL(activated(int)),SLOT(slotStatistics(int)));
        QHBoxLayout* lay=new QHBoxLayout();
        lay->addWidget(label1);
        lay->addWidget(person2.date);
        lay->addStretch(1);
        QVBoxLayout* lay2=new QVBoxLayout();
        lay2->addLayout(lay);
        lay2->addWidget(label);
        lay2->addWidget(tableView);
        QWidget* wid=new QWidget(this);
        wid->setLayout(lay2);

        setCentralWidget(wid);
        emit person2.date->activated(0);
    }
    if(action->text()==tr("профиль администратора")){
        QFile file("C:/Users/lesko/Desktop/C++/DB3-jul/lr3/files/admin.txt");
        if(!file.open(QIODevice::ReadOnly)){
            getMessageBox("Файл с данными администратора не открылся",true);
            return;
        }
        QString str;
        str=file.readAll();

        QFont font1;
        font1.setPixelSize(20);
        QFont font2;
        font2.setUnderline(true);
        font2.setPixelSize(30);
        QFont font3(font1);
        font3.setBold(true);
        QFont font4(font2);
        font4.setBold(true);

        QLabel* admin=new QLabel("Администратора");
        admin->setFont(font4);
        QLabel* infa=new QLabel("ФИО: "+str.section(",",1,1)
                                +"\nНомер телефона: "+str.section(",",0,0)
                                +"\nПароль: "+str.section(",",2,2),this);
        infa->setFont(font3);

        QSqlQuery query1(db);
        if(!query1.exec("SELECT COUNT(*) FROM az_tutors;")){
            getMessageBox("Не открылась таблица с репетиторами",true);
            return;
        }
        query1.first();
        int tutors=query1.value(0).toInt();
        if(!query1.exec("SELECT COUNT(*) FROM az_clients;")){
            getMessageBox("Не открылась таблица с клиентами",true);
            return;
        }
        query1.first();
        int clients=query1.value(0).toInt();

        double profit=0;
        if(!query1.exec("SELECT Цена FROM az_lessons WHERE Было_занятие=true;")){
            getMessageBox("Не открылась таблица с занятиями",true);
            return;
        }
        query1.first();
        do{
            profit+=0.3*(query1.value(0).toInt());
        }while(query1.next());

        QLabel* statistics=new QLabel("Основная статистика");
        statistics->setFont(font2);
        QLabel* stat=new QLabel("Количество репетиторов: "+QString::number(tutors)+"\nКоличество клиентов: "+QString::number(clients)+"\nОбщая сумма дохода: "+QString::number(profit)+"руб.",this);
        stat->setFont(font1);

        QVBoxLayout* layout1=new QVBoxLayout();
        layout1->addWidget(admin);
        layout1->addWidget(infa);
        layout1->addStretch(1);

        QVBoxLayout* layout2=new QVBoxLayout();
        layout2->addWidget(statistics);
        layout2->addWidget(stat);
        layout2->addStretch(1);

        QHBoxLayout* layout3=new QHBoxLayout();
        layout3->addLayout(layout1);
        layout3->addLayout(layout2);

        QWidget* widget=new QWidget(this);
        widget->setLayout(layout3);
        setCentralWidget(widget);
    }
    if(action->text()==tr("настройки профиля администратора")){
        QFile file("C:/Users/lesko/Desktop/C++/DB3-jul/lr3/files/admin.txt");
        if(!file.open(QIODevice::ReadOnly)) {
            getMessageBox("Файл с данными администратора не открылся",true);
            return;
        }
        QString str;
        str=file.readAll();
        person.telephone=new QLineEdit(this);
        person.telephone->setText(str.section(',',0,0));
        person.telephone->setInputMask("99999999999");
        person.name=new QLineEdit(this);
        person.name->setText(str.section(',',1,1));
        person.password=new QLineEdit(this);
        person.password->setText(str.section(',',2,2));
        QPushButton* button=new QPushButton("Изменить",this);
        QVBoxLayout* layout=new QVBoxLayout();
        layout->addWidget(person.telephone);
        layout->addWidget(person.name);
        layout->addWidget(person.password);
        layout->addWidget(button);
        layout->addStretch(1);
        QWidget* w=new QWidget(this);
        w->setLayout(layout);
        setCentralWidget(w);
        connect(button,SIGNAL(pressed()),this,SLOT(changeInfoAdmin()));
    }
    if(action->text()==tr("выйти")){
        delete centralWidget();
        tableView=new QTableView();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        setWindowIcon(QIcon(":/MyPictures/pictures/mainIcon.jpg"));
        setWindowTitle(tr("Главная страница"));

        QPixmap bkgnd(":/MyPictures/pictures/mainPicture.jfif");
        bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatio);
        setFixedSize(QSize(bkgnd.width(),bkgnd.height()));
        palette->setBrush(QPalette::Window, bkgnd);
        setPalette(*palette);

        changeMenuBar({{tr("&Войти")},{tr("&Репетиторы")}});
    }



    //меню репетитора
    if(action->text()==tr("профиль репетитора")){

        QFont font1;
        font1.setPixelSize(20);
        QFont font2;
        font2.setUnderline(true);
        font2.setPixelSize(30);
        QFont font3(font1);
        font3.setBold(true);
        QFont font4(font2);
        font4.setBold(true);

        QSqlQuery q(db);
        if(!q.exec("SELECT * FROM az_tutors WHERE Телефон='"+property("telephone").toString()+"';")){
            getMessageBox("Таблица с репетиторами не открывается",true);
            return;
         }
        q.first();
        QLabel* admin=new QLabel("Репетитор");
        admin->setFont(font4);
        QLabel* infa=new QLabel("Номер телефона: "+q.value(0).toString()
                                +"\nФИО: "+q.value(1).toString()
                                +"\nПредмет: "+q.value(2).toString()
                                +"\nРегион: "+q.value(3).toString()
                                +"\nЦена: "+q.value(4).toString()
                                +"\nПароль: "+q.value(5).toString(),this);
        infa->setFont(font3);
        setProperty("price",q.value(4).toString());

        QSqlQuery query1(db);
        if(!query1.exec("SELECT COUNT(*) FROM az_lessons WHERE Телефон_репетитора='"+property("telephone").toString()+"' AND Было_занятие=true;")){
            getMessageBox("Не открылась таблица с занятиями",true);
            return;
        }
        query1.first();
        int lessons=query1.value(0).toInt();

        if(!query1.exec("SELECT Цена FROM az_lessons WHERE Телефон_репетитора='"+property("telephone").toString()+"' AND Было_занятие=true;")){
            getMessageBox("Не открылась таблица с занятиями",true);
            return;
        }
        query1.first();
        double profit=0;
        do{
            profit+=0.7*(query1.value(0).toInt());
        }while(query1.next());
        QLabel* statistics=new QLabel("Основная статистика");
        statistics->setFont(font2);
        QLabel* stat=new QLabel("Количество проведённых занятий: "+QString::number(lessons)+"\nОбщая прибыль: "+QString::number(profit)+"руб.",this);
        stat->setFont(font1);

        QVBoxLayout* layout1=new QVBoxLayout();
        layout1->addWidget(admin);
        layout1->addWidget(infa);
        layout1->addStretch(1);

        QVBoxLayout* layout2=new QVBoxLayout();
        layout2->addWidget(statistics);
        layout2->addWidget(stat);
        layout2->addStretch(1);

        QHBoxLayout* layout3=new QHBoxLayout();
        layout3->addLayout(layout1);
        layout3->addLayout(layout2);

        delete centralWidget();
        QWidget* widget=new QWidget(this);
        widget->setLayout(layout3);
        setCentralWidget(widget);
    }
    if(action->text()==tr("настройки профиля репетитора")){
        QSqlQuery q(db);
        if(!q.exec("SELECT * FROM az_tutors WHERE Телефон='"+property("telephone").toString()+"';")){
            getMessageBox("Таблица с репетиторами не открывается",true);
            return;
        }
        q.first();
        person.telephone=new QLineEdit(this);
        person.telephone->setText(q.value(0).toString());
        person.telephone->setInputMask("99999999999");
        person.name=new QLineEdit(this);
        person.name->setText(q.value(1).toString());
        person.price=new QLineEdit(this);
        person.price->setText(q.value(4).toString());
        QIntValidator* val=new QIntValidator(1,5000,this);
        person.price->setValidator(val);
        person.password=new QLineEdit(this);
        person.password->setText(q.value(5).toString());
        person.region=new QComboBox(this);
        if(!q.exec("SELECT * FROM az_regions;")){
            getMessageBox("Таблица с регионами не открывается",true);
            return;
         }
        do{
            QString str=q.value(0).toString();
            person.region->addItem(str);
        }while(q.next());
        person.region->setCurrentText(q.value(3).toString());
        person.region->removeItem(0);


        QPushButton* button=new QPushButton("Изменить",this);
        QVBoxLayout* layout=new QVBoxLayout();
        layout->addWidget(person.telephone);
        layout->addWidget(person.name);
        layout->addWidget(person.region);
        layout->addWidget(person.price);
        layout->addWidget(person.password);
        layout->addWidget(button);
        layout->addStretch(1);
        QWidget* w=new QWidget(this);
        w->setLayout(layout);
        setCentralWidget(w);
        connect(button,SIGNAL(pressed()),this,SLOT(changeInfoTutor()));
    }
    if(action->text()==tr("Добавить/удалить занятие")){
        QSqlQuery query(db);
        if(!query.exec("SELECT Цена,День,Время_начала,Телефон_клиента FROM az_lessons WHERE Телефон_репетитора='"+property("telephone").toString()+"' AND Было_занятие=false;")){
            getMessageBox("Таблица с занятиями не открывается",true);
            return;
         }
        delete centralWidget();
        tableView=new QTableView();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        model->setQuery(query);
        tableView->setModel(model);
        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotDeleteLesson(QPoint)));

        QPushButton* add=new QPushButton("&Добавить",this);
        lineEdit=new QLineEdit(this);
        lineEdit->setPlaceholderText("Введите дату(ДД.MM.ГГГГ)");
        QRegularExpression regExp("[0-9]{2}[.][0-9]{2}[.][0-9]{4}");
        lineEdit->setValidator(new QRegularExpressionValidator(regExp,this));
        lineEdit2=new QLineEdit(this);
        lineEdit2->setPlaceholderText("Введите время(ЧЧ:MM)");
        QRegularExpression regExp1("[0-2]{1}[0-9]{1}:[0-9]{2}");
        lineEdit2->setValidator(new QRegularExpressionValidator(regExp1,this));
        QVBoxLayout* layout1=new QVBoxLayout();
        layout1->addWidget(lineEdit);
        layout1->addWidget(lineEdit2);
        layout1->addWidget(add);
        layout1->addStretch(1);
        QHBoxLayout* layout2=new QHBoxLayout();
        layout2->addWidget(tableView);
        layout2->addLayout(layout1);
        QWidget* widget=new QWidget(this);
        widget->setLayout(layout2);
        setCentralWidget(widget);

        connect(add,SIGNAL(pressed()),this,SLOT(slotAddLesson()));
    }
    if(action->text()==tr("Прошедшие занятия")){
        QSqlQuery query(db);
        if(!query.exec("SELECT Цена,День,Время_начала,Телефон_клиента FROM az_lessons WHERE Телефон_репетитора='"+property("telephone").toString()+"' AND Было_занятие=true;")){
            getMessageBox("Не удалось открыть таблицу занятий",true);
        }
        delete centralWidget();
        tableView=new QTableView();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        model->setQuery(query);
        tableView->setModel(model);
        setCentralWidget(tableView);
    }


    //меню клиента
    if(action->text()==tr("профиль клиента")){
        QFont font1;
        font1.setPixelSize(20);
        QFont font2;
        font2.setUnderline(true);
        font2.setPixelSize(30);
        QFont font3(font1);
        font3.setBold(true);
        QFont font4(font2);
        font4.setBold(true);

        QSqlQuery q(db);
        if(!q.exec("SELECT * FROM az_clients WHERE Телефон='"+property("telephone").toString()+"';")){
            getMessageBox("Таблица с клиентами не открывается",true);
            return;
         }
        q.first();
        QLabel* client=new QLabel("Клиент");
        client->setFont(font4);
        QLabel* infa=new QLabel("Номер телефона: "+q.value(0).toString()
                                +"\nФИО: "+q.value(1).toString()
                                +"\nПароль: "+q.value(2).toString(),this);
        infa->setFont(font3);

        QSqlQuery query1(db);
        if(!query1.exec("SELECT COUNT(*) FROM az_lessons WHERE Телефон_клиента='"+property("telephone").toString()+"' AND Было_занятие=true;")){
            getMessageBox("Не открылась таблица с занятиями",true);
            return;
        }
        query1.first();
        int lessons=query1.value(0).toInt();

        if(!query1.exec("SELECT Цена FROM az_lessons WHERE Телефон_клиента='"+property("telephone").toString()+"' AND Было_занятие=true;")){
            getMessageBox("Не открылась таблица с занятиями",true);
            return;
        }
        query1.first();
        int profit=0;
        do{
            profit=profit+(query1.value(0).toInt());
        }while(query1.next());
        QLabel* statistics=new QLabel("Основная статистика");
        statistics->setFont(font2);
        QLabel* stat=new QLabel("Количество посещённых занятий: "+QString::number(lessons)+"\nОбщая сумма затрат: "+QString::number(profit)+"руб.",this);
        stat->setFont(font1);

        QVBoxLayout* layout1=new QVBoxLayout();
        layout1->addWidget(client);
        layout1->addWidget(infa);
        layout1->addStretch(1);

        QVBoxLayout* layout2=new QVBoxLayout();
        layout2->addWidget(statistics);
        layout2->addWidget(stat);
        layout2->addStretch(1);

        QHBoxLayout* layout3=new QHBoxLayout();
        layout3->addLayout(layout1);
        layout3->addLayout(layout2);

        QWidget* widget=new QWidget(this);
        widget->setLayout(layout3);
        setCentralWidget(widget);
    }
    if(action->text()==tr("настройки профиля клиента")){
        QSqlQuery q(db);
        if(!q.exec("SELECT * FROM az_clients WHERE Телефон='"+property("telephone").toString()+"';")){
            getMessageBox("Таблица с клиентами не открывается",true);
            return;
         }
        q.first();
        person.telephone=new QLineEdit(this);
        person.telephone->setText(q.value(0).toString());
        person.telephone->setInputMask("99999999999");
        person.name=new QLineEdit(this);
        person.name->setText(q.value(1).toString());
        person.password=new QLineEdit(this);
        person.password->setText(q.value(2).toString());

        QPushButton* button=new QPushButton("Изменить",this);
        QVBoxLayout* layout=new QVBoxLayout();
        layout->addWidget(person.telephone);
        layout->addWidget(person.name);
        layout->addWidget(person.password);
        layout->addWidget(button);
        layout->addStretch(1);
        QWidget* w=new QWidget(this);
        w->setLayout(layout);
        setCentralWidget(w);
        connect(button,SIGNAL(pressed()),this,SLOT(changeInfoClient()));
    }
    if(action->text()==tr("выбрать занятие")){
        QSqlQuery q(db);
        if(!q.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента is NULL;")){
            getMessageBox("Таблица не открывается",true);
            qDebug()<<q.lastError();
            return;
         }
        delete centralWidget();
        tableView=new QTableView();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        model->setQuery(q);
        tableView->setModel(model);
        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotChooseDiscipline(QPoint)));

        person2.discipline=new QComboBox(this);
        person2.discipline->addItem("<Дисциплина>");
        q.first();
        QStringList list;
        do{
            list<<q.value(2).toString();
        }while(q.next());
        list.removeDuplicates();
        for(auto a:list){
            person2.discipline->addItem(a);
        }
        QVBoxLayout* layout=new QVBoxLayout();
        layout->addWidget(tableView);
        layout->addWidget(person2.discipline);
        QWidget* w=new QWidget(this);
        w->setLayout(layout);
        setCentralWidget(w);
        connect(person2.discipline,SIGNAL(currentTextChanged(const QString)),SLOT(currentTextChanged1(const QString)));
    }
    if(action->text()==tr("выбранные занятия")){
        QSqlQuery query(db);
        if(!query.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента='"+property("telephone").toString()+"' AND az_lessons.Было_занятие=false;")){
            getMessageBox("Таблица не открывается",true);
            return;
         }
        delete centralWidget();
        tableView=new QTableView();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        model->setQuery(query);
        tableView->setModel(model);
        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotChangeLesson(QPoint)));
        setCentralWidget(tableView);
    }
    if(action->text()==tr("прошедшие занятия")){
        QSqlQuery query(db);
        if(!query.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента='"+property("telephone").toString()+"' AND az_lessons.Было_занятие=true;")){
            qDebug()<<query.lastError();
            getMessageBox("Не удалось открыть таблицу занятий",true);
            return;
        }
        delete centralWidget();
        tableView=new QTableView();
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        model->setQuery(query);
        tableView->setModel(model);
        setCentralWidget(tableView);
    }
}

