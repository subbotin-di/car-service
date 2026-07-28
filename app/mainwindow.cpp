#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "owner.h"
#include "master.h"
#include "manager.h"
#include "client.h"

#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Авторизация");
    dataBase = QSqlDatabase::addDatabase("QODBC");  // создание нового подключения с использованием драйвера QtSql ODBC
    dataBase.setDatabaseName(QString("DRIVER={SQL Server};Server=DESKTOP-944V2PH;Database=car_service;Trusted_Connection=yes;")); // установка параметров подключения
    if (!dataBase.open())   // установить подключение
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть базу данных");
        close();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QSqlQuery MainWindow::select(QString& textOfQuery)
{
    QSqlQuery query;
    if(!query.exec(textOfQuery))
    {
        QMessageBox::critical(this, "Ошибка запроса", "Не удалось выбрать данные из базы данных");
        close();
    }
    else
    {
        return query;
    }
}

void MainWindow::on_pushButton_clicked()
{
    //проверка логина и пароля
    QString login = ui->login->text();
    QString password = ui->password->text();
    QString textOfQueryAutorisation = "OPEN SYMMETRIC KEY symmetric_key "
            "DECRYPTION BY CERTIFICATE certificate; "
            "SELECT id FROM Login_Details WHERE login = '" + login + "' "
            "AND convert(char,DecryptByKey(password)) = '" + password + "'";
    QSqlQuery queryAutorisation = select(textOfQueryAutorisation);
    if(!queryAutorisation.first())
    {
        QMessageBox::warning(this, "Ошибка авторизации", "Проверьте правильность логина и пароля");
    }
    else
    {
        QString idLoginDetails = queryAutorisation.value(0).toString();
        //владелец
        QString textOfQueryOwner = "SELECT * FROM Owner WHERE id_login_details = '" + idLoginDetails + "'";
        QSqlQuery queryOwner = select(textOfQueryOwner);
        if (queryOwner.first())
        {
            this->close();
            Owner* ownerWindow = new Owner();
            ownerWindow->show();
        }
        //клиент
        QString textOfQueryClient = "SELECT * FROM Clients WHERE id_login_details = '" + idLoginDetails + "'";
        QSqlQuery queryClient = select(textOfQueryClient);
        if (queryClient.first())
        {
            QString id = queryClient.value(0).toString();
            this->close();
            Client* clientWindow = new Client(nullptr, id);
            clientWindow->show();
        }

        //работник
        QString textOfQueryWorker = "SELECT * FROM Workers WHERE id_login_details = '" + idLoginDetails + "'";
        QSqlQuery queryWorker = select(textOfQueryWorker);
        if (queryWorker.first())
        {
            QString id = queryWorker.value(0).toString();
            QString idPosition = queryWorker.value(1).toString();
            //мастер
            if (idPosition == "1")
            {
                this->close();
                Master* masterWindow = new Master(nullptr, id);
                masterWindow->show();
            }
            //менеджер
            if (idPosition == "2")
            {
                this->close();
                Manager* managerWindow = new Manager(nullptr, id);
                managerWindow->show();
            }
        }
    }
}
