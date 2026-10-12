#ifndef SAVINGSACCOUNT_H
#define SAVINGSACCOUNT_H

#include "account.h"

class SavingsAccount : public Account
{
private:
    double interestRate;
    double minimumBalance;

public:
    SavingsAccount(int number,
                   const QString &owner,
                   double initialBalance,
                   double rate = 5.0,
                   double minimum = 500.0);

    bool withdraw(double amount, QString &message) override;
    void applyMonthlyUpdate(QString &message) override;

    QString getAccountType() const override;
    QString getExtraInfo() const override;

    double getInterestRate() const;
    double getMinimumBalance() const;
};

#endif
