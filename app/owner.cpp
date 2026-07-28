#include "owner.h"
#include "ui_owner.h"

#include <QMessageBox>


Owner::Owner(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Owner)
{
       ui->setupUi(this);
       setWindowTitle("Владелец");
       dataBase = QSqlDatabase::addDatabase("QODBC");  // создание нового подключения с использованием драйвера QtSql ODBC
       dataBase.setDatabaseName(QString("DRIVER={SQL Server};Server=DESKTOP-944V2PH;Database=car_service;Trusted_Connection=yes;")); // установка параметров подключения
       if (!dataBase.open())   // установить подключение
       {
           QMessageBox::critical(this, "Ошибка", "Не удалось открыть базу данных");
           close();
       }

       QSqlTableModel *clientsView = new QSqlTableModel(this, dataBase);
       clientsView->setTable("ClientsLogin_Details");
       ui->clientsView->setModel(clientsView);
       clientsView->select();
       ui->clientsView->resizeColumnsToContents();

       QSqlTableModel *mastersView = new QSqlTableModel(this, dataBase);
       mastersView->setTable("MastersLogin_Details");
       ui->mastersView->setModel(mastersView);
       mastersView->select();
       ui->mastersView->resizeColumnsToContents();

       QSqlTableModel *managersView = new QSqlTableModel(this, dataBase);
       managersView->setTable("ManagersLogin_Details");
       ui->managersView->setModel(managersView);
       managersView->select();
       ui->managersView->resizeColumnsToContents();

       QSqlTableModel *listServices = new QSqlTableModel(this, dataBase);
       listServices->setTable("Services");
       ui->listServices->setModel(listServices);
       listServices->select();
       ui->listServices->resizeColumnsToContents();

       QSqlQueryModel *queryTopServices = new QSqlQueryModel;
       QString textQueryTopServices = "SELECT TOP 10 [Services].[name], "
                                      "COUNT(Order_List.id) AS [number of uses] "
                                      "FROM [Services] "
                                      "JOIN Order_List "
                                      "ON [Services].id = Order_List.id_service "
                                      "GROUP BY [Services].[name] "
                                      "ORDER BY COUNT(Order_List.id) DESC";
       queryTopServices->setQuery(textQueryTopServices);
       QTableView *topServices = new QTableView();
       topServices->setModel(queryTopServices);
       ui->topServices->setModel(queryTopServices);
       ui->topServices->resizeColumnsToContents();

       QSqlQueryModel *queryCashFlow = new QSqlQueryModel;
       QString textQueryCashFlow = "SELECT SUM([Services].price) + "
                                   "    (SELECT SUM(Spares.price) "
                                   "    FROM Spares "
                                   "    JOIN Used_Spares "
                                   "    ON Spares.id = Used_Spares.id_spare "
                                   "    JOIN Order_List "
                                   "    ON Used_Spares.id_order_list = Order_List.id "
                                   "    JOIN Orders "
                                   "    ON Order_List.id_order = Orders.id "
                                   "    WHERE DATEDIFF(day, Orders.expiration_date, CONVERT (date, GETDATE())) <= 365) "
                                   "AS [cash turnover per year] "
                                   "FROM [Services] "
                                   "JOIN Order_List "
                                   "ON [Services].id = Order_List.id_service "
                                   "JOIN Orders "
                                   "ON Order_List.id_order = Orders.id "
                                   "WHERE DATEDIFF(day, Orders.expiration_date, CONVERT (date, GETDATE())) <= 365";
       queryCashFlow->setQuery(textQueryCashFlow);
       QTableView *cashFlow = new QTableView();
       cashFlow->setModel(queryCashFlow);
       ui->cashFlow->setModel(queryCashFlow);
       ui->cashFlow->resizeColumnsToContents();
}

Owner::~Owner()
{
    delete ui;
}

//Добавление клиента
void Owner::on_pushButton_clicked()
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
                QString textOfQueryInsert = "INSERT INTO Clients ([name], surname, patronymic, phone_number, id_login_details) VALUES ('" +
                                           name + "', '" + secondName + "', '" + patronymic + "', '" + phone + "', '" + idLoginDetails + "')";

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
void Owner::on_pushButton_3_clicked()
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

//Добавление мастера
void Owner::on_pushButton_4_clicked()
{
    //Проверка на пустые поля
    bool isEmptyString = false;

    QString secondName = ui->secondNameInsert_2->text();
    if (!isEmptyString)
    {
        isEmptyString = secondName.isEmpty();
    }

    QString name = ui->nameInsert_2->text();
    if (!isEmptyString)
    {
        isEmptyString = name.isEmpty();
    }

    QString patronymic = ui->patronymicInsert_2->text();

    QString phone = ui->phoneInsert_2->text();
    if (!isEmptyString)
    {
        isEmptyString = phone.isEmpty();
    }

    QString login = ui->loginInsert_2->text();
    if (!isEmptyString)
    {
        isEmptyString = login.isEmpty();
    }

    QString password = ui->passwordInsert_2->text();
    if (!isEmptyString)
    {
        isEmptyString = password.isEmpty();
    }

    QString salary = ui->salaryInsert_2->text();
    if (!isEmptyString)
    {
        isEmptyString = salary.isEmpty();
    }

    if (isEmptyString)
    {
        QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля");
    }
    else
    {
        //Проверка на уникальность логина
        QString textOfQueryCheckLoginDetails = "SELECT * FROM Login_Details WHERE Login_Details.login = '" + login + "'";
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

                //Добавить данные в таблицу Masters
                QString textOfQueryInsert = "INSERT INTO Workers (id_position, [name], surname, patronymic, phone_number, id_login_details, salary) VALUES ('1', '" +
                                           name + "', '" + secondName + "', '" + patronymic + "', '" + phone + "', '" + idLoginDetails + "', '" + salary + "')";

                QSqlQuery queryInsert;
                queryInsert.exec(textOfQueryInsert);

                //Обновление таблицы
                QSqlTableModel *mastersView = new QSqlTableModel(this, dataBase);
                mastersView->setTable("MastersLogin_Details");
                ui->mastersView->setModel(mastersView);
                mastersView->select();
                ui->mastersView->resizeColumnsToContents();

                QMessageBox::information(this, "Успех", "Пользователь успешно добавлен");
            }
        }
    }
}

//Изменение мастера
void Owner::on_pushButton_5_clicked()
{
    //Не изменились заполненные поля
    bool isNotChanged = false;

    //Проверка на пустое поле id
    QString id = ui->idUpdate_2->text();
    if (id.isEmpty())
    {
         QMessageBox::warning(this, "Ошибка", "Введите id пользователя");
    }
    else
    {
        //Проверка, есть ли такой id
        QString textQueryCheckId = "SELECT * FROM Workers WHERE id = '" + id + "'";
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

            QString secondName = ui->secondNameUpdate_2->text();
            if (!secondName.isEmpty())
            {
                isEmptyString = false;
                QString textQuerySecondNameUpdate = "UPDATE Workers SET surname = '" + secondName + "' WHERE id = '" + id + "'";
                QSqlQuery querySecondNameUpdate;
                querySecondNameUpdate.exec(textQuerySecondNameUpdate);
            }

            QString name = ui->nameUpdate_2->text();
            if (!name.isEmpty())
            {
                isEmptyString = false;
                QString textQueryNameUpdate = "UPDATE Workers SET name = '" + name + "' WHERE id = '" + id + "'";
                QSqlQuery queryNameUpdate;
                queryNameUpdate.exec(textQueryNameUpdate);
            }

            QString patronymic = ui->patronymicUpdate_2->text();
            if (!patronymic.isEmpty())
            {
                isEmptyString = false;
                QString textQueryPatronymicUpdate = "UPDATE Workers SET patronymic = '" + patronymic + "' WHERE id = '" + id + "'";
                QSqlQuery queryPatronymicUpdate;
                queryPatronymicUpdate.exec(textQueryPatronymicUpdate);
            }

            QString phone = ui->phoneUpdate_2->text();
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
                    QString textQueryPhoneUpdate = "UPDATE Workers SET phone_number = '" + phone + "' WHERE id = '" + id + "'";
                    QSqlQuery queryPhoneUpdate;
                    queryPhoneUpdate.exec(textQueryPhoneUpdate);
                }
            }

            QString login = ui->loginUpdate_2->text();
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
                    QString textOfQuerySelectIdLoginDetails = "SELECT id_login_details FROM Workers WHERE id = '" + id + "'";
                    QSqlQuery querySelectIdLoginDetails;
                    querySelectIdLoginDetails.exec(textOfQuerySelectIdLoginDetails);
                    querySelectIdLoginDetails.next();
                    QString idLoginDetails = querySelectIdLoginDetails.value(0).toString();

                    QString textQueryLoginUpdate = "UPDATE Login_Details SET login = '" + login + "' WHERE id = '" + idLoginDetails + "'";
                    QSqlQuery queryLoginUpdate;
                    queryLoginUpdate.exec(textQueryLoginUpdate);
                }
            }

            QString password = ui->passwordUpdate_2->text();
            if (!password.isEmpty())
            {
                isEmptyString = false;

                //Выбрать id_login_details
                QString textOfQuerySelectIdLoginDetails = "SELECT id_login_details FROM Workers WHERE id = '" + id + "'";
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

            QString salary = ui->salaryUpdate_2->text();
            if (!salary.isEmpty())
            {
                isEmptyString = false;
                QString textQuerySalaryUpdate = "UPDATE Workers SET salary = '" + salary + "' WHERE id = '" + id + "'";
                QSqlQuery querySalaryUpdate;
                querySalaryUpdate.exec(textQuerySalaryUpdate);
            }

            if (isEmptyString)
            {
                QMessageBox::warning(this, "Ошибка", "Заполните поля, которые нужно изменить");
            }
            else
            {
                //Обновление таблицы
                QSqlTableModel *mastersView = new QSqlTableModel(this, dataBase);
                mastersView->setTable("MastersLogin_Details");
                ui->mastersView->setModel(mastersView);
                mastersView->select();
                ui->mastersView->resizeColumnsToContents();

                if (!isNotChanged)
                {
                QMessageBox::information(this, "Успех", "Пользователь успешно обновлен");
                }
            }
        }
    }
}

//Добавление менеджера
void Owner::on_pushButton_7_clicked()
{
    //Проверка на пустые поля
    bool isEmptyString = false;

    QString secondName = ui->secondNameInsert_3->text();
    if (!isEmptyString)
    {
        isEmptyString = secondName.isEmpty();
    }

    QString name = ui->nameInsert_3->text();
    if (!isEmptyString)
    {
        isEmptyString = name.isEmpty();
    }

    QString patronymic = ui->patronymicInsert_3->text();

    QString phone = ui->phoneInsert_3->text();
    if (!isEmptyString)
    {
        isEmptyString = phone.isEmpty();
    }

    QString login = ui->loginInsert_3->text();
    if (!isEmptyString)
    {
        isEmptyString = login.isEmpty();
    }
    QString password = ui->passwordInsert_3->text();
    if (!isEmptyString)
    {
        isEmptyString = password.isEmpty();
    }

    QString salary = ui->salaryInsert_3->text();
    if (!isEmptyString)
    {
        isEmptyString = salary.isEmpty();
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

                //Добавить данные в таблицу Masters
                QString textOfQueryInsert = "INSERT INTO Workers (id_position, [name], surname, patronymic, phone_number, id_login_details, salary) VALUES ('2', '" +
                                           name + "', '" + secondName + "', '" + patronymic + "', '" + phone + "', '" + idLoginDetails + "', '" + salary + "')";

                QSqlQuery queryInsert;
                queryInsert.exec(textOfQueryInsert);

                //Обновление таблицы
                QSqlTableModel *managersView = new QSqlTableModel(this, dataBase);
                managersView->setTable("ManagersLogin_Details");
                ui->managersView->setModel(managersView);
                managersView->select();
                ui->managersView->resizeColumnsToContents();

                QMessageBox::information(this, "Успех", "Пользователь успешно добавлен");
            }
        }
    }
}

//Изменить менеджера
void Owner::on_pushButton_8_clicked()
{
    //Не изменились заполненные поля
    bool isNotChanged = false;

    //Проверка на пустое поле id
    QString id = ui->idUpdate_3->text();
    if (id.isEmpty())
    {
         QMessageBox::warning(this, "Ошибка", "Введите id пользователя");
    }
    else
    {
        //Проверка, есть ли такой id
        QString textQueryCheckId = "SELECT * FROM Workers WHERE id = '" + id + "'";
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

            QString secondName = ui->secondNameUpdate_3->text();
            if (!secondName.isEmpty())
            {
                isEmptyString = false;
                QString textQuerySecondNameUpdate = "UPDATE Workers SET surname = '" + secondName + "' WHERE id = '" + id + "'";
                QSqlQuery querySecondNameUpdate;
                querySecondNameUpdate.exec(textQuerySecondNameUpdate);
            }

            QString name = ui->nameUpdate_3->text();
            if (!name.isEmpty())
            {
                isEmptyString = false;
                QString textQueryNameUpdate = "UPDATE Workers SET name = '" + name + "' WHERE id = '" + id + "'";
                QSqlQuery queryNameUpdate;
                queryNameUpdate.exec(textQueryNameUpdate);
            }

            QString patronymic = ui->patronymicUpdate_3->text();
            if (!patronymic.isEmpty())
            {
                isEmptyString = false;
                QString textQueryPatronymicUpdate = "UPDATE Workers SET patronymic = '" + patronymic + "' WHERE id = '" + id + "'";
                QSqlQuery queryPatronymicUpdate;
                queryPatronymicUpdate.exec(textQueryPatronymicUpdate);
            }

            QString phone = ui->phoneUpdate_3->text();
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
                    QString textQueryPhoneUpdate = "UPDATE Workers SET phone_number = '" + phone + "' WHERE id = '" + id + "'";
                    QSqlQuery queryPhoneUpdate;
                    queryPhoneUpdate.exec(textQueryPhoneUpdate);
                }
            }

            QString login = ui->loginUpdate_3->text();
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
                    QString textOfQuerySelectIdLoginDetails = "SELECT id_login_details FROM Workers WHERE id = '" + id + "'";
                    QSqlQuery querySelectIdLoginDetails;
                    querySelectIdLoginDetails.exec(textOfQuerySelectIdLoginDetails);
                    querySelectIdLoginDetails.next();
                    QString idLoginDetails = querySelectIdLoginDetails.value(0).toString();

                    QString textQueryLoginUpdate = "UPDATE Login_Details SET login = '" + login + "' WHERE id = '" + idLoginDetails + "'";
                    QSqlQuery queryLoginUpdate;
                    queryLoginUpdate.exec(textQueryLoginUpdate);
                }
            }

            QString password = ui->passwordUpdate_3->text();
            if (!password.isEmpty())
            {
                isEmptyString = false;

                //Выбрать id_login_details
                QString textOfQuerySelectIdLoginDetails = "SELECT id_login_details FROM Workers WHERE id = '" + id + "'";
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

            QString salary = ui->salaryUpdate_3->text();
            if (!salary.isEmpty())
            {
                isEmptyString = false;
                QString textQuerySalaryUpdate = "UPDATE Workers SET salary = '" + salary + "' WHERE id = '" + id + "'";
                QSqlQuery querySalaryUpdate;
                querySalaryUpdate.exec(textQuerySalaryUpdate);
            }

            if (isEmptyString)
            {
                QMessageBox::warning(this, "Ошибка", "Заполните поля, которые нужно изменить");
            }
            else
            {
                //Обновление таблицы
                QSqlTableModel *managersView = new QSqlTableModel(this, dataBase);
                managersView->setTable("ManagersLogin_Details");
                ui->managersView->setModel(managersView);
                managersView->select();
                ui->managersView->resizeColumnsToContents();

                if (!isNotChanged)
                {
                QMessageBox::information(this, "Успех", "Пользователь успешно обновлен");
                }
            }
        }
    }
}

//Добавление услуги
void Owner::on_pushButton_10_clicked()
{
    //Проверка на пустые поля
    bool isEmptyString = false;

    QString name = ui->nameInsert_4->text();
    if (!isEmptyString)
    {
        isEmptyString = name.isEmpty();
    }

    QString price = ui->priceInsert_4->text();
    if (!isEmptyString)
    {
        isEmptyString = price.isEmpty();
    }

    if (isEmptyString)
    {
        QMessageBox::warning(this, "Ошибка", "Заполните все обязательные поля");
    }
    else
    {
        //Проверка на уникальность названия
        QString textOfQueryCheckName = "SELECT * FROM Services WHERE Services.name = '" +
                name + "'";
        QSqlQuery queryCheckName;
        queryCheckName.exec(textOfQueryCheckName);

        if (queryCheckName.first())
        {
             QMessageBox::warning(this, "Ошибка", "Услуга с таким именем уже существует");
        }
        else
        {
            //Проверка на отрицательную стоимость
            if (price < 0)
            {
                 QMessageBox::warning(this, "Ошибка", "Цена услуги не может быть отрицаетльной");
            }
            else
            {
                //Добавить данные в таблицу Services
                QString textOfQueryInsert = "INSERT INTO Services (name, price) VALUES ('" +
                                           name + "', '" + price + "')";

                QSqlQuery queryInsert;
                queryInsert.exec(textOfQueryInsert);

                //Обновление таблицы
                QSqlTableModel *listServices = new QSqlTableModel(this, dataBase);
                listServices->setTable("Services");
                ui->listServices->setModel(listServices);
                listServices->select();
                ui->listServices->resizeColumnsToContents();

                QMessageBox::information(this, "Успех", "Услуга успешно добавлена");
            }
        }
    }
}

//Изменение услуги
void Owner::on_pushButton_11_clicked()
{
    //Не изменились заполненные поля
    bool isNotChanged = false;

    //Проверка на пустое поле id
    QString id = ui->idUpdate_4->text();
    if (id.isEmpty())
    {
         QMessageBox::warning(this, "Ошибка", "Введите id услуги");
    }
    else
    {
        //Проверка, есть ли такой id
        QString textQueryCheckId = "SELECT * FROM Services WHERE id = '" + id + "'";
        QSqlQuery queryCheckId;
        queryCheckId.exec(textQueryCheckId);
        if (!queryCheckId.first())
        {
            QMessageBox::warning(this, "Ошибка", "Услуга с таким id не найдена");
        }
        else
        {
            //Проверка на пустые поля
            bool isEmptyString = true;

            QString name = ui->nameUpdate_4->text();
            if (!name.isEmpty())
            {
                isEmptyString = false;
                //Проверка на уникальность названия
                QString textOfQueryCheckName = "SELECT * FROM Services WHERE Services.name = '" +
                        name + "'";
                QSqlQuery queryCheckName;
                queryCheckName.exec(textOfQueryCheckName);

                if (queryCheckName.first())
                {
                     QMessageBox::warning(this, "Ошибка", "Услуга с таким именем уже существует");
                     isNotChanged = true;
                }
                else
                {
                    QString textQueryNameUpdate = "UPDATE Services SET name = '" + name + "' WHERE id = '" + id + "'";
                    QSqlQuery queryNameUpdate;
                    queryNameUpdate.exec(textQueryNameUpdate);
                }
            }

            QString price = ui->priceUpdate_4->text();
            if (!price.isEmpty())
            {
                //Проверка на отрицательную стоимость
                if (price < 0)
                {
                     QMessageBox::warning(this, "Ошибка", "Цена услуги не может быть отрицаетльной");
                }
                else
                {
                    isEmptyString = false;
                    QString textQueryPriceUpdate = "UPDATE Services SET price = '" + price + "' WHERE id = '" + id + "'";
                    QSqlQuery queryPriceUpdate;
                    queryPriceUpdate.exec(textQueryPriceUpdate);
                }
            }

            if (isEmptyString)
            {
                QMessageBox::warning(this, "Ошибка", "Заполните поля, которые нужно изменить");
            }
            else
            {
                //Обновление таблицы
                QSqlTableModel *listServices = new QSqlTableModel(this, dataBase);
                listServices->setTable("Services");
                ui->listServices->setModel(listServices);
                listServices->select();
                ui->listServices->resizeColumnsToContents();

                QSqlQueryModel *queryTopServices = new QSqlQueryModel;
                QString textQueryTopServices = "SELECT TOP 10 [Services].[name], COUNT(Order_List.id) AS [number of uses] FROM [Services] JOIN Order_List ON [Services].id = Order_List.id_service GROUP BY [Services].[name] ORDER BY COUNT(Order_List.id) DESC";
                queryTopServices->setQuery(textQueryTopServices);
                QTableView *topServices = new QTableView();
                topServices->setModel(queryTopServices);
                ui->topServices->setModel(queryTopServices);
                ui->topServices->resizeColumnsToContents();

                if (!isNotChanged)
                {
                QMessageBox::information(this, "Успех", "Услуга успешно обновлена");
                }
            }
        }
    }
}
