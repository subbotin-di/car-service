#include "master.h"
#include "ui_master.h"

#include <QMessageBox>

Master::Master(QWidget *parent, QString id) :
    QWidget(parent),
    idMaster(id),
    ui(new Ui::Master)

{
    ui->setupUi(this);
    setWindowTitle("Мастер id=" + idMaster);
    dataBase = QSqlDatabase::addDatabase("QODBC");  // создание нового подключения с использованием драйвера QtSql ODBC
    dataBase.setDatabaseName(QString("DRIVER={SQL Server};Server=DESKTOP-944V2PH;Database=car_service;Trusted_Connection=yes;")); // установка параметров подключения
    if (!dataBase.open())   // установить подключение
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть базу данных");
        close();
    }

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

    QSqlQueryModel *queryOrdersView = new QSqlQueryModel;
    QString textQueryOrdersView = "SELECT Orders.id, "
            "Cars.number, "
            "Orders.status, "
            "Orders.creation_date "
            "FROM Orders, Workers, Cars "
            "WHERE Orders.id_master = Workers.id "
            "AND Orders.id_car = Cars.id "
            "AND Workers.id = '" + idMaster + "' "
            "AND Orders.status = 'выполняется' ";
    queryOrdersView->setQuery(textQueryOrdersView);
    QTableView *ordersView = new QTableView();
    ordersView->setModel(queryOrdersView);
    ui->ordersView->setModel(queryOrdersView);
    ui->ordersView->resizeColumnsToContents();

    QSqlQueryModel *queryUsedSpares = new QSqlQueryModel;
    QString textQueryUsedSpares = "SELECT Used_Spares.id, "
            "Orders.id, "
            "Services.name, "
            "Order_List.id_service, "
            "Spares.name, "
            "Used_Spares.id_spare "
            "FROM Orders "
            "JOIN Order_List "
            "ON Orders.id = Order_List.id_order "
            "JOIN Services "
            "ON Order_List.id_service = Services.id "
            "LEFT JOIN Used_Spares "
            "ON Order_List.id = Used_Spares.id_order_list "
            "FULL JOIN Spares "
            "ON Used_Spares.id_spare = Spares.id "
            "WHERE Orders.status = 'выполняется' "
            "AND Orders.id_master = '" + idMaster + "'";
    queryUsedSpares->setQuery(textQueryUsedSpares);
    QTableView *usedSpares = new QTableView();
    usedSpares->setModel(queryUsedSpares);
    ui->usedSpares->setModel(queryUsedSpares);
    ui->usedSpares->resizeColumnsToContents();
}

Master::~Master()
{
    delete ui;
}

//Добавление запчасти
void Master::on_pushButton_25_clicked()
{
    {
        //Проверка на пустые поля
        bool isEmptyString = false;

        QString name = ui->nameInsert_9->text();
        if (!isEmptyString)
        {
            isEmptyString = name.isEmpty();
        }

        QString code = ui->codeInsert_9->text();
        if (!isEmptyString)
        {
            isEmptyString = code.isEmpty();
        }

        QString price = ui->priceInsert_9->text();
        if (!isEmptyString)
        {
            isEmptyString = price.isEmpty();
        }

        QString quantity = ui->quantityInsert_9->text();
        if (!isEmptyString)
        {
            isEmptyString = quantity.isEmpty();
        }

        if (isEmptyString)
        {
            QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля");
        }
        else
        {
            //Проверка на уникальность названия
            QString textOfQueryCheckName = "SELECT * FROM Spares WHERE Spares.name = '" +
                    name + "'";
            QSqlQuery queryCheckName;
            queryCheckName.exec(textOfQueryCheckName);

            if (queryCheckName.first())
            {
                 QMessageBox::warning(this, "Ошибка", "Запчасть с таким именем уже существует");
            }
            else
            {
                //Проверка на уникальность кода
                QString textOfQueryCheckCode = "SELECT * FROM Spares WHERE Spares.code = '" +
                        code + "'";
                QSqlQuery queryCheckCode;
                queryCheckCode.exec(textOfQueryCheckCode);

                if (queryCheckCode.first())
                {
                     QMessageBox::warning(this, "Ошибка", "Запчасть с таким кодом уже существует");
                }
                else
                {
                    //Проверка на отрицательную стоимость
                    if (price < 0)
                    {
                         QMessageBox::warning(this, "Ошибка", "Цена запчасти не может быть отрицаетльной");
                    }
                    else
                    {
                        //Проверка на отрицательное количество
                        if (quantity < 0)
                        {
                             QMessageBox::warning(this, "Ошибка", "Количество запчастей не может быть отрицаетльным");
                        }
                        else
                        {
                            //Добавить данные в таблицу Spares
                            QString textOfQueryInsert = "INSERT INTO Spares (name, code, price, quantity) VALUES ('" +
                                                       name + "', '" + code + "', '" + price + "', '" + quantity + "')";

                            QSqlQuery queryInsert;
                            queryInsert.exec(textOfQueryInsert);

                            //Обновление таблицы
                            QSqlTableModel *sparesView = new QSqlTableModel(this, dataBase);
                            sparesView->setTable("Spares");
                            ui->sparesView->setModel(sparesView);
                            sparesView->select();
                            ui->sparesView->resizeColumnsToContents();

                            QMessageBox::information(this, "Успех", "Услуга успешно добавлена");
                        }
                    }
                }
            }
        }
    }
}

//Изменение запчасти
void Master::on_pushButton_26_clicked()
{
    //Не изменились заполненные поля
    bool isNotChanged = false;

    //Проверка на пустое поле id
    QString id = ui->idUpdate_9->text();
    if (id.isEmpty())
    {
         QMessageBox::warning(this, "Ошибка", "Введите id запчасти");
    }
    else
    {
        //Проверка, есть ли такой id
        QString textQueryCheckId = "SELECT * FROM Spares WHERE id = '" + id + "'";
        QSqlQuery queryCheckId;
        queryCheckId.exec(textQueryCheckId);
        if (!queryCheckId.first())
        {
            QMessageBox::warning(this, "Ошибка", "Запчасть с таким id не найдена");
        }
        else
        {
            //Проверка на пустые поля
            bool isEmptyString = true;

            QString name = ui->nameUpdate_9->text();
            if (!name.isEmpty())
            {
                isEmptyString = false;
                //Проверка на уникальность названия
                QString textOfQueryCheckName = "SELECT * FROM Spares WHERE Spares.name = '" +
                        name + "'";
                QSqlQuery queryCheckName;
                queryCheckName.exec(textOfQueryCheckName);

                if (queryCheckName.first())
                {
                     QMessageBox::warning(this, "Ошибка", "Запчасть с таким названием уже существует");
                     isNotChanged = true;
                }
                else
                {
                    QString textQueryNameUpdate = "UPDATE Spares SET name = '" + name + "' WHERE id = '" + id + "'";
                    QSqlQuery queryNameUpdate;
                    queryNameUpdate.exec(textQueryNameUpdate);
                }
            }

            QString code = ui->codeUpdate_9->text();
            if (!code.isEmpty())
            {
                isEmptyString = false;
                //Проверка на уникальность кода
                QString textOfQueryCheckCode = "SELECT * FROM Spares WHERE Spares.code = '" + code + "'";
                QSqlQuery queryCheckCode;
                queryCheckCode.exec(textOfQueryCheckCode);

                if (queryCheckCode.first())
                {
                     QMessageBox::warning(this, "Ошибка", "Запчасть с таким кодом уже существует");
                }
                else
                {
                    QString textQueryCodeUpdate = "UPDATE Spares SET code = '" + code + "' WHERE id = '" + id + "'";
                    QSqlQuery queryCodeUpdate;
                    queryCodeUpdate.exec(textQueryCodeUpdate);
                }
            }

            QString price = ui->priceUpdate_9->text();
            if (!price.isEmpty())
            {
                //Проверка на отрицательную стоимость
                if (price < 0)
                {
                     QMessageBox::warning(this, "Ошибка", "Цена запчасти не может быть отрицаетльной");
                }
                else
                {
                    isEmptyString = false;
                    QString textQueryPriceUpdate = "UPDATE Spares SET price = '" + price + "' WHERE id = '" + id + "'";
                    QSqlQuery queryPriceUpdate;
                    queryPriceUpdate.exec(textQueryPriceUpdate);
                }
            }

            QString quantity = ui->quantityUpdate_9->text();
            if (!quantity.isEmpty())
            {
                //Проверка на отрицательное количество
                if (quantity < 0)
                {
                     QMessageBox::warning(this, "Ошибка", "Количество запчастей не может быть отрицаетльным");
                }
                else
                {
                    isEmptyString = false;
                    QString textQueryQuantityUpdate = "UPDATE Spares SET quantity = '" + quantity + "' WHERE id = '" + id + "'";
                    QSqlQuery queryQuantityUpdate;
                    queryQuantityUpdate.exec(textQueryQuantityUpdate);
                }
            }

            if (isEmptyString)
            {
                QMessageBox::warning(this, "Ошибка", "Заполните поля, которые нужно изменить");
            }
            else
            {
                //Обновление таблицы
                QSqlTableModel *sparesView = new QSqlTableModel(this, dataBase);
                sparesView->setTable("Spares");
                ui->sparesView->setModel(sparesView);
                sparesView->select();
                ui->sparesView->resizeColumnsToContents();

                if (!isNotChanged)
                {
                QMessageBox::information(this, "Успех", "Услуга успешно обновлена");
                }
            }
        }
    }
}

//Изменение статуса
void Master::on_pushButton_28_clicked()
{
    {
        //Проверка на пустые поля
        bool isEmptyString = false;

        QString id = ui->idUpdate_11->text();
        if (!isEmptyString)
        {
            isEmptyString = id.isEmpty();
        }

        if (isEmptyString)
        {
            QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля");
        }
        else
        {
            //Проверка, есть ли такой id
            QString textQueryCheckId = "SELECT creation_date FROM Orders WHERE id = '" + id + "'";
            QSqlQuery queryCheckId;
            queryCheckId.exec(textQueryCheckId);
            if (!queryCheckId.first())
            {
                QMessageBox::warning(this, "Ошибка", "Заказ с таким id не найден");
            }
            else
            {
                QString textQueryStatusUpdate = "UPDATE Orders SET status = 'завершен' WHERE id = '" + id + "'";
                QSqlQuery queryStatusUpdate;
                queryStatusUpdate.exec(textQueryStatusUpdate);

                //Текущая дата
                QDate expirationDate = QDate::currentDate();
                QDate creationDate = queryCheckId.value(0).toDate();
                if (expirationDate < creationDate)
                {
                    QMessageBox::warning(this, "Ошибка", "Дата завершения заказа не может быть раньше даты создания заказа");
                }
                else
                {
                    QString date = expirationDate.toString(Qt::ISODate);
                    QString textQueryDateUpdate = "UPDATE Orders SET expiration_date = '" + date + "' WHERE id = '" + id + "'";
                    QSqlQuery queryDateUpdate;
                    queryDateUpdate.exec(textQueryDateUpdate);

                    //Обновление таблиц
                    QSqlQueryModel *queryOrdersView = new QSqlQueryModel;
                    QString textQueryOrdersView = "SELECT Orders.id, "
                            "Cars.number, "
                            "Orders.status, "
                            "Orders.creation_date "
                            "FROM Orders, Workers, Cars "
                            "WHERE Orders.id_master = Workers.id "
                            "AND Orders.id_car = Cars.id "
                            "AND Workers.id = '" + idMaster + "' "
                            "AND Orders.status = 'выполняется' ";
                    queryOrdersView->setQuery(textQueryOrdersView);
                    QTableView *ordersView = new QTableView();
                    ordersView->setModel(queryOrdersView);
                    ui->ordersView->setModel(queryOrdersView);
                    ui->ordersView->resizeColumnsToContents();

                    QSqlQueryModel *queryUsedSpares = new QSqlQueryModel;
                    QString textQueryUsedSpares = "SELECT Used_Spares.id, "
                            "Orders.id, "
                            "Services.name, "
                            "Order_List.id_service, "
                            "Spares.name, "
                            "Used_Spares.id_spare "
                            "FROM Orders "
                            "JOIN Order_List "
                            "ON Orders.id = Order_List.id_order "
                            "JOIN Services "
                            "ON Order_List.id_service = Services.id "
                            "LEFT JOIN Used_Spares "
                            "ON Order_List.id = Used_Spares.id_order_list "
                            "FULL JOIN Spares "
                            "ON Used_Spares.id_spare = Spares.id "
                            "WHERE Orders.status = 'выполняется' "
                            "AND Orders.id_master = '" + idMaster + "'";
                    queryUsedSpares->setQuery(textQueryUsedSpares);
                    QTableView *usedSpares = new QTableView();
                    usedSpares->setModel(queryUsedSpares);
                    ui->usedSpares->setModel(queryUsedSpares);
                    ui->usedSpares->resizeColumnsToContents();

                    QMessageBox::information(this, "Успех", "Статус успешно обновлен");
                }
            }
        }
    }
}

//Добавление использованной запчасти
void Master::on_pushButton_29_clicked()
{
    {
        //Проверка на пустые поля
        bool isEmptyString = false;

        QString id = ui->idUpdate_10->text();
        if (!isEmptyString)
        {
            isEmptyString = id.isEmpty();
        }

        QString service = ui->serviceUpdate_10->text();
        if (!isEmptyString)
        {
            isEmptyString = service.isEmpty();
        }

        QString spare = ui->spareUpdate_10->text();
        if (!isEmptyString)
        {
            isEmptyString = spare.isEmpty();
        }

        if (isEmptyString)
        {
            QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля");
        }
        else
        {
            //Проверка, есть ли такой id
            QString textQueryCheckId = "SELECT * FROM Orders WHERE id = '" + id + "'";
            QSqlQuery queryCheckId;
            queryCheckId.exec(textQueryCheckId);
            if (!queryCheckId.first())
            {
                QMessageBox::warning(this, "Ошибка", "Заказ с таким id не найден");
            }
            else
            {
                //Проверка, принадлежит ли заказ к этому мастеру
                QString textQueryCheckIdMaster = "SELECT * FROM Orders WHERE id = '" + id + "' AND Orders.id_master = '" + idMaster + "' AND Orders.status = 'выполняется'";
                QSqlQuery queryCheckIdMaster;
                queryCheckIdMaster.exec(textQueryCheckIdMaster);
                if (!queryCheckIdMaster.first())
                {
                    QMessageBox::warning(this, "Ошибка", "Обращение должно быть выполняемым в данный момент и принадлежать данному мастеру");
                }
                else
                {
                    //Выбрать idOrderList
                    QString textQueryCheckUsedSpares = "SELECT Order_List.id "
                            "FROM Order_List "
                            "WHERE Order_List.id_order = '" + id + "' "
                            "AND Order_List.id_service = '" + service + "'";
                    QSqlQuery queryCheckUsedSpares;
                    queryCheckUsedSpares.exec(textQueryCheckUsedSpares);
                    //Проверка, принадлежит ли услуга к этому заказу
                    if (!queryCheckUsedSpares.first())
                    {
                        QMessageBox::warning(this, "Ошибка", "Данная услуга не принадлежит заказу");
                    }
                    else
                    {
                        //Проверка, есть ли еще на складе использованные запчасти
                        QString textQueryCheckQuantity = "SELECT * FROM Spares WHERE id = '" + spare + "' AND quantity > '0'";
                        QSqlQuery queryCheckQuantity;
                        queryCheckQuantity.exec(textQueryCheckQuantity);
                        if (!queryCheckQuantity.first())
                        {
                            QMessageBox::warning(this, "Ошибка", "На складе нет данных запчастей");
                        }
                        else
                        {
                            QString idOrderList = queryCheckUsedSpares.value(0).toString();

                            //Добавление запчасти
                            QString textQueryUsed_SparesUpdate = "INSERT INTO Used_Spares (id_spare, id_order_list) VALUES ('" + spare + "', '" + idOrderList + "')";
                            QSqlQuery queryUsed_SparesUpdate;
                            queryUsed_SparesUpdate.exec(textQueryUsed_SparesUpdate);

                            //Изменение количества запчастей
                            QString textQuerySparesUpdate = "UPDATE Spares SET quantity = quantity - 1 WHERE id = '" + spare + "'";
                            QSqlQuery querySparesUpdate;
                            querySparesUpdate.exec(textQuerySparesUpdate);

                            //Обновление таблиц
                            QSqlQueryModel *queryUsedSpares = new QSqlQueryModel;
                            QString textQueryUsedSpares = "SELECT Used_Spares.id, "
                                    "Orders.id, "
                                    "Services.name, "
                                    "Order_List.id_service, "
                                    "Spares.name, "
                                    "Used_Spares.id_spare "
                                    "FROM Orders "
                                    "JOIN Order_List "
                                    "ON Orders.id = Order_List.id_order "
                                    "JOIN Services "
                                    "ON Order_List.id_service = Services.id "
                                    "LEFT JOIN Used_Spares "
                                    "ON Order_List.id = Used_Spares.id_order_list "
                                    "FULL JOIN Spares "
                                    "ON Used_Spares.id_spare = Spares.id "
                                    "WHERE Orders.status = 'выполняется' "
                                    "AND Orders.id_master = '" + idMaster + "'";
                            queryUsedSpares->setQuery(textQueryUsedSpares);
                            QTableView *usedSpares = new QTableView();
                            usedSpares->setModel(queryUsedSpares);
                            ui->usedSpares->setModel(queryUsedSpares);
                            ui->usedSpares->resizeColumnsToContents();

                            QSqlTableModel *sparesView = new QSqlTableModel(this, dataBase);
                            sparesView->setTable("Spares");
                            ui->sparesView->setModel(sparesView);
                            sparesView->select();
                            ui->sparesView->resizeColumnsToContents();

                            QMessageBox::information(this, "Успех", "Запчасть успешно добавлена");
                        }
                    }
                }
            }
        }
    }
}

//Удаление использованной запчасти
void Master::on_pushButton_30_clicked()
{
    QString id = ui->idUsedSparesDelete->text();
    if (id.isEmpty())
    {
         QMessageBox::warning(this, "Ошибка", "Введите id использованной запчасти");
    }
    else
    {
        //Проверка, есть ли такой id
        QString textQueryCheckId = "SELECT id_spare FROM Used_Spares WHERE id = '" + id + "'";
        QSqlQuery queryCheckId;
        queryCheckId.exec(textQueryCheckId);
        if (!queryCheckId.first())
        {
            QMessageBox::warning(this, "Ошибка", "Использованная запчасть с таким id не найдена");
        }
        else
        {
            QString textQueryDelete = "DELETE FROM Used_Spares WHERE id = '" + id + "'";
            QSqlQuery queryDelete;
            queryDelete.exec(textQueryDelete);

            //Получение id_spare
            QString spare = queryCheckId.value(0).toString();

            //Изменение количества запчастей
            QString textQuerySparesUpdate = "UPDATE Spares SET quantity = quantity + 1 WHERE id = '" + spare + "'";
            QSqlQuery querySparesUpdate;
            querySparesUpdate.exec(textQuerySparesUpdate);

            //Обновление таблиц
            QSqlQueryModel *queryUsedSpares = new QSqlQueryModel;
            QString textQueryUsedSpares = "SELECT Used_Spares.id, "
                    "Orders.id, "
                    "Services.name, "
                    "Order_List.id_service, "
                    "Spares.name, "
                    "Used_Spares.id_spare "
                    "FROM Orders "
                    "JOIN Order_List "
                    "ON Orders.id = Order_List.id_order "
                    "JOIN Services "
                    "ON Order_List.id_service = Services.id "
                    "LEFT JOIN Used_Spares "
                    "ON Order_List.id = Used_Spares.id_order_list "
                    "FULL JOIN Spares "
                    "ON Used_Spares.id_spare = Spares.id "
                    "WHERE Orders.status = 'выполняется' "
                    "AND Orders.id_master = '" + idMaster + "'";
            queryUsedSpares->setQuery(textQueryUsedSpares);
            QTableView *usedSpares = new QTableView();
            usedSpares->setModel(queryUsedSpares);
            ui->usedSpares->setModel(queryUsedSpares);
            ui->usedSpares->resizeColumnsToContents();

            QSqlTableModel *sparesView = new QSqlTableModel(this, dataBase);
            sparesView->setTable("Spares");
            ui->sparesView->setModel(sparesView);
            sparesView->select();
            ui->sparesView->resizeColumnsToContents();

            QMessageBox::information(this, "Успех", "Использованная запчасть успешно удалена");
        }
    }
}
