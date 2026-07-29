USE car_service
GO

--1.Удаление учетных записей из таблицы Clients
DELETE FROM Clients WHERE phone_number = '+79000000000'
GO

--2.Удаление учетных записей из таблицы Workers
DELETE FROM Workers 
WHERE phone_number = '+79000000000'
GO

--3.Удаление информации о запчастях из таблицы Spares
DELETE FROM Spares 
WHERE [name] = 'test_name'
GO

--4.Удаление информации об услугах из таблицы Services
DELETE FROM [Services]
WHERE [name] = 'test_name'
GO