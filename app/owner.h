#ifndef OWNER_H
#define OWNER_H

#include <QWidget>
#include <QtSql>


namespace Ui {
class Owner;
}

class Owner : public QWidget
{
    Q_OBJECT

protected:
    QSqlDatabase dataBase;

public:
    explicit Owner(QWidget *parent = nullptr);
    ~Owner();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

private:
    Ui::Owner *ui;
};

#endif // OWNER_H
