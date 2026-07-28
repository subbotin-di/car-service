#ifndef MANAGER_H
#define MANAGER_H

#include <QWidget>
#include <QtSql>

namespace Ui {
class Manager;
}

class Manager : public QWidget
{
    Q_OBJECT

public:
    explicit Manager(QWidget *parent = nullptr, QString id = nullptr);
    ~Manager();
    QString idManager;

protected:
    QSqlDatabase dataBase;

private slots:
    void on_pushButton_29_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_30_clicked();

    void on_pushButton_31_clicked();

    void on_pushButton_32_clicked();

private:
    Ui::Manager *ui;
};

#endif // MANAGER_H
