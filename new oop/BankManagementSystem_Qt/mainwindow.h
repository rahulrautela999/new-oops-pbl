#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class BankManager;
class Account;
class QLabel;
class QTableWidget;
class QStackedWidget;
class QProgressBar;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(BankManager *bank, QWidget *parent = nullptr);

private:
    BankManager *bank;
    QStackedWidget *pages;
    QTableWidget *accountsTable;
    QTableWidget *transactionsTable;
    QTableWidget *customersTable;
    QProgressBar *savingsProgress;
    QProgressBar *checkingProgress;

    QLabel *totalBalanceLabel;
    QLabel *savingsLabel;
    QLabel *checkingLabel;
    QLabel *accountCountLabel;
    QLabel *customerCountLabel;
    QLabel *welcomeNameLabel;

    int selectedAccountNumber() const;

    QWidget* buildDashboard();
    QWidget* buildAccountsPage();
    QWidget* buildCustomersPage();
    QWidget* buildTransactionsPage();
    QWidget* buildMonthlyPage();
    QWidget* buildAboutPage();

    void refreshDashboard();
    void refreshAccounts();
    void refreshCustomers();
    void refreshTransactions();
    void deposit();
    void withdraw();
    void transfer();
    void createAccount();
    void addCustomer();
    void monthlyUpdate();
    void showAccountDetails();

    void showPage(int index);
};

#endif
