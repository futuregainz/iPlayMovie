#ifndef HANDLELOGING_H
#define HANDLELOGING_H

#include <QWidget>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace Ui {
class HandleLoging;
}

class HandleLoging : public QWidget
{
    Q_OBJECT

public:
    explicit HandleLoging(QWidget *parent = 0);
    ~HandleLoging();

    bool firstClick = true;
    QString firstEntry = "";
    void displayMessage(QString const &msg, int const &msgtype);

signals:
    void userLoginValid();

private slots:
    void on_login_PushButton_clicked();
    void on_signupButton_clicked();
    void on_loginPwd_returnPressed();

private:
    Ui::HandleLoging *ui;

    QSqlDatabase db;
    QString username;
    QString password;

    void createDatabase();
    bool getUserEntries();
    void loginUSer();
    bool getUserCredentials(QString uname, QString pwd);
    void addNewUserEntry(QString uname, QString pwd);

};

#endif // HANDLELOGING_H
