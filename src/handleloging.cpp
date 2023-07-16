#include "handleloging.h"
#include "ui_handleloging.h"
#include "text.h"


HandleLoging::HandleLoging(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HandleLoging)
{
    ui->setupUi(this);
    createDatabase();
}

HandleLoging::~HandleLoging()
{
    delete ui;

    if (db.isOpen())
        db.close();
}

void HandleLoging::loginUSer()
{
    if (!getUserEntries())
        return;

    bool success =  getUserCredentials(username, password);

    if (success)
    {
        emit userLoginValid();
    }
    else
    {
        displayMessage("Wrong username or password.\nTry again!", QtWarningMsg);
    }
}

void HandleLoging::displayMessage(const QString &msg, const int &msgtype)
{
    switch (msgtype)
    {
    case QtCriticalMsg:
        QMessageBox::critical(this, "", msg, QMessageBox::Ok);
        break;

    case QtInfoMsg:
        QMessageBox::information(this, "", msg, QMessageBox::Ok);
        break;

    case QtWarningMsg:
        QMessageBox::warning(this, "", msg, QMessageBox::Ok);
        break;

    default:
        break;
    }
}

bool HandleLoging::getUserEntries()
{
    username = ui->loginUname->text().trimmed();
    password = ui->loginPwd->text().trimmed();

    if (username.isEmpty() || password.isEmpty())
    {
        displayMessage("Invalid entries. Username or password is missing!", QtWarningMsg);
        return false;
    }

    return true;
}

void HandleLoging::on_login_PushButton_clicked()
{
    loginUSer();
}

void HandleLoging::on_signupButton_clicked()
{
    if (!getUserEntries())
        return;

    QString verifyPassword = "";
    verifyPassword  = ui->loginPwd->text().trimmed();

    if (firstClick)
    {
        if (!password.isEmpty())
        {
            firstEntry = password;
            displayMessage("Re-enter password to verify", QtInfoMsg);
            firstClick = false;
        }
    }
    else
    {
        if (verifyPassword == firstEntry)
        {
            addNewUserEntry(username, password);
        }
        else
        {
            displayMessage("Password did not match, try again!", QtWarningMsg);
        }

        firstClick = true;
    }
}

void HandleLoging::createDatabase()
{
    if (!db.isOpen())
        db.open();

    if (db.tables().contains(QLatin1String("user")))
        return;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Applications/iPlayMovie.app/Contents/MacOS/:crendentials.db:");

    //QSqlQuery query(db);
    //query.exec("DROP table user");

    if (!db.open())
    {
        displayMessage("Unable to establish a database connection.\nClick Ok to exit.", QtCriticalMsg);
        return;
    }

    QSqlQuery query(db);
    query.exec("create table IF NOT EXISTS user ("
               "uname varchar(20) primary key, pwd varchar(20))");

    db.commit();
}

bool HandleLoging::getUserCredentials(QString uname, QString pwd)
{
    QSqlQuery query(db);
    query.exec("SELECT uname, pwd FROM user WHERE uname =  '" + uname + "' AND pwd =  '" + pwd + "'");

    // returns true if matching credentials were found
    return query.next();
}

void HandleLoging::addNewUserEntry(QString uname, QString pwd)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO user (uname, pwd)"
                  "VALUES (?, ?)");

    query.addBindValue(uname);
    query.addBindValue(pwd);

    if (!query.exec())
    {
        QString error = query.lastError().text();

        if (error.contains(QString(USER_EXISTS)))
        {
            displayMessage("Username already exists, try a different one.", QtWarningMsg);
        }

        return;
    }

    db.commit();
}

void HandleLoging::on_loginPwd_returnPressed()
{
    loginUSer();
}
