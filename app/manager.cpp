#include "manager.h"
#include "ui_manager.h"

#include <QMessageBox>
#include <QDate>

Manager::Manager(QWidget *parent, QString id) :
    QWidget(parent),
    idManager(id),
    ui(new Ui::Manager)
{
    ui->setupUi(this);
    setWindowTitle("Менеджер id=" + idManager);
    dataBase = QSqlDatabase::addDatabase("QODBC");  // создание нового подключения с использованием драйвера QtSql ODBC
    dataBase.setDatabaseName(QString("DRIVER={SQL Server};Server=DESKTOP-944V2PH;Database=car_service;Trusted_Connection=yes;")); // установка параметров подключения
    if (!dataBase.open())   // установить подключение
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть базу данных");
        close();
    }

    QSqlQueryModel *queryOrdersView = new QSqlQueryModel;
    QString textQueryOrdersView= "SELECT Orders.*, "
            "Order_List.id, "
            "Order_List.id_service "
            "FROM Orders "
            "LEFT JOIN Order_List "
            "ON Orders.id = Order_List.id_order "
            "WHERE Orders.status = 'выполняется'";
    queryOrdersView->setQuery(textQueryOrdersView);
    QTableView *ordersView = new QTableView();
    ordersView->setModel(queryOrdersView);
    ui->ordersView->setModel(queryOrdersView);
    ui->ordersView->resizeColumnsToContents();

    QSqlQueryModel *queryMastersView = new QSqlQueryModel;
    QString textQueryMastersView= "SELECT Workers.id, "
            "Workers.surname, "
            "Workers.[name], "
            "Workers.patronymic, "
            "COUNT(Orders.id) AS [number of cars] "
            "FROM Workers "
            "JOIN Orders ON Workers.id = Orders.id_master "
            "WHERE Orders.status = 'выполняется' "
            "GROUP BY Workers.id, Workers.[name], Workers.surname, Workers.patronymic "
            "ORDER BY COUNT(Orders.id) DESC ";
    queryMastersView->setQuery(textQueryMastersView);
    QTableView *mastersView = new QTableView();
    mastersView->setModel(queryMastersView);
    ui->mastersView->setModel(queryMastersView);
    ui->mastersView->resizeColumnsToContents();

    QSqlQueryModel *queryFreeMastersView = new QSqlQueryModel;
    QString textQueryFreeMastersView= "SELECT Workers.id, "
            "Workers.surname, "
            "Workers.[name], "
            "Workers.patronymic "
            "FROM Workers "
            "WHERE NOT EXISTS "
            "(SELECT * "
            "FROM Orders "
            "WHERE Workers.id = Orders.id_master "
            "AND Orders.status = 'выполняется') "
            "AND Workers.id_position = '1'";
    queryFreeMastersView->setQuery(textQueryFreeMastersView);
    QTableView *freeMastersView = new QTableView();
    freeMastersView->setModel(queryFreeMastersView);
    ui->freeMastersView->setModel(queryFreeMastersView);
    ui->freeMastersView->resizeColumnsToContents();

    QSqlTableModel *clientsView = new QSqlTableModel(this, dataBase);
    clientsView->setTable("ClientsLogin_Details");
    ui->clientsView->setModel(clientsView);
    clientsView->select();
    ui->clientsView->resizeColumnsToContents();

    QSqlTableModel *carsView = new QSqlTableModel(this, dataBase);
    carsView->setTable("Cars");
    ui->carsView->setModel(carsView);
    carsView->select();
    ui->carsView->resizeColumnsToContents();

    QSqlTableModel *sparesView = new QSqlTableModel(this, dataBase);
    sparesView->setTable("Spares");
    ui->sparesView->setModel(sparesView);
    sparesView->select();
    ui->sparesView->resizeColumnsToContents();

    QSqlTableModel *servicesView = new QSqlTableModel(this, dataBase);
    servicesView->setTable("Services");
    ui->servicesView->setModel(servicesView);
    servicesView->select();
    ui->servicesView->resizeColumnsToContents();
}

Manager::~Manager()
{
    delete ui;
}

//Создание обращения
void Manager::on_pushButton_29_clicked()
{
    {
        //Проверка на пустые поля
        bool isEmptyString = false;

        QString idCar = ui->idCarInsert->text();
        if (!isEmptyString)
        {
            isEmptyString = idCar.isEmpty();
        }

        QString idMaster = ui->idMasterInsert->text();
        if (!isEmptyString)
        {
            isEmptyString = idMaster.isEmpty();
        }

        if (isEmptyString)
        {
            QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля");
        }
        else
        {
            //Проверка, есть ли такой idCar
            QString textQueryCheckIdCar = "SELECT * FROM Cars WHERE id = '" + idCar + "'";
            QSqlQuery queryCheckIdCar;
            queryCheckIdCar.exec(textQueryCheckIdCar);
            if (!queryCheckIdCar.first())
            {
                QMessageBox::warning(this, "Ошибка", "Автомобиль с таким id не найден");
            }
            else
            {
                //Проверка, есть ли такой idMaster
                QString textQueryCheckIdMaster = "SELECT * FROM Workers WHERE id_position = '1' AND id = '" + idMaster + "'";
                QSqlQuery queryCheckIdMaster;
                queryCheckIdMaster.exec(textQueryCheckIdMaster);
                if (!queryCheckIdMaster.first())
                {
                    QMessageBox::warning(this, "Ошибка", "Мастер с таким id не найден");
                }
                else
                {
                    //Текущая дата
                    QDate creation_date = QDate::currentDate();

                    //Добавить данные в таблицу Orders
                    QString textOfQueryInsert = "INSERT INTO Orders (id_car, status, id_manager, id_master, creation_date) VALUES ('" +
                                               idCar + "', 'выполняется', '" + idManager + "', '" + idMaster + "', '" + creation_date.toString(Qt::ISODate) + "')";

                    QSqlQuery queryInsert;
                    queryInsert.exec(textOfQueryInsert);

                    //Обновление таблиц
                    QSqlQueryModel *queryOrdersView = new QSqlQueryModel;
                    QString textQueryOrdersView= "SELECT Orders.*, "
                            "Order_List.id, "
                            "Order_List.id_service "
                            "FROM Orders "
                            "LEFT JOIN Order_List "
                            "ON Orders.id = Order_List.id_order "
                            "WHERE Orders.status = 'выполняется'";
                    queryOrdersView->setQuery(textQueryOrdersView);
                    QTableView *ordersView = new QTableView();
                    ordersView->setModel(queryOrdersView);
                    ui->ordersView->setModel(queryOrdersView);
                    ui->ordersView->resizeColumnsToContents();

                    QSqlQueryModel *queryMastersView = new QSqlQueryModel;
                    QString textQueryMastersView= "SELECT Workers.id, "
                            "Workers.surname, "
                            "Workers.[name], "
                            "Workers.patronymic, "
                            "COUNT(Orders.id) AS [number of cars] "
                            "FROM Workers "
                            "JOIN Orders ON Workers.id = Orders.id_master "
                            "WHERE Orders.status = 'выполняется' "
                            "GROUP BY Workers.id, Workers.[name], Workers.surname, Workers.patronymic "
                            "ORDER BY COUNT(Orders.id) DESC ";
                    queryMastersView->setQuery(textQueryMastersView);
                    QTableView *mastersView = new QTableView();
                    mastersView->setModel(queryMastersView);
                    ui->mastersView->setModel(queryMastersView);
                    ui->mastersView->resizeColumnsToContents();

                    QSqlQueryModel *queryFreeMastersView = new QSqlQueryModel;
                    QString textQueryFreeMastersView= "SELECT Workers.id, "
                            "Workers.surname, "
                            "Workers.[name], "
                            "Workers.patronymic "
                            "FROM Workers "
                            "WHERE NOT EXISTS "
                            "(SELECT * "
                            "FROM Orders "
                            "WHERE Workers.id = Orders.id_master "
                            "AND Orders.status = 'выполняется') "
                            "AND Workers.id_position = '1'";
                    queryFreeMastersView->setQuery(textQueryFreeMastersView);
                    QTableView *freeMastersView = new QTableView();
                    freeMastersView->setModel(queryFreeMastersView);
                    ui->freeMastersView->setModel(queryFreeMastersView);
                    ui->freeMastersView->resizeColumnsToContents();

                    QMessageBox::information(this, "Успех", "Заказ успешно добавлен");
                }
            }
        }
    }
}

//Завершение обращения
void Manager::on_pushButton_32_clicked()
{
    {
        QString id = ui->idDelete_3->text();
        if (id.isEmpty())
        {
             QMessageBox::warning(this, "Ошибка", "Введите id обращения");
        }
        else
        {
            //Проверка, есть ли такой id
            QString textQueryCheckId = "SELECT * FROM Orders WHERE id = '" + id + "'";
            QSqlQuery queryCheckId;
            queryCheckId.exec(textQueryCheckId);
            if (!queryCheckId.first())
            {
                QMessageBox::warning(this, "Ошибка", "Обращение с таким id не найдено");
            }
            else
            {
                //Проверка, привязаны ли услуги к обращению
                QString textQueryCheckOrder = "SELECT * FROM Order_List WHERE Order_List.id_order = '" + id + "'";
                QSqlQuery queryCheckOrder;
                queryCheckOrder.exec(textQueryCheckOrder);
                if (queryCheckOrder.first())
                {
                    QMessageBox::warning(this, "Ошибка", "Нельзя завершить обращение, если к нему привязаны услуги");
                }
                else
                {
                    QString textQueryDelete = "DELETE FROM Orders WHERE id = '" + id + "'";
                    QSqlQuery queryDelete;
                    queryDelete.exec(textQueryDelete);

                    //Обновление таблиц
                    QSqlQueryModel *queryOrdersView = new QSqlQueryModel;
                    QString textQueryOrdersView= "SELECT Orders.*, "
                            "Order_List.id, "
                            "Order_List.id_service "
                            "FROM Orders "
                            "LEFT JOIN Order_List "
                            "ON Orders.id = Order_List.id_order "
                            "WHERE Orders.status = 'выполняется'";
                    queryOrdersView->setQuery(textQueryOrdersView);
                    QTableView *ordersView = new QTableView();
                    ordersView->setModel(queryOrdersView);
                    ui->ordersView->setModel(queryOrdersView);
                    ui->ordersView->resizeColumnsToContents();

                    QSqlQueryModel *queryMastersView = new QSqlQueryModel;
                    QString textQueryMastersView= "SELECT Workers.id, "
                            "Workers.surname, "
                            "Workers.[name], "
                            "Workers.patronymic, "
                            "COUNT(Orders.id) AS [number of cars] "
                            "FROM Workers "
                            "JOIN Orders ON Workers.id = Orders.id_master "
                            "WHERE Orders.status = 'выполняется' "
                            "GROUP BY Workers.id, Workers.[name], Workers.surname, Workers.patronymic "
                            "ORDER BY COUNT(Orders.id) DESC ";
                    queryMastersView->setQuery(textQueryMastersView);
                    QTableView *mastersView = new QTableView();
                    mastersView->setModel(queryMastersView);
                    ui->mastersView->setModel(queryMastersView);
                    ui->mastersView->resizeColumnsToContents();

                    QSqlQueryModel *queryFreeMastersView = new QSqlQueryModel;
                    QString textQueryFreeMastersView= "SELECT Workers.id, "
                            "Workers.surname, "
                            "Workers.[name], "
                            "Workers.patronymic "
                            "FROM Workers "
                            "WHERE NOT EXISTS "
                            "(SELECT * "
                            "FROM Orders "
                            "WHERE Workers.id = Orders.id_master "
                            "AND Orders.status = 'выполняется') "
                            "AND Workers.id_position = '1'";
                    queryFreeMastersView->setQuery(textQueryFreeMastersView);
                    QTableView *freeMastersView = new QTableView();
                    freeMastersView->setModel(queryFreeMastersView);
                    ui->freeMastersView->setModel(queryFreeMastersView);
                    ui->freeMastersView->resizeColumnsToContents();

                    QMessageBox::information(this, "Успех", "Обращение успешно удалено");
                }
            }
        }
    }
}

//Добавление услуги
void Manager::on_pushButton_30_clicked()
{
    {
        {
            //Проверка на пустые поля
            bool isEmptyString = false;

            QString idOrder = ui->idOrderInsert->text();
            if (!isEmptyString)
            {
                isEmptyString = idOrder.isEmpty();
            }

            QString idService = ui->idServiceInsert->text();
            if (!isEmptyString)
            {
                isEmptyString = idService.isEmpty();
            }

            if (isEmptyString)
            {
                QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля");
            }
            else
            {
                //Проверка, есть ли такой idOrder
                QString textQueryCheckIdOrder = "SELECT * FROM Orders WHERE id = '" + idOrder + "'";
                QSqlQuery queryCheckIdOrder;
                queryCheckIdOrder.exec(textQueryCheckIdOrder);
                if (!queryCheckIdOrder.first())
                {
                    QMessageBox::warning(this, "Ошибка", "Заказ с таким id не найден");
                }
                else
                {
                    //Проверка, есть ли такой idService
                    QString textQueryCheckIdService = "SELECT * FROM Services WHERE id = '" + idService + "'";
                    QSqlQuery queryCheckIdService;
                    queryCheckIdService.exec(textQueryCheckIdService);
                    if (!queryCheckIdService.first())
                    {
                        QMessageBox::warning(this, "Ошибка", "Услуга с таким id не найдена");
                    }
                    else
                    {
                        //Добавить данные в таблицу Orders
                        QString textOfQueryInsert = "INSERT INTO Order_List (id_service, id_order) VALUES ('" +
                                                   idService + "', '" + idOrder + "')";

                        QSqlQuery queryInsert;
                        queryInsert.exec(textOfQueryInsert);

                        //Обновление таблиц
                        QSqlQueryModel *queryOrdersView = new QSqlQueryModel;
                        QString textQueryOrdersView= "SELECT Orders.*, "
                                "Order_List.id, "
                                "Order_List.id_service "
                                "FROM Orders "
                                "LEFT JOIN Order_List "
                                "ON Orders.id = Order_List.id_order "
                                "WHERE Orders.status = 'выполняется'";
                        queryOrdersView->setQuery(textQueryOrdersView);
                        QTableView *ordersView = new QTableView();
                        ordersView->setModel(queryOrdersView);
                        ui->ordersView->setModel(queryOrdersView);
                        ui->ordersView->resizeColumnsToContents();

                        QSqlQueryModel *queryMastersView = new QSqlQueryModel;
                        QString textQueryMastersView= "SELECT Workers.id, "
                                "Workers.surname, "
                                "Workers.[name], "
                                "Workers.patronymic, "
                                "COUNT(Orders.id) AS [number of cars] "
                                "FROM Workers "
                                "JOIN Orders ON Workers.id = Orders.id_master "
                                "WHERE Orders.status = 'выполняется' "
                                "GROUP BY Workers.id, Workers.[name], Workers.surname, Workers.patronymic "
                                "ORDER BY COUNT(Orders.id) DESC ";
                        queryMastersView->setQuery(textQueryMastersView);
                        QTableView *mastersView = new QTableView();
                        mastersView->setModel(queryMastersView);
                        ui->mastersView->setModel(queryMastersView);
                        ui->mastersView->resizeColumnsToContents();

                        QSqlQueryModel *queryFreeMastersView = new QSqlQueryModel;
                        QString textQueryFreeMastersView= "SELECT Workers.id, "
                                "Workers.surname, "
                                "Workers.[name], "
                                "Workers.patronymic "
                                "FROM Workers "
                                "WHERE NOT EXISTS "
                                "(SELECT * "
                                "FROM Orders "
                                "WHERE Workers.id = Orders.id_master "
                                "AND Orders.status = 'выполняется') "
                                "AND Workers.id_position = '1'";
                        queryFreeMastersView->setQuery(textQueryFreeMastersView);
                        QTableView *freeMastersView = new QTableView();
                        freeMastersView->setModel(queryFreeMastersView);
                        ui->freeMastersView->setModel(queryFreeMastersView);
                        ui->freeMastersView->resizeColumnsToContents();

                        QMessageBox::information(this, "Успех", "Услуга успешно добавлена");
                    }
                }
            }
        }
    }
}

//Удаление услуги
void Manager::on_pushButton_31_clicked()
{
    QString id = ui->idDelete_2->text();
    if (id.isEmpty())
    {
         QMessageBox::warning(this, "Ошибка", "Введите id списка услуг");
    }
    else
    {
        //Проверка, есть ли такой id
        QString textQueryCheckId = "SELECT * FROM Order_List WHERE id = '" + id + "'";
        QSqlQuery queryCheckId;
        queryCheckId.exec(textQueryCheckId);
        if (!queryCheckId.first())
        {
            QMessageBox::warning(this, "Ошибка", "Список услуг с таким id не найден");
        }
        else
        {
            //Проверка, добавлены ли используемые запчасти
            QString textQueryCheckSpares = "SELECT * FROM Used_Spares WHERE id_order_list = '" + id + "'";
            QSqlQuery queryCheckSpares;
            queryCheckSpares.exec(textQueryCheckSpares);
            if (queryCheckSpares.first())
            {
                QMessageBox::warning(this, "Ошибка", "Нельзя удалить услугу, так как добавлены используемые запчасти");
            }
            else
            {
                QString textQueryDelete = "DELETE FROM Order_List WHERE id = '" + id + "'";
                QSqlQuery queryDelete;
                queryDelete.exec(textQueryDelete);

                //Обновление таблиц
                QSqlQueryModel *queryOrdersView = new QSqlQueryModel;
                QString textQueryOrdersView= "SELECT Orders.*, "
                        "Order_List.id, "
                        "Order_List.id_service "
                        "FROM Orders "
                        "LEFT JOIN Order_List "
                        "ON Orders.id = Order_List.id_order "
                        "WHERE Orders.status = 'выполняется'";
                queryOrdersView->setQuery(textQueryOrdersView);
                QTableView *ordersView = new QTableView();
                ordersView->setModel(queryOrdersView);
                ui->ordersView->setModel(queryOrdersView);
                ui->ordersView->resizeColumnsToContents();

                QSqlQueryModel *queryMastersView = new QSqlQueryModel;
                QString textQueryMastersView= "SELECT Workers.id, "
                        "Workers.surname, "
                        "Workers.[name], "
                        "Workers.patronymic, "
                        "COUNT(Orders.id) AS [number of cars] "
                        "FROM Workers "
                        "JOIN Orders ON Workers.id = Orders.id_master "
                        "WHERE Orders.status = 'выполняется' "
                        "GROUP BY Workers.id, Workers.[name], Workers.surname, Workers.patronymic "
                        "ORDER BY COUNT(Orders.id) DESC ";
                queryMastersView->setQuery(textQueryMastersView);
                QTableView *mastersView = new QTableView();
                mastersView->setModel(queryMastersView);
                ui->mastersView->setModel(queryMastersView);
                ui->mastersView->resizeColumnsToContents();

                QSqlQueryModel *queryFreeMastersView = new QSqlQueryModel;
                QString textQueryFreeMastersView= "SELECT Workers.id, "
                        "Workers.surname, "
                        "Workers.[name], "
                        "Workers.patronymic "
                        "FROM Workers "
                        "WHERE NOT EXISTS "
                        "(SELECT * "
                        "FROM Orders "
                        "WHERE Workers.id = Orders.id_master "
                        "AND Orders.status = 'выполняется') "
                        "AND Workers.id_position = '1'";
                queryFreeMastersView->setQuery(textQueryFreeMastersView);
                QTableView *freeMastersView = new QTableView();
                freeMastersView->setModel(queryFreeMastersView);
                ui->freeMastersView->setModel(queryFreeMastersView);
                ui->freeMastersView->resizeColumnsToContents();

                QMessageBox::information(this, "Успех", "Услуга успешно удалена");
            }
        }
    }
}

//Добавление клиента
void Manager::on_pushButton_clicked()
{
    //Проверка на пустые поля
    bool isEmptyString = false;

    QString secondName = ui->secondNameInsert->text();
    if (!isEmptyString)
    {
        isEmptyString = secondName.isEmpty();
    }

    QString name = ui->nameInsert->text();
    if (!isEmptyString)
    {
        isEmptyString = name.isEmpty();
    }

    QString patronymic = ui->patronymicInsert->text();

    QString phone = ui->phoneInsert->text();
    if (!isEmptyString)
    {
        isEmptyString = phone.isEmpty();
    }

    QString login = ui->loginInsert->text();
    if (!isEmptyString)
    {
        isEmptyString = login.isEmpty();
    }

    QString password = ui->passwordInsert->text();
    if (!isEmptyString)
    {
        isEmptyString = password.isEmpty();
    }

    if (isEmptyString)
    {
        QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля");
    }
    else
    {
        //Проверка на уникальность логина
        QString textOfQueryCheckLoginDetails = "SELECT * FROM Login_Details WHERE Login_Details.login = '" +
                login + "'";
        QSqlQuery queryCheckLoginDetails;
        queryCheckLoginDetails.exec(textOfQueryCheckLoginDetails);

        if (queryCheckLoginDetails.first())
        {
             QMessageBox::warning(this, "Ошибка", "Пользователь с таким логином уже зарегистрирован");
        }
        else
        {
            //Проверка на уникальность номера телефона
            QString textOfQueryCheckPhone = "SELECT phone_number FROM Clients WHERE phone_number = '" +
                    phone + "' UNION SELECT Workers.phone_number FROM Workers WHERE phone_number = '" +
                    phone + "' UNION SELECT Owner.phone_number FROM Owner WHERE phone_number = '" +
                    phone + "'";
            QSqlQuery queryCheckPhone;
            queryCheckPhone.exec(textOfQueryCheckPhone);

            if (queryCheckPhone.first())
            {
                QMessageBox::warning(this, "Ошибка", "Пользователь с таким номером телефона уже зарегистрирован");
            }
            else
            {
                //Добавить данные в таблицу Login_Details
                QString textOfQueryInsertLoginDetails = "OPEN SYMMETRIC KEY symmetric_key "
                        "DECRYPTION BY CERTIFICATE certificate; "
                        "INSERT INTO Login_Details ([login], [password]) "
                        "VALUES ('" + login + "', EncryptByKey(Key_GUID('symmetric_key'),'" + password + "'))";
                QSqlQuery queryInsertLoginDetails;
                queryInsertLoginDetails.exec(textOfQueryInsertLoginDetails);

                //Выбрать id_login_details
                queryCheckLoginDetails.exec(textOfQueryCheckLoginDetails);
                queryCheckLoginDetails.next();
                QString idLoginDetails = queryCheckLoginDetails.value(0).toString();

                //Добавить данные в таблицу Clients
                QString textOfQueryInsert = "INSERT INTO Clients ([name], surname, patronymic, phone_number, id_login_details) "
                                            "VALUES ('" + name + "', '" + secondName + "', '" + patronymic + "', '" + phone + "', '" + idLoginDetails + "')";

                QSqlQuery queryInsert;
                queryInsert.exec(textOfQueryInsert);

                //Обновление таблицы
                QSqlTableModel *clientsView = new QSqlTableModel(this, dataBase);
                clientsView->setTable("ClientsLogin_Details");
                ui->clientsView->setModel(clientsView);
                clientsView->select();
                ui->clientsView->resizeColumnsToContents();

                QMessageBox::information(this, "Успех", "Пользователь успешно добавлен");
            }
        }
    }
}

//Изменение клиента
void Manager::on_pushButton_3_clicked()
{
    //Не изменились заполненные поля
    bool isNotChanged = false;

    //Проверка на пустое поле id
    QString id = ui->idUpdate->text();
    if (id.isEmpty())
    {
         QMessageBox::warning(this, "Ошибка", "Введите id пользователя");
    }
    else
    {
        //Проверка, есть ли такой id
        QString textQueryCheckId = "SELECT * FROM Clients WHERE id = '" + id + "'";
        QSqlQuery queryCheckId;
        queryCheckId.exec(textQueryCheckId);
        if (!queryCheckId.first())
        {
            QMessageBox::warning(this, "Ошибка", "Пользователь с таким id не найден");
        }
        else
        {
            //Проверка на пустые поля
            bool isEmptyString = true;

            QString secondName = ui->secondNameUpdate->text();
            if (!secondName.isEmpty())
            {
                isEmptyString = false;
                QString textQuerySecondNameUpdate = "UPDATE Clients SET surname = '" + secondName + "' WHERE id = '" + id + "'";
                QSqlQuery querySecondNameUpdate;
                querySecondNameUpdate.exec(textQuerySecondNameUpdate);
            }

            QString name = ui->nameUpdate->text();
            if (!name.isEmpty())
            {
                isEmptyString = false;
                QString textQueryNameUpdate = "UPDATE Clients SET name = '" + name + "' WHERE id = '" + id + "'";
                QSqlQuery queryNameUpdate;
                queryNameUpdate.exec(textQueryNameUpdate);
            }

            QString patronymic = ui->patronymicUpdate->text();
            if (!patronymic.isEmpty())
            {
                isEmptyString = false;
                QString textQueryPatronymicUpdate = "UPDATE Clients SET patronymic = '" + patronymic + "' WHERE id = '" + id + "'";
                QSqlQuery queryPatronymicUpdate;
                queryPatronymicUpdate.exec(textQueryPatronymicUpdate);
            }

            QString phone = ui->phoneUpdate->text();
            if (!phone.isEmpty())
            {
                isEmptyString = false;

                //Проверка на уникальность номера телефона
                QString textOfQueryCheckPhone = "SELECT phone_number FROM Clients WHERE phone_number = '" +
                        phone + "' UNION SELECT Workers.phone_number FROM Workers WHERE phone_number = '" +
                        phone + "' UNION SELECT Owner.phone_number FROM Owner WHERE phone_number = '" +
                        phone + "'";
                QSqlQuery queryCheckPhone;
                queryCheckPhone.exec(textOfQueryCheckPhone);

                if (queryCheckPhone.first())
                {
                    QMessageBox::warning(this, "Ошибка", "Пользователь с таким номером телефона уже зарегистрирован");
                    isNotChanged = true;
                }
                else
                {
                    QString textQueryPhoneUpdate = "UPDATE Clients SET phone_number = '" + phone + "' WHERE id = '" + id + "'";
                    QSqlQuery queryPhoneUpdate;
                    queryPhoneUpdate.exec(textQueryPhoneUpdate);
                }
            }

            QString login = ui->loginUpdate->text();
            if (!login.isEmpty())
            {
                isEmptyString = false;

                //Проверка на уникальность логина
                QString textOfQueryCheckLoginDetails = "SELECT * FROM Login_Details WHERE Login_Details.login = '" +
                        login + "'";
                QSqlQuery queryCheckLoginDetails;
                queryCheckLoginDetails.exec(textOfQueryCheckLoginDetails);

                if (queryCheckLoginDetails.first())
                {
                     QMessageBox::warning(this, "Ошибка", "Пользователь с таким логином уже зарегистрирован");
                     isNotChanged = true;
                }
                else
                {
                    //Выбрать id_login_details
                    QString textOfQuerySelectIdLoginDetails = "SELECT id_login_details FROM Clients WHERE id = '" + id + "'";
                    QSqlQuery querySelectIdLoginDetails;
                    querySelectIdLoginDetails.exec(textOfQuerySelectIdLoginDetails);
                    querySelectIdLoginDetails.next();
                    QString idLoginDetails = querySelectIdLoginDetails.value(0).toString();

                    QString textQueryLoginUpdate = "UPDATE Login_Details SET login = '" + login + "' WHERE id = '" + idLoginDetails + "'";
                    QSqlQuery queryLoginUpdate;
                    queryLoginUpdate.exec(textQueryLoginUpdate);
                }
            }

            QString password = ui->passwordUpdate->text();
            if (!password.isEmpty())
            {
                isEmptyString = false;

                //Выбрать id_login_details
                QString textOfQuerySelectIdLoginDetails = "SELECT id_login_details FROM Clients WHERE id = '" + id + "'";
                QSqlQuery querySelectIdLoginDetails;
                querySelectIdLoginDetails.exec(textOfQuerySelectIdLoginDetails);
                querySelectIdLoginDetails.next();
                QString idLoginDetails = querySelectIdLoginDetails.value(0).toString();

                QString textQueryPasswordUpdate = "OPEN SYMMETRIC KEY symmetric_key "
                        "DECRYPTION BY CERTIFICATE certificate; "
                        "UPDATE Login_Details SET password = EncryptByKey(Key_GUID('symmetric_key'),'" + password + "') "
                        "WHERE id = '" + idLoginDetails + "'";
                QSqlQuery queryPasswordUpdate;
                queryPasswordUpdate.exec(textQueryPasswordUpdate);
            }

            if (isEmptyString)
            {
                QMessageBox::warning(this, "Ошибка", "Заполните поля, которые нужно изменить");
            }
            else
            {
                //Обновление таблицы
                QSqlTableModel *clientsView = new QSqlTableModel(this, dataBase);
                clientsView->setTable("ClientsLogin_Details");
                ui->clientsView->setModel(clientsView);
                clientsView->select();
                ui->clientsView->resizeColumnsToContents();

                if (!isNotChanged)
                {
                    QMessageBox::information(this, "Успех", "Пользователь успешно обновлен");
                }
            }
        }
    }
}

//Добавление автомобиля
void Manager::on_pushButton_4_clicked()
{
    {
        //Проверка на пустые поля
        bool isEmptyString = false;

        QString idClient = ui->idClientInsert->text();
        if (!isEmptyString)
        {
            isEmptyString = idClient.isEmpty();
        }

        QString number = ui->numberInsert->text();
        if (!isEmptyString)
        {
            isEmptyString = number.isEmpty();
        }

        QString brand = ui->brandInsert->text();
        if (!isEmptyString)
        {
            isEmptyString = brand.isEmpty();
        }

        QString model = ui->modelInsert->text();

        QString color = ui->colorInsert->text();

        if (isEmptyString)
        {
            QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля");
        }
        else
        {
            //Проверка, есть ли такой idClient
            QString textQueryCheckId = "SELECT * FROM Clients WHERE id = '" + idClient + "'";
            QSqlQuery queryCheckId;
            queryCheckId.exec(textQueryCheckId);
            if (!queryCheckId.first())
            {
                QMessageBox::warning(this, "Ошибка", "Клиент с таким id не найден");
            }
            else
            {
                //Проверка на уникальность номера
                QString textOfQueryCheckNumber = "SELECT * FROM Cars "
                                                 "WHERE Cars.number = '" + number + "'";
                QSqlQuery queryCheckNumber;
                queryCheckNumber.exec(textOfQueryCheckNumber);

                if (queryCheckNumber.first())
                {
                     QMessageBox::warning(this, "Ошибка", "Автомобиль с таким номером уже существует");
                }
                else
                {
                    //Добавить данные в таблицу Cars
                    QString textOfQueryInsert = "INSERT INTO Cars (id_owner, number, brand, model, color) VALUES ('" +
                                               idClient + "', '" + number + "', '" + brand + "', '" + model + "', '" + color + "')";

                    QSqlQuery queryInsert;
                    queryInsert.exec(textOfQueryInsert);

                    //Обновление таблицы
                    QSqlTableModel *carsView = new QSqlTableModel(this, dataBase);
                    carsView->setTable("Cars");
                    ui->carsView->setModel(carsView);
                    carsView->select();
                    ui->carsView->resizeColumnsToContents();

                    QMessageBox::information(this, "Успех", "Автомобиль успешно добавлен");
                }
            }
        }
    }
}

//Изменение автомобиля
void Manager::on_pushButton_5_clicked()
{
    {
        //Не изменились заполненные поля
        bool isNotChanged = false;

        //Проверка на пустое поле id
        QString idCar = ui->idCarUpdate->text();
        if (idCar.isEmpty())
        {
             QMessageBox::warning(this, "Ошибка", "Введите id автомобиля");
        }
        else
        {
            //Проверка, есть ли такой id
            QString textQueryCheckId = "SELECT * FROM Cars WHERE id = '" + idCar + "'";
            QSqlQuery queryCheckId;
            queryCheckId.exec(textQueryCheckId);
            if (!queryCheckId.first())
            {
                QMessageBox::warning(this, "Ошибка", "Автомобиль с таким id не найден");
            }
            else
            {
                //Проверка на пустые поля
                bool isEmptyString = true;

                QString number = ui->numberUpdate->text();
                if (!number.isEmpty())
                {
                    //Проверка на уникальность номера
                    isEmptyString = false;
                    QString textOfQueryCheckNumber = "SELECT * FROM Cars WHERE Cars.number = '" +
                            number + "'";
                    QSqlQuery queryCheckNumber;
                    queryCheckNumber.exec(textOfQueryCheckNumber);

                    if (queryCheckNumber.first())
                    {
                         QMessageBox::warning(this, "Ошибка", "Автомобиль с таким номером уже существует");
                         isNotChanged = true;
                    }
                    else
                    {
                        QString textQueryNumberUpdate = "UPDATE Cars SET number = '" + number + "' WHERE id = '" + idCar + "'";
                        QSqlQuery queryNumberUpdate;
                        queryNumberUpdate.exec(textQueryNumberUpdate);
                    }
                }

                QString brand = ui->brandUpdate->text();
                if (!brand.isEmpty())
                {
                    isEmptyString = false;
                    QString textQueryBrandUpdate = "UPDATE Cars SET brand = '" + brand + "' WHERE id = '" + idCar + "'";
                    QSqlQuery queryBrandUpdate;
                    queryBrandUpdate.exec(textQueryBrandUpdate);
                }

                QString model = ui->modelUpdate->text();
                if (!model.isEmpty())
                {
                    isEmptyString = false;
                    QString textQueryModelUpdate = "UPDATE Cars SET model = '" + model + "' WHERE id = '" + idCar + "'";
                    QSqlQuery queryModelUpdate;
                    queryModelUpdate.exec(textQueryModelUpdate);
                }

                QString color = ui->colorUpdate->text();
                if (!color.isEmpty())
                {
                    isEmptyString = false;
                    QString textQueryColorUpdate = "UPDATE Cars SET color = '" + color + "' WHERE id = '" + idCar + "'";
                    QSqlQuery queryColorUpdate;
                    queryColorUpdate.exec(textQueryColorUpdate);
                }

                if (isEmptyString)
                {
                    QMessageBox::warning(this, "Ошибка", "Заполните поля, которые нужно изменить");
                }
                else
                {
                    //Обновление таблицы
                    QSqlTableModel *carsView = new QSqlTableModel(this, dataBase);
                    carsView->setTable("Cars");
                    ui->carsView->setModel(carsView);
                    carsView->select();
                    ui->carsView->resizeColumnsToContents();

                    if (!isNotChanged)
                    {
                        QMessageBox::information(this, "Успех", "Автомобиль успешно изменен");
                    }
                }
            }
        }
    }
}
