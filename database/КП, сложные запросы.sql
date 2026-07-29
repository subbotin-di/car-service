USE car_service
GO

----5.Вывод списка всех менеджеров, который отсортирован в порядке убывания количества созданных обращений
--SELECT Workers.surname,
--Workers.[name],
--Workers.patronymic,
--COUNT(Orders.id) AS [number of orders]
--FROM Workers
--JOIN Orders ON Workers.id = Orders.id_manager
--GROUP BY Workers.[name], Workers.surname, Workers.patronymic
--ORDER BY COUNT(Orders.id) DESC
--GO

----6.Вывод списка всех клиентов, который отсортирован в порядке убывания количества обращений
--SELECT Clients.surname,
--Clients.[name],
--Clients.patronymic,
--COUNT(Orders.id) AS [number of orders]
--FROM Clients
--JOIN Cars ON Clients.id = Cars.id_owner
--JOIN Orders ON Cars.id = Orders.id_car
--GROUP BY Clients.surname, Clients.[name], Clients.patronymic
--ORDER BY COUNT(Orders.id) DESC
--GO

----8.Вывод списка клиентов, автомобили которых не ремонтировались больше года
--SELECT Clients.surname,
--Clients.[name],
--Clients.patronymic,
--Orders.expiration_date AS [completion date]
--FROM Clients
--JOIN Cars ON Clients.id = Cars.id_owner
--JOIN Orders ON Cars.id = Orders.id_car
--WHERE (Orders.[status] = 'завершен' and DATEDIFF(day, Orders.expiration_date, CONVERT (date, GETDATE())) >= 365)
--GO

----15. Выбрать мастеров, у которых статус заказа 'выполняется'
--SELECT *
--FROM Orders, Workers
--WHERE Orders.id_master = Workers.id
--AND Workers.id = '" + id + "'
--AND Orders.status = 'выполняется'

----16. Выбрать услуги, которые есть в выполняемом заказе
--SELECT *
--FROM Order_List, Orders 
--WHERE Orders.id = Order_List.id_order
--AND Order_List.id_order = '" + id + "'
--AND Orders.status = 'выполняется'

----17. Выбрать запчасти, которые есть в выполняемом заказе
--SELECT *
--FROM Used_Spares, Orders, Order_List
--WHERE Orders.id = Order_List.id_order
--AND Order_List.id = Used_Spares.id_order_list
--AND Used_Spares.id_spare = '" + id + "'
--AND Orders.status = 'выполняется'

----14. Выбрать клиентов, у которых статус заказа 'выполняется'
--SELECT *
--FROM Orders, Cars, Clients
--WHERE Orders.id_car = Cars.id
--AND Cars.id_owner = Clients.id
--AND Clients.id = '" + id + "'
--AND Orders.status = 'выполняется'

----18. Выбрать автомобили, которые есть в выполняемом заказе
--SELECT *
--FROM Orders, Cars
--WHERE Orders.id_car = Cars.id
--AND Cars.id = '" + id + "'
--AND Orders.status = 'выполняется'

--1.Вывод списка 10 наиболее популярных услуг
SELECT TOP 10 [Services].[name],
COUNT(Order_List.id) AS [number of uses]
FROM [Services]
JOIN Order_List 
ON [Services].id = Order_List.id_service
GROUP BY [Services].[name]
ORDER BY COUNT(Order_List.id) DESC
GO

--2.Вывод оборота денежных средств за год
SELECT SUM([Services].price) + 
	(SELECT SUM(Spares.price)
	FROM Spares
	JOIN Used_Spares 
	ON Spares.id = Used_Spares.id_spare
	JOIN Order_List 
	ON Used_Spares.id_order_list = Order_List.id
	JOIN Orders 
	ON Order_List.id_order = Orders.id 
	WHERE DATEDIFF(day, Orders.expiration_date, CONVERT (date, GETDATE())) <= 365)
AS [cash turnover per year]
FROM [Services]
JOIN Order_List 
ON [Services].id = Order_List.id_service
JOIN Orders 
ON Order_List.id_order = Orders.id 
WHERE DATEDIFF(day, Orders.expiration_date, CONVERT (date, GETDATE())) <= 365
GO

--3. Вывод список обращений, которыми в данный момент занимается определенный мастер
SELECT Orders.id,
Cars.number,
Orders.status,
Orders.creation_date 
FROM Orders, Workers, Cars
WHERE Orders.id_master = Workers.id
AND Orders.id_car = Cars.id
AND Workers.id = '" + idMaster + "'
AND Orders.status = 'выполняется'
GO

--4. Вывод списка использованных запчастей для каждой услуги
SELECT Used_Spares.id,
Orders.id,
Services.name,
Order_List.id_service,
Spares.name,
Used_Spares.id_spare
FROM Orders
JOIN Order_List
ON Orders.id = Order_List.id_order
JOIN Services
ON Order_List.id_service = Services.id
LEFT JOIN Used_Spares
ON Order_List.id = Used_Spares.id_order_list
FULL JOIN Spares
ON Used_Spares.id_spare = Spares.id
WHERE Orders.status = 'выполняется'
AND Orders.id_master = '" + idMaster + "'
GO

--5. Вывод списка выполняемых заказов
SELECT Orders.*,
Order_List.id,
Order_List.id_service
FROM Orders
LEFT JOIN Order_List
ON Orders.id = Order_List.id_order
WHERE Orders.status = 'выполняется'
GO

--6.Вывод списка всех мастеров, который отсортирован в порядке убывания количества отремонтированных машин
SELECT Workers.id, 
Workers.surname, 
Workers.[name], 
Workers.patronymic, 
COUNT(Orders.id) AS [number of cars] 
FROM Workers 
JOIN Orders ON Workers.id = Orders.id_master 
WHERE Orders.status = 'выполняется' 
GROUP BY Workers.id, Workers.[name], Workers.surname, Workers.patronymic 
ORDER BY COUNT(Orders.id) DESC 
GO

--7.Вывод списка мастеров, которые не ремонтируют ни один автомобиль
SELECT Workers.id, 
Workers.surname, 
Workers.[name], 
Workers.patronymic
FROM Workers
WHERE NOT EXISTS 
	(SELECT *
	FROM Orders
	WHERE Workers.id = Orders.id_master
	AND Orders.status = 'выполняется')
AND Workers.id_position = '1'
GO

--8.Вывод списка обращений, которые поступили от определенного клиента
SELECT Orders.id,
Services.name,
Orders.id_car,
Orders.id_manager,
Orders.id_master,
Orders.creation_date,
Orders.expiration_date,
Orders.status
FROM Clients
JOIN Cars 
ON Clients.id = Cars.id_owner
JOIN Orders 
ON Cars.id = Orders.id_car 
JOIN Order_List
ON Orders.id = Order_List.id_order
JOIN Services
ON Order_List.id_service = Services.id
WHERE Clients.id = '" + idClient + "'
GO

--9.Вывод списка всех автомобилей, которыми владеет определенный клиент
SELECT Cars.number,
Cars.brand,
Cars.model,
Cars.color
FROM Clients
JOIN Cars ON Clients.id = Cars.id_owner
WHERE Clients.id = '" + idClient + "'
GO

--10.Вывод списка всех клиентов, который отсортирован в порядке убывания количества обращений
SELECT Clients.surname,
Clients.[name],
Clients.patronymic,
COUNT(Orders.id) AS [number of orders]
FROM Clients
JOIN Cars ON Clients.id = Cars.id_owner
JOIN Orders ON Cars.id = Orders.id_car
GROUP BY Clients.surname, Clients.[name], Clients.patronymic
ORDER BY COUNT(Orders.id) DESC
GO