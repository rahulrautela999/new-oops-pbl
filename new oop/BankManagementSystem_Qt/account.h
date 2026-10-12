#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <QVector>

struct Transaction
{
    QString dateTime;
    QString type;
    QString description;
    double amount;
    double balanceAfter;
};

class BankManager;

class Account
{
protected:
    int accountNumber;
    QString ownerName;
    double balance;
    QVector<Transaction> transactionHistory;

    void logTransaction(const QString &type,
                        const QString &description,
                        double amount);

    void restoreBalance(double value);
    void restoreTransaction(const Transaction &transaction);
    void updateLatestTransaction(const QString &type, const QString &description);

friend class BankManager;

public:
    Account(int number, const QString &owner, double initialBalance);
    virtual ~Account() = default;

    virtual bool withdraw(double amount, QString &message) = 0;
    virtual void applyMonthlyUpdate(QString &message) = 0;

    bool deposit(double amount, QString &message);

    int getAccountNumber() const;
    QString getOwnerName() const;
    double getBalance() const;

    virtual QString getAccountType() const = 0;
    virtual QString getExtraInfo() const = 0;

    const QVector<Transaction>& getHistory() const;

    QString displayInfo() const;
    QString printHistory() const;
};

#endif
