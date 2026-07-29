--1.Добавить нового клиента
CREATE PROCEDURE insert_client (
	@login VARCHAR(30),
	@password VARCHAR(30),
	@name VARCHAR(30),
	@surname VARCHAR(30),
	@patronymic VARCHAR(30),
	@phone_number VARCHAR(20))
AS 
BEGIN
	INSERT INTO Login_Details (
		login,
		password)
	VALUES (
		@login,
		@password)

	INSERT INTO Clients (
		name,
		surname,
		patronymic,
		phone_number,
		id_login_details)
	VALUES (
		@name,
		@surname,
		@patronymic,
		@phone_number,
		SCOPE_IDENTITY())
END
GO

EXEC insert_client
'12345',
'12345',
'Иван',
'Иванов',
'Иванович',
'+79123456789'
GO


--2.Создать заказ
CREATE PROCEDURE create_order (
@car_id INT,
@manager_id INT,
@master_id INT,
@service_id INT)
AS
BEGIN
	INSERT INTO Orders (
		id_car,
		status,
		id_manager,
		id_master,
		creation_date)
	VALUES (
	@car_id,
	'выполняется',
	@manager_id,
	@master_id,
	GETDATE())

	INSERT INTO Order_List (
		id_service,
		id_order)
	VALUES (
		@service_id,
		SCOPE_IDENTITY())
END
GO

EXEC create_order
1,
7,
4,
5
GO