--1.Ќельз€ завершить обращение, если автомобиль находитс€ в ремонте
CREATE TRIGGER complete_order
ON Orders
AFTER UPDATE
AS
BEGIN
	DECLARE @car_status VARCHAR(30)
	DECLARE @order_status VARCHAR(30)
	DECLARE @order_id INT

	SELECT @order_status = (
		SELECT [status]
		FROM inserted)
	SELECT @car_status = (
		SELECT Cars.status 
		FROM Cars, inserted
		WHERE Cars.id = id_car)
	SELECT @order_id = (
		SELECT id
		FROM inserted)

	IF @order_status = 'завершен' 
	AND @car_status = 'в ремонте'
	BEGIN
		SELECT 'Ќельз€ завершить обращение, если машина находитс€ в ремонте'
		UPDATE Orders
		SET Orders.status = 'выполн€етс€'
		WHERE Orders.id = @order_id
	END
END
GO

UPDATE Orders
SET status = 'завершен'
WHERE id = 2
GO

--2.«авершение заказа не может быть раньше создани€ заказа
CREATE TRIGGER order_date
ON Orders
AFTER UPDATE
AS
BEGIN
	DECLARE @creation_date DATE
	DECLARE @expiration_date DATE
	DECLARE @id INT
	
	SELECT @creation_date = (
		SELECT creation_date 
		FROM inserted)
	SELECT @expiration_date = (
		SELECT expiration_date 
		FROM inserted)
	SELECT @id = (
		SELECT id 
		FROM inserted)

	IF @creation_date > @expiration_date
	BEGIN
		SELECT '«авершение заказа не может быть раньше создани€ заказа'
		UPDATE Orders
		SET Orders.expiration_date = NULL
		WHERE Orders.id = @id
	END
END
GO

UPDATE Orders
SET expiration_date = '2020-09-10'
WHERE id = 2
GO

--3.Ќельз€ добавл€ть запчасть в обращение, если их нет в наличии
CREATE TRIGGER out_of_spares
ON Used_Spares
AFTER INSERT
AS
BEGIN
	DECLARE @quantity INT

	SELECT @quantity = (
		SELECT quantity
		FROM Spares, inserted
		WHERE Spares.id = id_spare)

	IF @quantity = 0 
	BEGIN
		SELECT 'Ќеобходимых запчастей нет в наличии'
		DELETE FROM Used_Spares
		WHERE id = (
			SELECT id
			FROM inserted)
	END
END
GO

INSERT INTO Used_Spares (
	id_spare,
	id_order_list)
VALUES (
	1,
	1)
GO