#include "handleloging.h"
#include "ui_handleloging.h"

HandleLoging::HandleLoging(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HandleLoging)
{
    ui->setupUi(this);
    ui->logsText->hide();
}

HandleLoging::~HandleLoging()
{
    delete ui;

    if(db.isOpen()) db.close();
}

void HandleLoging::getUserEntries()
{
    createDatabase();

    username = ui->loginUname->text().trimmed();
    password = ui->loginPwd->text().trimmed();
}

void HandleLoging::on_login_PushButton_clicked()
{
    getUserEntries();

    bool success =  getUserCredentials(username, password);

    if(success)
    {
        emit userLoginValid();
    }
    else
    {
        ui->logsText->show();
        ui->logsText->append("Wrong username or password.\nTry again!");
    }
}

void HandleLoging::on_signupButton_clicked()
{
    getUserEntries();
    QString verifyPassword = "";
    verifyPassword  = ui->loginPwd->text().trimmed();

    if(!password.isEmpty() && firstClick) {

        firstEntry = password;
        ui->loginPwd->clear();
        ui->logsText->show();
        ui->logsText->append("Re-enter password to verify");
        firstClick = false;
    }
    else if(verifyPassword == firstEntry && !firstClick) {

        addNewUserEntry(username, password);
        firstClick = true;
    }
    else if (verifyPassword != firstEntry && !firstClick) {

        ui->logsText->show();
        ui->logsText->append("Password did not match, try again!");
        firstClick = true;
    }
}

void HandleLoging::createDatabase()
{
    if(!db.isOpen())
        db.open();

    if(db.tables().contains( QLatin1String("user")))
        return;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Applications/iPlayMovie.app/Contents/MacOS/:crendentials.db:");

    //QSqlQuery query(db);
    //query.exec("DROP table user");

    if (!db.open())
    {
        QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
                              QObject::tr("Unable to establish a database connection.\n"
                                          "Click Cancel to exit."), QMessageBox::Cancel);
        return;
    }

    QSqlQuery query(db);
    query.exec("create table IF NOT EXISTS user ("
               "uname varchar(20) primary key, pwd varchar(20))");

    db.commit();
}

bool HandleLoging::getUserCredentials(QString uname, QString pwd)
{
    if(uname.isEmpty() || pwd.isEmpty()) return false;

    QSqlQuery query(db);
    query.exec("SELECT uname, pwd FROM user WHERE uname =  '" + uname + "' AND pwd =  '" + pwd + "'");

    // returns true if matching credentials was found
    return (query.next());
}

void HandleLoging::addNewUserEntry(QString uname, QString pwd)
{
    if(uname.isEmpty() || pwd.isEmpty()) return;

    QSqlQuery query(db);
    query.prepare("INSERT INTO user (uname, pwd)"
                  "VALUES (?, ?)");

    query.addBindValue(uname);
    query.addBindValue(pwd);

    if(!query.exec())
    {
        QString error = query.lastError().text();

        if(error.contains(QString(USER_EXISTS))) {

            ui->logsText->show();
            ui->logsText->append("User already exist, try a different one.");
        }
        return;
    }

    db.commit();
}
