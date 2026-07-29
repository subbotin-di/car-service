USE car_service
GO

--1.Добавление учетных записей в таблицу Clients
INSERT INTO Clients ([name], surname, patronymic, phone_number, id_login_details) 
VALUES ('test_name', 'test_surname', 'test_patronymic', '+79000000000', '15')
GO

--2.Добавление учетных записей в таблицу Workers
INSERT INTO Workers (id_position, [name], surname, patronymic, phone_number, id_login_details, salary) 
VALUES ('1', 'test_name', 'test_surname', 'test_patronymic', '+79000000000', '15', '5000')
GO

--3.Добавление обращений в таблицу Orders
INSERT INTO Orders (id_car, [status], id_manager, id_master, creation_date) 
VALUES ('1', 'выполняется', '6', '1', '', '15')
GO

--4.Добавление информации о запчастях в таблицу Spares
INSERT INTO Spares ([name], code, price, quantity)
VALUES ('tast_name', '000000', '1000', '1')
GO

--5.Добавление информации об услугах в таблицу Services
INSERT INTO [Services] ([name], price)
VALUES ('tast_name', '1000')
GO

--6.Добавление информации об услугах в таблицу Login_Details
OPEN SYMMETRIC KEY symmetric_key
DECRYPTION BY CERTIFICATE certificate;
INSERT INTO Login_Details ([login], [password])
VALUES ('" + login + "', EncryptByKey(Key_GUID('symmetric_key'),'" + password + "'))