USE car_service
GO

--1.Таблица Cars
CREATE TABLE [dbo].[Cars](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[id_owner] [int] NOT NULL,
	[number] [varchar](10) NOT NULL,
	[brand] [varchar](30) NOT NULL,
	[model] [varchar](30) NULL,
	[color] [varchar](30) NULL,
 CONSTRAINT [PK_cars] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO

--2.Таблица Clients
CREATE TABLE [dbo].[Clients](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[name] [varchar](30) NOT NULL,
	[surname] [varchar](30) NOT NULL,
	[patronymic] [varchar](30) NULL,
	[phone_number] [varchar](20) NOT NULL,
	[id_login_details] [int] NOT NULL,
 CONSTRAINT [PK_clients] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO

--3.Таблица Login_Details
CREATE TABLE [dbo].[Login_Details](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[login] [varchar](30) NOT NULL,
	[password] [varchar](255) NOT NULL,
 CONSTRAINT [PK_login_details] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO

--4.Таблица Order_List
CREATE TABLE [dbo].[Order_List](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[id_service] [int] NOT NULL,
	[id_order] [int] NOT NULL,
 CONSTRAINT [PK_spares_services] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO

--5.Таблица Orders
CREATE TABLE [dbo].[Orders](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[id_car] [int] NOT NULL,
	[status] [varchar](30) NOT NULL,
	[id_manager] [int] NOT NULL,
	[id_master] [int] NOT NULL,
	[creation_date] [date] NOT NULL,
	[expiration_date] [date] NULL,
 CONSTRAINT [PK_orders] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO

--6.Таблица Owner
CREATE TABLE [dbo].[Owner](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[name] [varchar](30) NOT NULL,
	[surname] [varchar](30) NOT NULL,
	[patronymic] [varchar](30) NULL,
	[phone_number] [varchar](20) NOT NULL,
	[id_login_details] [int] NOT NULL,
 CONSTRAINT [PK_owner] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO

--7.Таблица Positions
CREATE TABLE [dbo].[Positions](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[position] [varchar](30) NOT NULL,
 CONSTRAINT [PK_positions] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO

--8.Таблица Services
CREATE TABLE [dbo].[Services](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[name] [varchar](50) NOT NULL,
	[price] [int] NOT NULL,
 CONSTRAINT [PK_catalog] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO

--9.Таблица Spares
CREATE TABLE [dbo].[Spares](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[name] [varchar](30) NOT NULL,
	[code] [varchar](30) NOT NULL,
	[price] [money] NOT NULL,
	[quantity] [int] NOT NULL,
 CONSTRAINT [PK_spares] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO

--10.Таблица Used_Spares
CREATE TABLE [dbo].[Used_Spares](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[id_spare] [int] NOT NULL,
	[id_order_list] [int] NOT NULL,
 CONSTRAINT [PK_Service_Spares] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO

--11.Таблица Workers
CREATE TABLE [dbo].[Workers](
	[id] [int] IDENTITY(1,1) NOT NULL,
	[id_position] [int] NOT NULL,
	[name] [varchar](30) NOT NULL,
	[surname] [varchar](30) NOT NULL,
	[patronymic] [varchar](30) NULL,
	[phone_number] [varchar](20) NOT NULL,
	[id_login_details] [int] NOT NULL,
	[salary] [money] NOT NULL,
 CONSTRAINT [PK_masters] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO