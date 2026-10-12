#include "loginwindow.h"
#include "bankmanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QFrame>
#include <QFont>

LoginWindow::LoginWindow(BankManager *bank, QWidget *parent)
    : QDialog(parent), bank(bank)
{
    setWindowTitle("BMS • Admin Login");
    setFixedSize(980, 600);

    setStyleSheet(R"(
        QDialog { background:#061522; color:#effafa; }
        QLabel { color:#eaf5f4; }
        QLineEdit {
            background:#0c2637; border:1px solid #1a4658; border-radius:10px;
            padding:13px; color:white; font-size:14px;
        }
        QLineEdit:focus { border:1px solid #1bbbad; }
        QPushButton {
            background:#0da99b; color:white; border:none; border-radius:10px;
            padding:13px; font-weight:800;
        }
        QPushButton:hover { background:#13c2b2; }
    )");

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    auto *left = new QFrame;
    left->setStyleSheet("background:#071d2d;");
    auto *leftLayout = new QVBoxLayout(left);
    leftLayout->setContentsMargins(58,52,58,52);

    QLabel *logo = new QLabel("◈");
    logo->setAlignment(Qt::AlignCenter);
    logo->setStyleSheet("background:#0b827b;border:1px solid #2acbb9;border-radius:34px;color:white;font-size:32px;font-weight:bold;");
    logo->setFixedSize(68,68);

    QLabel *title = new QLabel("Admin Login");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:30px;font-weight:800;");

    QLabel *sub = new QLabel("Secure access to the Bank Management System");
    sub->setWordWrap(true); sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet("color:#7fa1ad;font-size:13px;");

    userEdit = new QLineEdit;
    userEdit->setPlaceholderText("Admin username");
    userEdit->setText("admin");

    pinEdit = new QLineEdit;
    pinEdit->setPlaceholderText("4-digit PIN");
    pinEdit->setEchoMode(QLineEdit::Password);
    pinEdit->setText("1234");

    QPushButton *loginButton = new QPushButton("SIGN IN  →");
    QLabel *hint = new QLabel("First-run admin:  admin  /  1234");
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet("color:#607f8b;font-size:11px;");

    leftLayout->addStretch();
    leftLayout->addWidget(logo,0,Qt::AlignHCenter);
    leftLayout->addSpacing(20);
    leftLayout->addWidget(title);
    leftLayout->addWidget(sub);
    leftLayout->addSpacing(28);
    leftLayout->addWidget(new QLabel("USERNAME"));
    leftLayout->addWidget(userEdit);
    leftLayout->addSpacing(10);
    leftLayout->addWidget(new QLabel("PIN"));
    leftLayout->addWidget(pinEdit);
    leftLayout->addSpacing(15);
    leftLayout->addWidget(loginButton);
    leftLayout->addSpacing(8);
    leftLayout->addWidget(hint);
    leftLayout->addStretch();

    auto *right = new QFrame;
    right->setStyleSheet(R"(
        QFrame { background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
        stop:0 #063a48, stop:0.5 #075d61, stop:1 #062b43); }
    )");
    auto *rightLayout = new QVBoxLayout(right);
    rightLayout->setContentsMargins(55,55,55,55);

    QLabel *bankTitle = new QLabel("BANK");
    bankTitle->setAlignment(Qt::AlignCenter);
    bankTitle->setStyleSheet("font-size:46px;font-weight:900;color:white;letter-spacing:3px;");
    QLabel *tag = new QLabel("Safe Banking  •  Better Tomorrow");
    tag->setAlignment(Qt::AlignCenter);
    tag->setStyleSheet("font-size:16px;color:#aee8e2;");

    QFrame *featureCard = new QFrame;
    featureCard->setStyleSheet("background:#073e4c;border:1px solid #1d7779;border-radius:14px;");
    QVBoxLayout *fl = new QVBoxLayout(featureCard);
    QLabel *ft = new QLabel("ADMIN CONTROL CENTER");
    ft->setStyleSheet("color:#68ddd0;font-size:11px;font-weight:800;letter-spacing:1px;");
    QLabel *features = new QLabel("✓  Persistent customer database\n✓  Account and balance management\n✓  Deposit, withdrawal & transfer\n✓  Complete transaction audit trail\n✓  Savings interest & checking fees");
    features->setStyleSheet("color:#c1dddd;font-size:14px;line-height:160%;");
    fl->addWidget(ft); fl->addSpacing(8); fl->addWidget(features);

    QLabel *shield = new QLabel("◉  Your banking workspace is protected");
    shield->setAlignment(Qt::AlignCenter);
    shield->setStyleSheet("color:#83ded4;font-size:12px;");

    rightLayout->addStretch(); rightLayout->addWidget(bankTitle); rightLayout->addWidget(tag); rightLayout->addSpacing(30); rightLayout->addWidget(featureCard); rightLayout->addSpacing(25); rightLayout->addWidget(shield); rightLayout->addStretch();

    mainLayout->addWidget(left,1);
    mainLayout->addWidget(right,1);

    connect(loginButton,&QPushButton::clicked,this,&LoginWindow::attemptLogin);
    connect(pinEdit,&QLineEdit::returnPressed,this,&LoginWindow::attemptLogin);
}

void LoginWindow::attemptLogin()
{
    if (bank->authenticateAdmin(userEdit->text(), pinEdit->text()))
    {
        accept();
        return;
    }

    QMessageBox::warning(this,"Access Denied","Incorrect admin username or PIN.");
}
