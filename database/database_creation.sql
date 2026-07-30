USE master
GO

CREATE DATABASE car_service 
ON
(Name=car_service,
FileName='C:\Program Files\Microsoft SQL Server\MSSQL15.MSSQLSERVER\MSSQL\DATA\car_service.mdf'
)
LOG 
ON
(Name=car_service_log,
FileName='C:\Program Files\Microsoft SQL Server\MSSQL15.MSSQLSERVER\MSSQL\DATA\car_service.ldf'
)
GO

ALTER DATABASE car_service
SET AUTO_SHRINK 
ON
GO

ALTER DATABASE car_service
MODIFY FILE(name=car_service, maxsize=100MB)
GO

