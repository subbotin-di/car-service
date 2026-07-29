USE car_service
GO

--1.Изменение учетных записей в таблице Clients
UPDATE Clients 
SET id_login_details = '2' 
WHERE id_login_details = '1'
GO

--2.Изменение учетных записей в таблице Workers
UPDATE Workers
SET id_login_details = '2' 
WHERE id_login_details = '1'
GO

--3.Изменение обращений в таблице Orders
UPDATE Orders
SET [status] = 'завершено' 
WHERE id = '1'
GO

--4.Изменение информации об услугах в таблице Services
UPDATE [Services]
SET price = '2000' 
WHERE id = '1'
GO

--5.Изменение информации о запчастях в таблице Spares
UPDATE [Services]
SET price = '2000' 
WHERE id = '1'
GO

--6.Изменение информации об автомобилях в таблице Cars
UPDATE Cars
SET color = 'черный' 
WHERE id = '1'
GO

--7. Изменение пароля в таблице Login_Details
OPEN SYMMETRIC KEY symmetric_key
DECRYPTION BY CERTIFICATE certificate;
UPDATE Login_Details SET password = EncryptByKey(Key_GUID('symmetric_key'),'" + password + "') 
WHERE id = '" + idLoginDetails + "'

--8. Изменение количества запчастей при добавлении
UPDATE Spares
SET quantity = quantity - 1 
WHERE id = '" + spare + "'
GO