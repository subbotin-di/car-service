#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QtSql>

namespace Ui {
class Client;
}

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr, QString id = nullptr);
    ~Client();
    QString idClient;

protected:
    QSqlDatabase dataBase;

private:
    Ui::Client *ui;
};

#endif // CLIENT_H
