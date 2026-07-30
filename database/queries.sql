USE car_service
GO

--1.Insert data into tables

INSERT INTO Clients ([name], surname, patronymic, phone_number, id_login_details) 
VALUES ('test_name', 'test_surname', 'test_patronymic', '+79000000000', '15')
GO

INSERT INTO Workers (id_position, [name], surname, patronymic, phone_number, id_login_details, salary) 
VALUES ('1', 'test_name', 'test_surname', 'test_patronymic', '+79000000000', '15', '5000')
GO

INSERT INTO Orders (id_car, [status], id_manager, id_master, creation_date) 
VALUES ('1', 'выполняется', '6', '1', '', '15')
GO

INSERT INTO Spares ([name], code, price, quantity)
VALUES ('tast_name', '000000', '1000', '1')
GO

INSERT INTO [Services] ([name], price)
VALUES ('tast_name', '1000')
GO

OPEN SYMMETRIC KEY symmetric_key
DECRYPTION BY CERTIFICATE certificate;
INSERT INTO Login_Details ([login], [password])
VALUES ('" + login + "', EncryptByKey(Key_GUID('symmetric_key'),'" + password + "'))

--2.Select data from tables

SELECT * 
FROM [Services]
GO

SELECT * 
FROM Spares
GO

SELECT * 
FROM Orders
GO

SELECT * 
FROM Cars
GO

OPEN SYMMETRIC KEY symmetric_key
DECRYPTION BY CERTIFICATE certificate;
SELECT * FROM Login_Details WHERE login = '" + login + "' 
AND convert(char,DecryptByKey(password)) = '" + password + "'

--3.Update data in tables

UPDATE Clients 
SET id_login_details = '2' 
WHERE id_login_details = '1'
GO

UPDATE Workers
SET id_login_details = '2' 
WHERE id_login_details = '1'
GO

UPDATE Orders
SET [status] = 'завершено' 
WHERE id = '1'
GO

UPDATE [Services]
SET price = '2000' 
WHERE id = '1'
GO

UPDATE Spares
SET quantity = quantity - 1 
WHERE id = '" + spare + "'
GO

UPDATE Cars
SET color = 'черный' 
WHERE id = '1'
GO

OPEN SYMMETRIC KEY symmetric_key
DECRYPTION BY CERTIFICATE certificate;
UPDATE Login_Details SET password = EncryptByKey(Key_GUID('symmetric_key'),'" + password + "') 
WHERE id = '" + idLoginDetails + "'

--4.Delete data from tables

DELETE FROM Clients 
WHERE phone_number = '+79000000000'
GO

DELETE FROM Workers 
WHERE phone_number = '+79000000000'
GO

DELETE FROM Spares 
WHERE [name] = 'test_name'
GO

DELETE FROM [Services]
WHERE [name] = 'test_name'
GO