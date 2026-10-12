#include "account.h"
#include <QDateTime>

Account::Account(int number, const QString &owner, double initialBalance)
    : accountNumber(number), ownerName(owner), balance(0.0)
{
    if (initialBalance > 0)
    {
        balance = initialBalance;
        logTransaction("Deposit", "Initial deposit", initialBalance);
    }
}

bool Account::deposit(double amount, QString &message)
{
    if (amount <= 0)
    {
        message = "Deposit amount must be greater than zero.";
        return false;
    }

    balance += amount;
    logTransaction("Deposit", "Cash deposit", amount);
    message = QString("₹%1 deposited successfully.").arg(amount, 0, 'f', 2);
    return true;
}

void Account::restoreBalance(double value)
{
    balance = value;
}

void Account::restoreTransaction(const Transaction &transaction)
{
    transactionHistory.append(transaction);
}

void Account::updateLatestTransaction(const QString &type, const QString &description)
{
    if (transactionHistory.isEmpty())
        return;
    transactionHistory.first().type = type;
    transactionHistory.first().description = description;
}

void Account::logTransaction(const QString &type,
                             const QString &description,
                             double amount)
{
    Transaction t;
    t.dateTime = QDateTime::currentDateTime().toString("dd MMM yyyy  hh:mm AP");
    t.type = type;
    t.description = description;
    t.amount = amount;
    t.balanceAfter = balance;
    transactionHistory.prepend(t);
}

int Account::getAccountNumber() const
{
    return accountNumber;
}

QString Account::getOwnerName() const
{
    return ownerName;
}

double Account::getBalance() const
{
    return balance;
}

const QVector<Transaction>& Account::getHistory() const
{
    return transactionHistory;
}

QString Account::displayInfo() const
{
    return QString(
        "Account No.: %1\n"
        "Owner: %2\n"
        "Type: %3\n"
        "Balance: ₹%4\n"
        "%5")
        .arg(accountNumber)
        .arg(ownerName)
        .arg(getAccountType())
        .arg(balance, 0, 'f', 2)
        .arg(getExtraInfo());
}

QString Account::printHistory() const
{
    QString result;

    for (const Transaction &t : transactionHistory)
    {
        result += QString("%1 | %2 | %3 | ₹%4 | Balance: ₹%5\n")
                      .arg(t.dateTime)
                      .arg(t.type)
                      .arg(t.description)
                      .arg(t.amount, 0, 'f', 2)
                      .arg(t.balanceAfter, 0, 'f', 2);
    }

    return result.isEmpty() ? "No transactions yet." : result;
}
