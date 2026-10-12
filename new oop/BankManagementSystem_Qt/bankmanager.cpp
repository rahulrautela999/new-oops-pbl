#include "bankmanager.h"

#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

BankManager::BankManager()
    : nextAccountNumber(1001001)
{
}

BankManager::~BankManager()
{
    qDeleteAll(accounts);
    accounts.clear();
}

bool BankManager::initialize(QString &error)
{
    if (!database.open(error))
        return false;

    qDeleteAll(accounts);
    accounts.clear();

    const QList<DatabaseManager::AccountRecord> rows = database.accounts(error);
    if (!error.isEmpty())
        return false;

    for (const auto &row : rows)
    {
        Account *account = nullptr;
        if (row.type == "Savings Account")
            account = new SavingsAccount(row.accountNumber, row.ownerName, 0.0,
                                         row.interestRate, row.minimumBalance);
        else
            account = new CheckingAccount(row.accountNumber, row.ownerName, 0.0,
                                          row.overdraftLimit, row.monthlyFee);

        account->restoreBalance(row.balance);

        QString historyError;
        const QList<QVariantMap> history = database.transactionsForAccount(row.accountNumber, historyError);
        if (!historyError.isEmpty())
        {
            delete account;
            error = historyError;
            return false;
        }

        // DB returns newest first; Account history also displays newest first.
        for (const QVariantMap &rowData : history)
        {
            Transaction t;
            t.dateTime = rowData["dateTime"].toString();
            t.type = rowData["type"].toString();
            t.description = rowData["description"].toString();
            t.amount = rowData["amount"].toDouble();
            t.balanceAfter = rowData["balanceAfter"].toDouble();
            account->restoreTransaction(t);
        }

        accounts.append(account);
        nextAccountNumber = qMax(nextAccountNumber, row.accountNumber + 1);
    }

    nextAccountNumber = qMax(nextAccountNumber, database.nextAccountNumber());
    return true;
}

bool BankManager::authenticateAdmin(const QString &username, const QString &pin) const
{
    return database.authenticateAdmin(username, pin);
}

QString BankManager::databasePath() const
{
    return database.databasePath();
}

void BankManager::loadDemoData()
{
    // First-run sample data. After that everything is loaded from SQLite.
    if (!accounts.isEmpty())
        return;

    QString error;
    QList<CustomerRecord> people = database.customers(error);
    if (!error.isEmpty())
        return;

    int rahulId = -1;
    for (const CustomerRecord &c : people)
    {
        if (c.name.compare("Rahul Singh", Qt::CaseInsensitive) == 0)
        {
            rahulId = c.id;
            break;
        }
    }

    if (rahulId < 0)
        database.addCustomer("Rahul Singh", "", "", rahulId, error);

    if (rahulId < 0 || !error.isEmpty())
        return;

    QString message;
    SavingsAccount *s = createSavings(rahulId, "Rahul Singh", 8450.0, 5.0, 500.0, message);
    CheckingAccount *c = createChecking(rahulId, "Rahul Singh", 7000.0, 2000.0, 10.0, message);

    if (s && c)
    {
        deposit(s->getAccountNumber(), 500.0, message);
        withdraw(s->getAccountNumber(), 200.0, message);
        deposit(c->getAccountNumber(), 1200.0, message);
        withdraw(c->getAccountNumber(), 350.0, message);
    }
}

SavingsAccount* BankManager::createSavings(int customerId, const QString &owner,
                                           double initial, double rate, double minimum,
                                           QString &message)
{
    if (!database.customerExists(customerId))
    {
        message = "Selected customer does not exist in the database.";
        return nullptr;
    }
    if (initial < 0 || rate < 0 || minimum < 0)
    {
        message = "Account values cannot be negative.";
        return nullptr;
    }

    const int number = nextAccountNumber++;
    const QString created = QDateTime::currentDateTime().toString(Qt::ISODate);
    if (!database.insertAccount(number, customerId, "Savings Account", initial, rate,
                                minimum, 0.0, 0.0, "Active", created, message))
    {
        --nextAccountNumber;
        return nullptr;
    }

    SavingsAccount *account = new SavingsAccount(number, owner, initial, rate, minimum);
    accounts.append(account);

    // Constructor's initial deposit is already in memory; persist that transaction.
    if (!account->getHistory().isEmpty())
    {
        const Transaction &t = account->getHistory().last();
        if (!database.addTransaction(number, t.dateTime, t.type, t.description,
                                     t.amount, t.balanceAfter, message))
        {
            accounts.removeLast();
            delete account;
            QSqlQuery cleanup;
            cleanup.exec(QString("DELETE FROM accounts WHERE account_number=%1").arg(number));
            return nullptr;
        }
    }

    message = QString("Savings account %1 created for %2.").arg(number).arg(owner);
    return account;
}

CheckingAccount* BankManager::createChecking(int customerId, const QString &owner,
                                             double initial, double overdraft, double fee,
                                             QString &message)
{
    if (!database.customerExists(customerId))
    {
        message = "Selected customer does not exist in the database.";
        return nullptr;
    }
    if (initial < 0 || overdraft < 0 || fee < 0)
    {
        message = "Account values cannot be negative.";
        return nullptr;
    }

    const int number = nextAccountNumber++;
    const QString created = QDateTime::currentDateTime().toString(Qt::ISODate);
    if (!database.insertAccount(number, customerId, "Checking Account", initial, 0.0,
                                0.0, overdraft, fee, "Active", created, message))
    {
        --nextAccountNumber;
        return nullptr;
    }

    CheckingAccount *account = new CheckingAccount(number, owner, initial, overdraft, fee);
    accounts.append(account);

    if (!account->getHistory().isEmpty())
    {
        const Transaction &t = account->getHistory().last();
        if (!database.addTransaction(number, t.dateTime, t.type, t.description,
                                     t.amount, t.balanceAfter, message))
        {
            accounts.removeLast();
            delete account;
            QSqlQuery cleanup;
            cleanup.exec(QString("DELETE FROM accounts WHERE account_number=%1").arg(number));
            return nullptr;
        }
    }

    message = QString("Checking account %1 created for %2.").arg(number).arg(owner);
    return account;
}

bool BankManager::addCustomer(const QString &name, const QString &phone,
                               const QString &email, int &customerId, QString &message)
{
    if (name.trimmed().isEmpty())
    {
        message = "Customer name cannot be empty.";
        return false;
    }
    if (database.addCustomer(name, phone, email, customerId, message))
    {
        message = QString("Customer '%1' was saved.").arg(name.trimmed());
        return true;
    }
    if (message.contains("UNIQUE", Qt::CaseInsensitive))
        message = "A customer with this name already exists.";
    return false;
}

QList<CustomerRecord> BankManager::getCustomers(QString &error) const
{
    return database.customers(error);
}

Account* BankManager::findAccount(int accountNumber)
{
    for (Account *account : accounts)
        if (account && account->getAccountNumber() == accountNumber)
            return account;
    return nullptr;
}

const QVector<Account*>& BankManager::getAccounts() const { return accounts; }

double BankManager::totalBalance() const
{
    double total = 0;
    for (Account *a : accounts) if (a) total += a->getBalance();
    return total;
}

double BankManager::totalSavings() const
{
    double total = 0;
    for (Account *a : accounts)
        if (a && a->getAccountType() == "Savings Account") total += a->getBalance();
    return total;
}

double BankManager::totalChecking() const
{
    double total = 0;
    for (Account *a : accounts)
        if (a && a->getAccountType() == "Checking Account") total += a->getBalance();
    return total;
}

bool BankManager::saveLatestTransaction(Account *account, QString &error)
{
    if (!account || account->getHistory().isEmpty())
    {
        error = "No transaction was generated.";
        return false;
    }
    const Transaction &t = account->getHistory().first();
    return database.addTransaction(account->getAccountNumber(), t.dateTime, t.type,
                                   t.description, t.amount, t.balanceAfter, error);
}

bool BankManager::persistAccountAndLatestTransaction(Account *account, QString &error)
{
    if (!account)
    {
        error = "Account not found.";
        return false;
    }

    if (!database.updateAccountBalance(account->getAccountNumber(), account->getBalance(), error))
        return false;

    return saveLatestTransaction(account, error);
}

bool BankManager::deposit(int accountNumber, double amount, QString &message)
{
    Account *account = findAccount(accountNumber);
    if (!account)
    {
        message = "Account not found.";
        return false;
    }

    const int historyBefore = account->getHistory().size();
    if (!account->deposit(amount, message))
        return false;

    if (account->getHistory().size() != historyBefore &&
        !persistAccountAndLatestTransaction(account, message))
    {
        return false;
    }
    return true;
}

bool BankManager::withdraw(int accountNumber, double amount, QString &message)
{
    Account *account = findAccount(accountNumber);
    if (!account)
    {
        message = "Account not found.";
        return false;
    }

    const int historyBefore = account->getHistory().size();
    if (!account->withdraw(amount, message))
        return false;

    if (account->getHistory().size() != historyBefore &&
        !persistAccountAndLatestTransaction(account, message))
    {
        return false;
    }
    return true;
}

bool BankManager::transfer(int fromAccount, int toAccount, double amount, QString &message)
{
    if (fromAccount == toAccount)
    {
        message = "Sender and receiver accounts must be different.";
        return false;
    }
    if (amount <= 0)
    {
        message = "Transfer amount must be greater than zero.";
        return false;
    }

    Account *from = findAccount(fromAccount);
    Account *to = findAccount(toAccount);
    if (!from || !to)
    {
        message = "Sender or receiver account was not found.";
        return false;
    }

    const double oldFromBalance = from->getBalance();
    const double oldToBalance = to->getBalance();
    const int oldFromHistory = from->getHistory().size();
    const int oldToHistory = to->getHistory().size();

    QString withdrawMessage;
    if (!from->withdraw(amount, withdrawMessage))
    {
        message = withdrawMessage;
        return false;
    }

    QString depositMessage;
    if (!to->deposit(amount, depositMessage))
    {
        from->restoreBalance(oldFromBalance);
        while (from->getHistory().size() > oldFromHistory)
            const_cast<QVector<Transaction>&>(from->getHistory()).removeFirst();
        message = depositMessage;
        return false;
    }

    from->updateLatestTransaction("Transfer", QString("Transfer to A/C %1").arg(toAccount));
    to->updateLatestTransaction("Transfer", QString("Transfer from A/C %1").arg(fromAccount));

    QSqlDatabase sqlDb = QSqlDatabase::database("bank_connection");
    if (!sqlDb.transaction())
    {
        from->restoreBalance(oldFromBalance);
        to->restoreBalance(oldToBalance);
        while (from->getHistory().size() > oldFromHistory)
            const_cast<QVector<Transaction>&>(from->getHistory()).removeFirst();
        while (to->getHistory().size() > oldToHistory)
            const_cast<QVector<Transaction>&>(to->getHistory()).removeFirst();
        message = "Unable to start the database transaction.";
        return false;
    }

    QString error;
    const Transaction &fromTx = from->getHistory().first();
    const Transaction &toTx = to->getHistory().first();
    bool ok = database.updateAccountBalance(fromAccount, from->getBalance(), error) &&
              database.updateAccountBalance(toAccount, to->getBalance(), error) &&
              database.addTransaction(fromAccount, fromTx.dateTime, fromTx.type,
                                      fromTx.description, -amount, from->getBalance(), error) &&
              database.addTransaction(toAccount, toTx.dateTime, toTx.type,
                                      toTx.description, amount, to->getBalance(), error);

    if (!ok || !sqlDb.commit())
    {
        sqlDb.rollback();
        from->restoreBalance(oldFromBalance);
        to->restoreBalance(oldToBalance);
        while (from->getHistory().size() > oldFromHistory)
            const_cast<QVector<Transaction>&>(from->getHistory()).removeFirst();
        while (to->getHistory().size() > oldToHistory)
            const_cast<QVector<Transaction>&>(to->getHistory()).removeFirst();
        message = error.isEmpty() ? "Transfer failed and was rolled back." : error;
        return false;
    }

    message = QString("₹%1 transferred from A/C %2 to A/C %3.")
                  .arg(amount, 0, 'f', 2).arg(fromAccount).arg(toAccount);
    return true;
}

QList<QVariantMap> BankManager::getTransactions(QString &error) const
{
    return database.transactions(error);
}

QList<QVariantMap> BankManager::getAccountTransactions(int accountNumber, QString &error) const
{
    return database.transactionsForAccount(accountNumber, error);
}

void BankManager::applyMonthlyUpdates(QString &report)
{
    report.clear();
    QSqlDatabase sqlDb = QSqlDatabase::database("bank_connection");
    if (!sqlDb.transaction())
    {
        report = "Unable to start database transaction.";
        return;
    }

    bool allOk = true;
    QString error;
    for (Account *account : accounts)
    {
        if (!account) continue;
        const int oldSize = account->getHistory().size();
        QString message;
        account->applyMonthlyUpdate(message);
        if (account->getHistory().size() == oldSize ||
            !database.updateAccountBalance(account->getAccountNumber(), account->getBalance(), error) ||
            !saveLatestTransaction(account, error))
        {
            allOk = false;
            break;
        }
        report += QString("%1 (%2): %3\n")
                      .arg(account->getAccountNumber())
                      .arg(account->getAccountType())
                      .arg(message);
    }

    if (!allOk || !sqlDb.commit())
    {
        sqlDb.rollback();
        report = error.isEmpty() ? "Monthly update failed and was rolled back." : error;
    }
}
