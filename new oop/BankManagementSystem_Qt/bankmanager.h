#ifndef BANKMANAGER_H
#define BANKMANAGER_H

#include <QList>
#include <QVector>
#include <QString>
#include <QVariantMap>

#include "account.h"
#include "savingsaccount.h"
#include "checkingaccount.h"
#include "database.h"

class BankManager
{
private:
    QVector<Account*> accounts;
    DatabaseManager database;
    int nextAccountNumber;

    bool saveLatestTransaction(Account *account, QString &error);
    bool persistAccountAndLatestTransaction(Account *account, QString &error);

public:
    BankManager();
    ~BankManager();

    BankManager(const BankManager&) = delete;
    BankManager& operator=(const BankManager&) = delete;

    bool initialize(QString &error);
    bool authenticateAdmin(const QString &username, const QString &pin) const;
    QString databasePath() const;

    // Loads persisted data. No demo records are created when saved data exists.
    void loadDemoData();

    SavingsAccount* createSavings(int customerId, const QString &owner,
                                  double initial, double rate, double minimum,
                                  QString &message);
    CheckingAccount* createChecking(int customerId, const QString &owner,
                                    double initial, double overdraft, double fee,
                                    QString &message);

    bool addCustomer(const QString &name, const QString &phone,
                     const QString &email, int &customerId, QString &message);
    QList<CustomerRecord> getCustomers(QString &error) const;

    Account* findAccount(int accountNumber);
    const QVector<Account*>& getAccounts() const;

    bool deposit(int accountNumber, double amount, QString &message);
    bool withdraw(int accountNumber, double amount, QString &message);
    bool transfer(int fromAccount, int toAccount, double amount, QString &message);

    double totalBalance() const;
    double totalSavings() const;
    double totalChecking() const;

    QList<QVariantMap> getTransactions(QString &error) const;
    QList<QVariantMap> getAccountTransactions(int accountNumber, QString &error) const;

    void applyMonthlyUpdates(QString &report);
};

#endif
