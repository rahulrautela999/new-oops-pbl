#include "database.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

namespace
{
QString hashPin(const QString &pin)
{
    return QString(QCryptographicHash::hash(pin.toUtf8(), QCryptographicHash::Sha256).toHex());
}
}

DatabaseManager::DatabaseManager()
{
    path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
           + "/bank_management.db";
    db = QSqlDatabase::addDatabase("QSQLITE", "bank_connection");
    db.setDatabaseName(path);
}

DatabaseManager::~DatabaseManager()
{
    if (db.isOpen())
        db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase("bank_connection");
}

bool DatabaseManager::exec(const QString &sql, QString &error) const
{
    QSqlQuery q(db);
    if (!q.exec(sql))
    {
        error = q.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::open(QString &error)
{
    QDir dir(QFileInfo(path).absolutePath());
    if (!dir.exists() && !dir.mkpath("."))
    {
        error = "Unable to create the application data directory.";
        return false;
    }

    if (!db.open())
    {
        error = db.lastError().text();
        return false;
    }

    const QStringList schema = {
        "PRAGMA foreign_keys = ON",
        "CREATE TABLE IF NOT EXISTS admin_users (username TEXT PRIMARY KEY, pin_hash TEXT NOT NULL)",
        "CREATE TABLE IF NOT EXISTS customers ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL COLLATE NOCASE UNIQUE,"
        "phone TEXT, email TEXT, created_at TEXT NOT NULL)",
        "CREATE TABLE IF NOT EXISTS accounts ("
        "account_number INTEGER PRIMARY KEY,"
        "customer_id INTEGER NOT NULL,"
        "type TEXT NOT NULL,"
        "balance REAL NOT NULL DEFAULT 0,"
        "interest_rate REAL NOT NULL DEFAULT 0,"
        "minimum_balance REAL NOT NULL DEFAULT 0,"
        "overdraft_limit REAL NOT NULL DEFAULT 0,"
        "monthly_fee REAL NOT NULL DEFAULT 0,"
        "status TEXT NOT NULL DEFAULT 'Active',"
        "created_at TEXT NOT NULL,"
        "FOREIGN KEY(customer_id) REFERENCES customers(id))",
        "CREATE TABLE IF NOT EXISTS transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "account_number INTEGER NOT NULL,"
        "date_time TEXT NOT NULL,"
        "type TEXT NOT NULL,"
        "description TEXT NOT NULL,"
        "amount REAL NOT NULL,"
        "balance_after REAL NOT NULL,"
        "FOREIGN KEY(account_number) REFERENCES accounts(account_number))",
        "CREATE INDEX IF NOT EXISTS idx_transactions_account ON transactions(account_number)",
        "CREATE INDEX IF NOT EXISTS idx_accounts_customer ON accounts(customer_id)"
    };

    for (const QString &sql : schema)
    {
        if (!exec(sql, error))
            return false;
    }

    QSqlQuery admin(db);
    admin.prepare("INSERT OR IGNORE INTO admin_users(username, pin_hash) VALUES(?, ?)");
    admin.addBindValue("admin");
    admin.addBindValue(hashPin("1234"));
    if (!admin.exec())
    {
        error = admin.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::isOpen() const { return db.isOpen(); }
QString DatabaseManager::databasePath() const { return path; }

bool DatabaseManager::authenticateAdmin(const QString &username, const QString &pin) const
{
    QSqlQuery q(db);
    q.prepare("SELECT pin_hash FROM admin_users WHERE username = ?");
    q.addBindValue(username.trimmed());
    if (!q.exec() || !q.next())
        return false;
    return q.value(0).toString() == hashPin(pin);
}

QList<CustomerRecord> DatabaseManager::customers(QString &error) const
{
    QList<CustomerRecord> result;
    QSqlQuery q(db);
    if (!q.exec("SELECT id, name, COALESCE(phone,''), COALESCE(email,'') FROM customers ORDER BY name COLLATE NOCASE"))
    {
        error = q.lastError().text();
        return result;
    }
    while (q.next())
    {
        CustomerRecord c;
        c.id = q.value(0).toInt();
        c.name = q.value(1).toString();
        c.phone = q.value(2).toString();
        c.email = q.value(3).toString();
        result.append(c);
    }
    return result;
}

bool DatabaseManager::addCustomer(const QString &name, const QString &phone,
                                  const QString &email, int &customerId, QString &error)
{
    QSqlQuery q(db);
    q.prepare("INSERT INTO customers(name, phone, email, created_at) VALUES(?, ?, ?, ?)");
    q.addBindValue(name.trimmed());
    q.addBindValue(phone.trimmed());
    q.addBindValue(email.trimmed());
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    if (!q.exec())
    {
        error = q.lastError().text();
        return false;
    }
    customerId = q.lastInsertId().toInt();
    return true;
}

bool DatabaseManager::customerExists(int customerId) const
{
    QSqlQuery q(db);
    q.prepare("SELECT 1 FROM customers WHERE id = ?");
    q.addBindValue(customerId);
    return q.exec() && q.next();
}

int DatabaseManager::nextAccountNumber() const
{
    QSqlQuery q(db);
    if (!q.exec("SELECT COALESCE(MAX(account_number), 1001000) + 1 FROM accounts") || !q.next())
        return 1001001;
    return q.value(0).toInt();
}

bool DatabaseManager::insertAccount(int accountNumber, int customerId, const QString &type,
                                    double balance, double interestRate,
                                    double minimumBalance, double overdraftLimit,
                                    double monthlyFee, const QString &status,
                                    const QString &createdAt, QString &error)
{
    QSqlQuery q(db);
    q.prepare("INSERT INTO accounts(account_number, customer_id, type, balance, interest_rate, "
              "minimum_balance, overdraft_limit, monthly_fee, status, created_at) "
              "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(accountNumber);
    q.addBindValue(customerId);
    q.addBindValue(type);
    q.addBindValue(balance);
    q.addBindValue(interestRate);
    q.addBindValue(minimumBalance);
    q.addBindValue(overdraftLimit);
    q.addBindValue(monthlyFee);
    q.addBindValue(status);
    q.addBindValue(createdAt);
    if (!q.exec())
    {
        error = q.lastError().text();
        return false;
    }
    return true;
}

QList<DatabaseManager::AccountRecord> DatabaseManager::accounts(QString &error) const
{
    QList<AccountRecord> result;
    QSqlQuery q(db);
    const QString sql =
        "SELECT a.account_number, a.customer_id, c.name, a.type, a.balance, "
        "a.interest_rate, a.minimum_balance, a.overdraft_limit, a.monthly_fee, "
        "a.status, a.created_at "
        "FROM accounts a JOIN customers c ON c.id = a.customer_id "
        "ORDER BY a.account_number";
    if (!q.exec(sql))
    {
        error = q.lastError().text();
        return result;
    }
    while (q.next())
    {
        AccountRecord a;
        a.accountNumber = q.value(0).toInt();
        a.customerId = q.value(1).toInt();
        a.ownerName = q.value(2).toString();
        a.type = q.value(3).toString();
        a.balance = q.value(4).toDouble();
        a.interestRate = q.value(5).toDouble();
        a.minimumBalance = q.value(6).toDouble();
        a.overdraftLimit = q.value(7).toDouble();
        a.monthlyFee = q.value(8).toDouble();
        a.status = q.value(9).toString();
        a.createdAt = q.value(10).toString();
        result.append(a);
    }
    return result;
}

bool DatabaseManager::updateAccountBalance(int accountNumber, double balance, QString &error)
{
    QSqlQuery q(db);
    q.prepare("UPDATE accounts SET balance = ? WHERE account_number = ?");
    q.addBindValue(balance);
    q.addBindValue(accountNumber);
    if (!q.exec())
    {
        error = q.lastError().text();
        return false;
    }
    return q.numRowsAffected() == 1;
}

bool DatabaseManager::addTransaction(int accountNumber, const QString &dateTime,
                                      const QString &type, const QString &description,
                                      double amount, double balanceAfter, QString &error)
{
    QSqlQuery q(db);
    q.prepare("INSERT INTO transactions(account_number, date_time, type, description, amount, balance_after) "
              "VALUES(?, ?, ?, ?, ?, ?)");
    q.addBindValue(accountNumber);
    q.addBindValue(dateTime);
    q.addBindValue(type);
    q.addBindValue(description);
    q.addBindValue(amount);
    q.addBindValue(balanceAfter);
    if (!q.exec())
    {
        error = q.lastError().text();
        return false;
    }
    return true;
}

QList<QVariantMap> DatabaseManager::transactions(QString &error) const
{
    QList<QVariantMap> result;
    QSqlQuery q(db);
    if (!q.exec("SELECT t.account_number, c.name, t.date_time, t.type, t.description, "
                "t.amount, t.balance_after FROM transactions t "
                "JOIN accounts a ON a.account_number=t.account_number "
                "JOIN customers c ON c.id=a.customer_id ORDER BY t.id DESC"))
    {
        error = q.lastError().text();
        return result;
    }
    while (q.next())
    {
        QVariantMap row;
        row["accountNumber"] = q.value(0);
        row["owner"] = q.value(1);
        row["dateTime"] = q.value(2);
        row["type"] = q.value(3);
        row["description"] = q.value(4);
        row["amount"] = q.value(5);
        row["balanceAfter"] = q.value(6);
        result.append(row);
    }
    return result;
}

QList<QVariantMap> DatabaseManager::transactionsForAccount(int accountNumber, QString &error) const
{
    QList<QVariantMap> result;
    QSqlQuery q(db);
    q.prepare("SELECT date_time, type, description, amount, balance_after "
              "FROM transactions WHERE account_number=? ORDER BY id DESC");
    q.addBindValue(accountNumber);
    if (!q.exec())
    {
        error = q.lastError().text();
        return result;
    }
    while (q.next())
    {
        QVariantMap row;
        row["dateTime"] = q.value(0);
        row["type"] = q.value(1);
        row["description"] = q.value(2);
        row["amount"] = q.value(3);
        row["balanceAfter"] = q.value(4);
        result.append(row);
    }
    return result;
}
