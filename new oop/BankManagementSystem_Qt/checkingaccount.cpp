#include "checkingaccount.h"

CheckingAccount::CheckingAccount(int number,
                                 const QString &owner,
                                 double initialBalance,
                                 double overdraft,
                                 double fee)
    : Account(number, owner, initialBalance),
      overdraftLimit(overdraft),
      monthlyFee(fee)
{
}

bool CheckingAccount::withdraw(double amount, QString &message)
{
    if (amount <= 0)
    {
        message = "Withdrawal amount must be greater than zero.";
        return false;
    }

    if (balance - amount < -overdraftLimit)
    {
        message = QString(
            "Withdrawal denied.\n\n"
            "Overdraft limit is ₹%1.")
            .arg(overdraftLimit, 0, 'f', 2);
        return false;
    }

    balance -= amount;
    logTransaction("Withdraw", "Checking withdrawal", -amount);

    message = QString("₹%1 withdrawn successfully.")
                  .arg(amount, 0, 'f', 2);
    return true;
}

void CheckingAccount::applyMonthlyUpdate(QString &message)
{
    if (monthlyFee < 0)
    {
        message = "Invalid negative monthly fee.";
        return;
    }

    balance -= monthlyFee;
    logTransaction("Fee", "Monthly account fee", -monthlyFee);

    message = QString("Monthly fee of ₹%1 deducted.")
                  .arg(monthlyFee, 0, 'f', 2);
}

QString CheckingAccount::getAccountType() const
{
    return "Checking Account";
}

QString CheckingAccount::getExtraInfo() const
{
    return QString("Overdraft Limit: ₹%1\nMonthly Fee: ₹%2")
        .arg(overdraftLimit, 0, 'f', 2)
        .arg(monthlyFee, 0, 'f', 2);
}

double CheckingAccount::getOverdraftLimit() const
{
    return overdraftLimit;
}

double CheckingAccount::getMonthlyFee() const
{
    return monthlyFee;
}
