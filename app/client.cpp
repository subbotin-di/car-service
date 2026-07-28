#include "client.h"
#include "ui_client.h"

#include <QMessageBox>

Client::Client(QWidget *parent, QString id) :
    QWidget(parent),
    idClient(id),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    setWindowTitle("Клиент id=" + idClient);
    dataBase = QSqlDatabase::addDatabase("QODBC");  // создание нового подключения с использованием драйвера QtSql ODBC
    dataBase.setDatabaseName(QString("DRIVER={SQL Server};Server=DESKTOP-944V2PH;Database=car_service;Trusted_Connection=yes;")); // установка параметров подключения
    if (!dataBase.open())   // установить подключение
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть базу данных");
        close();
    }   

    QSqlTableModel *sparesView = new QSqlTableModel(this, dataBase);
    sparesView->setTable("Spares");
    ui->sparesView_2->setModel(sparesView);
    sparesView->select();
    ui->sparesView_2->resizeColumnsToContents();

    QSqlTableModel *servicesView = new QSqlTableModel(this, dataBase);
    servicesView->setTable("Services");
    ui->servicesView_2->setModel(servicesView);
    servicesView->select();
    ui->servicesView_2->resizeColumnsToContents();

    QSqlQueryModel *queryOrdersView = new QSqlQueryModel;
    QString textQueryOrdersView= "SELECT Orders.id, "
            "Services.name, "
            "Orders.id_car, "
            "Orders.id_manager, "
            "Orders.id_master, "
            "Orders.creation_date, "
            "Orders.expiration_date, "
            "Orders.status "
            "FROM Clients "
            "JOIN Cars "
            "ON Clients.id = Cars.id_owner "
            "JOIN Orders "
            "ON Cars.id = Orders.id_car "
            "JOIN Order_List "
            "ON Orders.id = Order_List.id_order "
            "JOIN Services "
            "ON Order_List.id_service = Services.id "
            "WHERE Clients.id = '" + idClient + "'";
    queryOrdersView->setQuery(textQueryOrdersView);
    QTableView *ordersView = new QTableView();
    ordersView->setModel(queryOrdersView);
    ui->ordersView_2->setModel(queryOrdersView);
    ui->ordersView_2->resizeColumnsToContents();

    QSqlQueryModel *queryCarsView = new QSqlQueryModel;
    QString textQueryCarsView= "SELECT Cars.number, "
            "Cars.brand, "
            "Cars.model, "
            "Cars.color "
            "FROM Clients "
            "JOIN Cars ON Clients.id = Cars.id_owner "
            "WHERE Clients.id = '" + idClient + "'";
    queryCarsView->setQuery(textQueryCarsView);
    QTableView *carsView = new QTableView();
    carsView->setModel(queryCarsView);
    ui->carsView_2->setModel(queryCarsView);
    ui->carsView_2->resizeColumnsToContents();
}

Client::~Client()
{
    delete ui;
}
