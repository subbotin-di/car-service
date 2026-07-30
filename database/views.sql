USE car_service
GO

--1. View of positions and workers
CREATE VIEW WorkersPositions 
AS
SELECT Workers.[name], 
Workers.surname, 
Workers.patronymic, 
Positions.position
FROM Workers
JOIN Positions ON Workers.id_position = Positions.id
GO

--2. View of cars and clients
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

--3. View of client logins
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

--4. View of manager logins (id_position = 1)
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

--5. View of worker logins (id_position = 2)
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