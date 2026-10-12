#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QList>
#include <QVariantMap>
#include <QString>

struct CustomerRecord
{
    int id = -1;
    QString name;
    QString phone;
    QString email;
};

class DatabaseManager
{
private:
    QSqlDatabase db;
    QString path;

    bool exec(const QString &sql, QString &error) const;

public:
    DatabaseManager();
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool open(QString &error);
    bool isOpen() const;
    QString databasePath() const;

    bool authenticateAdmin(const QString &username, const QString &pin) const;

    QList<CustomerRecord> customers(QString &error) const;
    bool addCustomer(const QString &name, const QString &phone,
                     const QString &email, int &customerId, QString &error);
    bool customerExists(int customerId) const;

    int nextAccountNumber() const;

    bool insertAccount(int accountNumber, int customerId, const QString &type,
                       double balance, double interestRate,
                       double minimumBalance, double overdraftLimit,
                       double monthlyFee, const QString &status,
                       const QString &createdAt, QString &error);

    struct AccountRecord
    {
        int accountNumber;
        int customerId;
        QString ownerName;
        QString type;
        double balance;
        double interestRate;
        double minimumBalance;
        double overdraftLimit;
        double monthlyFee;
        QString status;
        QString createdAt;
    };

    QList<AccountRecord> accounts(QString &error) const;
    bool updateAccountBalance(int accountNumber, double balance, QString &error);
    bool addTransaction(int accountNumber, const QString &dateTime,
                        const QString &type, const QString &description,
                        double amount, double balanceAfter, QString &error);
    QList<QVariantMap> transactions(QString &error) const;
    QList<QVariantMap> transactionsForAccount(int accountNumber, QString &error) const;
};

#endif
