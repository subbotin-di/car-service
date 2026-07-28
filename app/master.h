#ifndef MASTER_H
#define MASTER_H

#include <QWidget>
#include <QtSql>

namespace Ui {
class Master;
}

class Master : public QWidget
{
    Q_OBJECT

public:
    explicit Master(QWidget *parent = nullptr, QString id = nullptr);
    ~Master();
    QString idMaster;

protected:
    QSqlDatabase dataBase;

private slots:
    void on_pushButton_25_clicked();

    void on_pushButton_26_clicked();

    void on_pushButton_27_clicked();

    void on_pushButton_28_clicked();

    void on_pushButton_29_clicked();

    void on_pushButton_30_clicked();

private:
    Ui::Master *ui;
};

#endif // MASTER_H
