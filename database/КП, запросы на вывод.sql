USE car_service
GO

--1.Вывод информации об услугах из таблицы Services
SELECT * 
FROM [Services]
GO

--2.Вывод информации о запчастях из таблицы Spares
SELECT * 
FROM Spares
GO

--3.Вывод информации об обращениях из таблицы Orders
SELECT * 
FROM Orders
GO

--4.Вывод информации об автомобиле из таблицы Cars
SELECT * 
FROM Cars
GO

--5.Вывод информации из таблицы Login_Details
OPEN SYMMETRIC KEY symmetric_key
DECRYPTION BY CERTIFICATE certificate;
SELECT * FROM Login_Details WHERE login = '" + login + "' 
AND convert(char,DecryptByKey(password)) = '" + password + "'
