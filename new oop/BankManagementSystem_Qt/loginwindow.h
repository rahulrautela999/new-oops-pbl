#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>

class BankManager;
class QLineEdit;

class LoginWindow : public QDialog
{
public:
    explicit LoginWindow(BankManager *bank, QWidget *parent = nullptr);

private:
    BankManager *bank;
    QLineEdit *userEdit;
    QLineEdit *pinEdit;

    void attemptLogin();
};

#endif
