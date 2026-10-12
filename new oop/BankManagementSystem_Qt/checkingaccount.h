#ifndef CHECKINGACCOUNT_H
#define CHECKINGACCOUNT_H

#include "account.h"

class CheckingAccount : public Account
{
private:
    double overdraftLimit;
    double monthlyFee;

public:
    CheckingAccount(int number,
                    const QString &owner,
                    double initialBalance,
                    double overdraft = 2000.0,
                    double fee = 10.0);

    bool withdraw(double amount, QString &message) override;
    void applyMonthlyUpdate(QString &message) override;

    QString getAccountType() const override;
    QString getExtraInfo() const override;

    double getOverdraftLimit() const;
    double getMonthlyFee() const;
};

#endif
