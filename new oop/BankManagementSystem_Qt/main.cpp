#include <QApplication>
#include <QMessageBox>

#include "loginwindow.h"
#include "mainwindow.h"
#include "bankmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Bank Management System");
    app.setOrganizationName("BMS Project");

    BankManager bank;
    QString error;
    if (!bank.initialize(error))
    {
        QMessageBox::critical(nullptr, "Database Error",
                              "The bank database could not be opened.\n\n" + error);
        return 1;
    }

    bank.loadDemoData();

    LoginWindow login(&bank);
    if (login.exec() == QDialog::Accepted)
    {
        MainWindow window(&bank);
        window.show();
        return app.exec();
    }

    return 0;
}
