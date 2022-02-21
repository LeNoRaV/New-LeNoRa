#include "mainwindow.h"

void MainWindow::getMessageBox(const QString textError,bool error){
    QMessageBox msgBox;
    if(error) msgBox.setIcon(QMessageBox::Critical);
    else msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(textError);
    msgBox.exec();
}

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
//    QPixmap bkgnd(":/MyPictures/pictures/mainPicture.jfif");
    QLabel label;
    QImage gif("C:/Users/lesko/Pictures/1b0b2ee4087d68e9ae5e0e28b136b2e3.jpg");
    QBrush brush(gif);
//    setFixedSize(QSize(bkgnd.width(),bkgnd.height()));
//    setFixedSize(QSize(gif.width(),gif.height()));
    label.setAlignment(Qt::AlignHCenter);
    palette=new QPalette();
//    palette->setBrush(QPalette::Window, bkgnd);
    palette->setBrush(QPalette::Window, brush);
    setPalette(*palette);

    setFixedSize(QSize(550,500));

    menuBar=new QMenuBar(this);
    changeMenuBar({{tr("&Войти")},{tr("&Участницы")}});

    setMenuBar(menuBar);
    connect(menuBar,SIGNAL(triggered(QAction*)),this,SLOT(slotTriggeredMenuBar(QAction*)));

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


void MainWindow::slotTriggeredMenuBar(QAction* action){

    if(action->text()==tr("&Войти")){
        QString whoAndPassword;
        MyDialogEnter(&db,&whoAndPassword).exec();
        if(whoAndPassword.section(",",0,0)=="админ"){
            adminProfile();
        }
        if(whoAndPassword.section(",",0,0)=="жюри"){
            tutorProfile(whoAndPassword.section(",",1,1));
        }
        if(whoAndPassword.section(",",0,0)=="участница"){
            clientProfile(whoAndPassword.section(",",1,1));
        }
    }
    if(action->text()==tr("&Участницы")){
        setWindowTitle(tr("Участницы"));
        QSqlQuery query (db);
        if(!query.exec("SELECT Номер_зачётки, ФИО, Вес, Рост, Возраст, Талант, кол_во_номинаций, Общие_баллы FROM lnr_participants ORDER BY Общие_баллы DESC;")){
            getMessageBox("Таблица с участницами не открылась",true);
            return;
        }
        model->setQuery(query);
        tableView->setModel(model);
        setCentralWidget(tableView);
        changeMenuBar({{tr("&Войти")}});
    }

    //меню админа
    if(action->text()==tr("удалить участницу")){
        QSqlQuery query(db);
        if(!query.exec("SELECT * FROM lnr_participants;")){
            getMessageBox("Таблица с участницами не открылась",true);
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

    if(action->text()==tr("добавить или удалить номинацию")){
        QSqlQuery query(db);
        if(!query.exec("SELECT * FROM lnr_nominations;")){
            getMessageBox("Таблица с номинациями не открылась",true);
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
        lineEdit->setPlaceholderText("Введите номинацию");
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

    if(action->text()==tr("топ")){
        QLabel* label1=new QLabel("Рейтинг по",this);
        person2.date=new QComboBox(this);
        person2.date->addItems({"всем участницам","номинациям"});
        connect(person2.date,SIGNAL(activated(int)),SLOT(slotStatistics(int)));
        QHBoxLayout* lay=new QHBoxLayout();
        lay->addWidget(label1);
        lay->addWidget(person2.date);
        lay->addStretch(1);
        QVBoxLayout* lay2=new QVBoxLayout();
        lay2->addLayout(lay);
        lay2->addWidget(tableView);
        QWidget* wid=new QWidget(this);
        wid->setLayout(lay2);

        setCentralWidget(wid);
        emit person2.date->activated(0);
    }
    if(action->text()==tr("профиль администратора")){
        QFile file("C:/Users/lesko/Desktop/C++/DB3-jul/New-LeNoRa/files/admin.txt");
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

        QLabel* admin=new QLabel("Администратор");
        admin->setFont(font4);
        QLabel* infa=new QLabel("ФИО: "+str.section(",",1,1)   //interface
                                +"\nЛогин: "+str.section(",",0,0)
                                +"\nПароль: "+str.section(",",2,2),this);
        infa->setFont(font3);

        QSqlQuery query1(db);
        if(!query1.exec("SELECT COUNT(*) FROM lnr_participants;")){
            getMessageBox("Не открылась таблица с участницами",true);
            return;
        }
        query1.first();
        int tutors=query1.value(0).toInt();

        if(!query1.exec("SELECT lnr_participants.Номер_зачётки, lnr_participants.ФИО FROM lnr_participants, lnr_competition WHERE lnr_participants.Номер_зачётки = lnr_competition.Участница LIMIT 3;")){
            getMessageBox("Не открылась таблица с участницами",true);
            return;
        }
//        query1.first();
//        int tutors=query1.value(0).toInt();

//        if(!query1.exec("SELECT COUNT(*) FROM az_clients;")){
//            getMessageBox("Не открылась таблица с клиентами",true);
//            return;
//        }
//        query1.first();
//        int clients=query1.value(0).toInt();

//        double profit=0;
        if(!query1.exec("SELECT COUNT(lnr_competition.Баллы) FROM lnr_participants, lnr_competition WHERE lnr_participants.Номер_зачётки = lnr_competition.Участница LIMIT 3;")){
            getMessageBox("Не открылась таблица с соревнованием",true);
            return;
        }
//        query1.first();
//        do{
//            profit+=0.3*(query1.value(0).toInt());
//        }while(query1.next());

        QLabel* statistics=new QLabel("Основная статистика");
        statistics->setFont(font2);
        QLabel* stat=new QLabel("Количество участниц: "+QString::number(tutors)+"\n1 место: "+QString::number(tutors)+"\n2 место: "+QString::number(tutors)+"\n3 место: "+QString::number(tutors),this);
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
        QFile file("C:/Users/lesko/Desktop/C++/DB3-jul/New-LeNoRa/files/admin.txt");
        if(!file.open(QIODevice::ReadOnly)) {
            getMessageBox("Файл с данными администратора не открылся",true);
            return;
        }
        QString str;
        str=file.readAll();
        person.login=new QLineEdit(this);
        person.login->setText(str.section(',',0,0));
//        person.login->setInputMask("99999999999");
        person.name=new QLineEdit(this);
        person.name->setText(str.section(',',1,1));
        person.password=new QLineEdit(this);
        person.password->setText(str.section(',',2,2));
        QPushButton* button=new QPushButton("Изменить",this);
        QVBoxLayout* layout=new QVBoxLayout();
        layout->addWidget(person.login);
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

        QPixmap bkgnd(":/MyPictures/pictures/mainPicture.jfif");          //!!!!!
        bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatio);
        setFixedSize(QSize(bkgnd.width(),bkgnd.height()));
        palette->setBrush(QPalette::Window, bkgnd);
        setPalette(*palette);

        changeMenuBar({{tr("&Войти")},{tr("&Участницы")}});
    }



    //меню участницы
    if(action->text()==tr("профиль участницы")){

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
        if(!q.exec("SELECT * FROM lnr_participants WHERE Номер_зачётки='"+property("record_book").toString()+"';")){  // по зачетке
            getMessageBox("Таблица с участницами не открывается",true);
            return;
         }
        q.first();
        QLabel* admin=new QLabel("Участница");
        admin->setFont(font4);
        QLabel* infa=new QLabel("Номер зачётки: "+q.value(0).toString()
                                +"\nФИО: "+q.value(1).toString()
                                +"\nВес: "+q.value(2).toString()
                                +"\nРост: "+q.value(3).toString()
                                +"\nВозраст: "+q.value(4).toString()
                                +"\nТалант: "+q.value(5).toString()
                                +"\nКол-во номинаций: "+q.value(7).toString(),this);
        infa->setFont(font3);
        setProperty("price",q.value(4).toString());      // !!!

        QSqlQuery query1(db);

        int cnt_nomin=0;
        if(!query1.exec("SELECT COUNT(*) FROM lnr_competition WHERE Участница='"+property("record_book").toString()+"';")){
            getMessageBox("Нет участия ни в одном соревновании",true);

//            return;
        }
        query1.first();
        cnt_nomin=query1.value(0).toInt();

        if(!query1.exec("SELECT lnr_nominations.Название_номинации, lnr_competition.Баллы FROM lnr_competition, lnr_nominations WHERE lnr_competition.Участница='"+property("record_book").toString()+"' AND lnr_nominations.\"ID\" = lnr_competition.Номинация;")){
            getMessageBox("Не открылась таблица со соревнованием",true);
            return;
        }
        query1.first();
        QString nomin1="";
        QString nomin2="";
        QString nomin3="";
        QString score1="";
        QString score2="";
        QString score3="";
        nomin1=query1.value(0).toString();
        score1=query1.value(1).toString();
        query1.next();
        nomin2=query1.value(0).toString();
        score2=query1.value(1).toString();
        query1.next();
        nomin3=query1.value(0).toString();
        score3=query1.value(1).toString();


        QLabel* statistics=new QLabel("Основная статистика");
        statistics->setFont(font2);
        QLabel* stat=new QLabel("1) "+QString(nomin1)+"   "+QString(score1)+"\n2) "+QString(nomin2)+"   "+QString(score2)+
                                "\n3) "+QString(nomin3)+"   "+QString(score3),this);
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
    if(action->text()==tr("настройки профиля участницы")){
        QSqlQuery q(db);
        if(!q.exec("SELECT * FROM lnr_participants WHERE Номер_зачётки='"+property("record_book").toString()+"';")){
            getMessageBox("Таблица с участницами не открывается",true);
            return;
        }
        q.first();
        person.login=new QLineEdit(this);
        person.login->setText(q.value(0).toString());
        person.name=new QLineEdit(this);
        person.name->setText(q.value(1).toString());
        person.weight=new QLineEdit(this);
        person.weight->setText(q.value(2).toString());
        person.height=new QLineEdit(this);
        person.height->setText(q.value(3).toString());
        person.age=new QLineEdit(this);
        person.age->setText(q.value(4).toString());
        person.talant=new QLineEdit(this);
        person.talant->setText(q.value(5).toString());
        person.password=new QLineEdit(this);
        person.password->setText(q.value(6).toString());
//        person.region=new QComboBox(this);
//        if(!q.exec("SELECT * FROM az_regions;")){
//            getMessageBox("Таблица с регионами не открывается",true);
//            return;
//         }
//        do{
//            QString str=q.value(0).toString();
//            person.region->addItem(str);
//        }while(q.next());
//        person.region->setCurrentText(q.value(3).toString());
//        person.region->removeItem(0);


        QPushButton* button=new QPushButton("Изменить",this);
        QVBoxLayout* layout=new QVBoxLayout();
        layout->addWidget(person.login);
        layout->addWidget(person.name);
        layout->addWidget(person.weight);
        layout->addWidget(person.height);
        layout->addWidget(person.age);
        layout->addWidget(person.talant);
        layout->addWidget(person.password);
        layout->addWidget(button);
        layout->addStretch(1);
        QWidget* w=new QWidget(this);
        w->setLayout(layout);
        setCentralWidget(w);
        connect(button,SIGNAL(pressed()),this,SLOT(changeInfoClient()));
    }
    if(action->text()==tr("посмотреть свои номинации")){
        QSqlQuery query(db);
        if(!query.exec("SELECT lnr_nominations.Название_номинации, lnr_competition.Баллы FROM lnr_nominations, lnr_competition WHERE lnr_competition.Номинация = lnr_nominations.\"ID\" AND lnr_competition.Участница = '"+property("record_book").toString()+"';")){
            getMessageBox("Таблица с номинациями не открывается",true);
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



    if(action->text()==tr("профиль жюри")){
        QFile file("C:/Users/lesko/Desktop/C++/DB3-jul/New-LeNoRa/files/jury.txt");
        if(!file.open(QIODevice::ReadOnly)){
            getMessageBox("Файл с данными жюри не открылся",true);
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

        QLabel* admin=new QLabel("Жюри");
        admin->setFont(font4);
        QLabel* infa=new QLabel("ФИО: "+str.section(",",1,1)   //interface
                                +"\nЛогин: "+str.section(",",0,0)
                                +"\nПароль: "+str.section(",",2,2),this);
        infa->setFont(font3);

        QSqlQuery query1(db);
        if(!query1.exec("SELECT COUNT(*) FROM lnr_participants;")){
            getMessageBox("Не открылась таблица с участницами",true);
            return;
        }
        query1.first();
        int tutors=query1.value(0).toInt();

        if(!query1.exec("SELECT lnr_participants.Номер_зачётки, lnr_participants.ФИО FROM lnr_participants, lnr_competition WHERE lnr_participants.Номер_зачётки = lnr_competition.Участница LIMIT 3;")){
            getMessageBox("Не открылась таблица с участницами",true);
            return;
        }
        if(!query1.exec("SELECT COUNT(lnr_competition.Баллы) FROM lnr_participants, lnr_competition WHERE lnr_participants.Номер_зачётки = lnr_competition.Участница LIMIT 3;")){
            getMessageBox("Не открылась таблица с соревнованием",true);
            return;
        }

        QLabel* statistics=new QLabel("Основная статистика");
        statistics->setFont(font2);
        QLabel* stat=new QLabel("Количество участниц: "+QString::number(tutors)+"\n1 место: "+QString::number(tutors)+"\n2 место: "+QString::number(tutors)+"\n3 место: "+QString::number(tutors),this);
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
    if(action->text()==tr("настройки профиля жюри")){
        QFile file("C:/Users/lesko/Desktop/C++/DB3-jul/New-LeNoRa/files/jury.txt");
        if(!file.open(QIODevice::ReadOnly)) {
            getMessageBox("Файл с данными жюри не открылся",true);
            return;
        }
        QString str;
        str=file.readAll();
        person.login=new QLineEdit(this);
        person.login->setText(str.section(',',0,0));
        person.name=new QLineEdit(this);
        person.name->setText(str.section(',',1,1));
        person.password=new QLineEdit(this);
        person.password->setText(str.section(',',2,2));
        QPushButton* button=new QPushButton("Изменить",this);
        QVBoxLayout* layout=new QVBoxLayout();
        layout->addWidget(person.login);
        layout->addWidget(person.name);
        layout->addWidget(person.password);
        layout->addWidget(button);
        layout->addStretch(1);
        QWidget* w=new QWidget(this);
        w->setLayout(layout);
        setCentralWidget(w);
        connect(button,SIGNAL(pressed()),this,SLOT(changeInfoTutor()));

    }
    if(action->text()==tr("Мисс Бауман 2022")){
        QSqlQuery q(db);
        if(!q.exec("SELECT Название_номинации FROM lnr_nominations;")){
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
        connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotChooseDiscipline(QPoint)));  //!!!

        person2.discipline=new QComboBox(this);
        person2.discipline->addItem("<Номинация>");
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
//    if(action->text()==tr("выбранные занятия")){
//        QSqlQuery query(db);
//        if(!query.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента='"+property("telephone").toString()+"' AND az_lessons.Было_занятие=false;")){
//            getMessageBox("Таблица не открывается",true);
//            return;
//         }
//        delete centralWidget();
//        tableView=new QTableView();
//        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//        model->setQuery(query);
//        tableView->setModel(model);
//        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
//        connect(tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotChangeLesson(QPoint)));
//        setCentralWidget(tableView);
//    }
//    if(action->text()==tr("прошедшие занятия")){
//        QSqlQuery query(db);
//        if(!query.exec("SELECT az_lessons.Телефон_репетитора,az_tutors.ФИО,az_tutors.Дисциплина,az_tutors.Регион,az_lessons.Цена,az_lessons.День,az_lessons.Время_начала FROM az_lessons INNER JOIN az_tutors ON az_lessons.Телефон_репетитора=az_tutors.Телефон WHERE az_lessons.Телефон_клиента='"+property("telephone").toString()+"' AND az_lessons.Было_занятие=true;")){
//            qDebug()<<query.lastError();
//            getMessageBox("Не удалось открыть таблицу занятий",true);
//            return;
//        }
//        delete centralWidget();
//        tableView=new QTableView();
//        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//        model->setQuery(query);
//        tableView->setModel(model);
//        setCentralWidget(tableView);
//    }
}

