USE car_service
GO

--1.Представление, состоящее из поля position из таблицы Positions и полей name, surname, patronymic из таблицы Workers
CREATE VIEW WorkersPositions 
AS
SELECT Workers.[name], 
Workers.surname, 
Workers.patronymic, 
Positions.position
FROM Workers
JOIN Positions ON Workers.id_position = Positions.id
GO

select * 
from WorkersPositions

--2.Представление, состоящее из полей number, status из таблицы Cars и полей name, surname, patronymic из таблицы Clients
CREATE VIEW ClientsCars
AS
SELECT Clients.[name],
Clients.surname,
Clients.patronymic,
Cars.number,
Cars.[status]
FROM Clients
JOIN Cars ON Clients.id = Cars.id_owner
GO

select *
from ClientsCars

--3.Представление, состоящее из полей login из таблицы Login_Details и полей name, surname, patronymic из таблицы Clients
CREATE VIEW ClientsLogin_Details
AS
SELECT Clients.id,
Clients.[name],
Clients.surname,
Clients.patronymic,
Clients.phone_number,
Login_Details.[login]
FROM Clients
JOIN Login_Details ON Clients.id_login_details = Login_Details.id
GO

select *
from ClientsLogin_Details

--4.Представление, состоящее из полей login из таблицы Login_Details и полей из таблицы Workers, где id_position = 1
CREATE VIEW MastersLogin_Details
AS
SELECT Workers.id,
Workers.[name],
Workers.surname,
Workers.patronymic,
Workers.phone_number,
Login_Details.[login],
Workers.Salary
FROM Workers
JOIN Login_Details ON Workers.id_login_details = Login_Details.id
WHERE Workers.id_position = '1'
GO

select *
from MastersLogin_Details

--5.Представление, состоящее из полей login из таблицы Login_Details и полей из таблицы Workers, где id_position = 2
CREATE VIEW ManagersLogin_Details
AS
SELECT Workers.id,
Workers.[name],
Workers.surname,
Workers.patronymic,
Workers.phone_number,
Login_Details.[login],
Workers.Salary
FROM Workers
JOIN Login_Details ON Workers.id_login_details = Login_Details.id
WHERE Workers.id_position = '2'
GO

select *
from ManagersLogin_Details