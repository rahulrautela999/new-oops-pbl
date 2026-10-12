#include "savingsaccount.h"

SavingsAccount::SavingsAccount(int number,
                               const QString &owner,
                               double initialBalance,
                               double rate,
                               double minimum)
    : Account(number, owner, initialBalance),
      interestRate(rate),
      minimumBalance(minimum)
{
}

bool SavingsAccount::withdraw(double amount, QString &message)
{
    if (amount <= 0)
    {
        message = "Withdrawal amount must be greater than zero.";
        return false;
    }

    if (balance - amount < minimumBalance)
    {
        message = QString(
            "Withdrawal denied.\n\n"
            "Savings accounts must maintain a minimum balance of ₹%1.")
            .arg(minimumBalance, 0, 'f', 2);
        return false;
    }

    balance -= amount;
    logTransaction("Withdraw", "Savings withdrawal", -amount);

    message = QString("₹%1 withdrawn successfully.")
                  .arg(amount, 0, 'f', 2);
    return true;
}

void SavingsAccount::applyMonthlyUpdate(QString &message)
{
    if (interestRate < 0)
    {
        message = "Invalid negative interest rate.";
        return;
    }

    double interest = balance * interestRate / 100.0 / 12.0;

    if (interest > 0)
    {
        balance += interest;
        logTransaction("Interest", "Monthly interest added", interest);
    }

    message = QString("Monthly interest of ₹%1 applied.")
                  .arg(interest, 0, 'f', 2);
}

QString SavingsAccount::getAccountType() const
{
    return "Savings Account";
}

QString SavingsAccount::getExtraInfo() const
{
    return QString("Interest Rate: %1%\nMinimum Balance: ₹%2")
        .arg(interestRate, 0, 'f', 2)
        .arg(minimumBalance, 0, 'f', 2);
}

double SavingsAccount::getInterestRate() const
{
    return interestRate;
}

double SavingsAccount::getMinimumBalance() const
{
    return minimumBalance;
}
