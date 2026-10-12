#include "mainwindow.h"
#include "bankmanager.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QTableWidget>
#include <QHeaderView>
#include <QStackedWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QProgressBar>
#include <QDateTime>
#include <QAbstractItemView>
#include <QFont>
#include <QColor>

static QString money(double value)
{
    return QString("₹%1").arg(value, 0, 'f', 2);
}

static void styleDialog(QDialog &dialog)
{
    dialog.setStyleSheet(R"(
        QDialog { background:#071a2a; color:#eef7f6; }
        QLabel { color:#dbe9e8; }
        QLineEdit, QComboBox, QDoubleSpinBox {
            background:#0e2a3a; color:#f4fbfa; border:1px solid #1f5261;
            border-radius:9px; padding:10px;
        }
        QLineEdit:focus, QComboBox:focus, QDoubleSpinBox:focus { border:1px solid #22c7b8; }
        QPushButton {
            background:#10a99b; color:white; border:none; border-radius:9px;
            padding:10px 18px; font-weight:bold;
        }
        QPushButton:hover { background:#18c2b2; }
    )");
}

MainWindow::MainWindow(BankManager *bank, QWidget *parent)
    : QMainWindow(parent), bank(bank)
{
    setWindowTitle("BMS • Admin Control Center");
    resize(1450, 850);
    setMinimumSize(1150, 700);

    setStyleSheet(R"(
        QMainWindow, QWidget {
            background:#061522;
            color:#eaf4f4;
            font-family:"Segoe UI";
        }
        QFrame#topbar {
            background:#071d2d;
            border-bottom:1px solid #15384a;
        }
        QFrame#sidebar {
            background:#061522;
            border-right:1px solid #143246;
        }
        QLabel#brand {
            color:#f3fffe;
            font-size:20px;
            font-weight:800;
        }
        QLabel#brandSub { color:#65bdb6; font-size:11px; letter-spacing:1px; }
        QLabel#pageTitle { color:#f5fbfb; font-size:30px; font-weight:800; }
        QLabel#muted { color:#7593a1; font-size:13px; }
        QLabel#welcome { color:#f7ffff; font-size:27px; font-weight:800; }
        QLabel#balanceHero { color:#ffffff; font-size:32px; font-weight:800; }
        QLabel#cardTitle { color:#7897a5; font-size:11px; font-weight:800; letter-spacing:1px; }
        QLabel#cardValue { color:#f5ffff; font-size:25px; font-weight:800; }
        QLabel#small { color:#8eb0bb; font-size:12px; }
        QLabel#positive { color:#28d2b7; font-weight:700; }
        QLabel#negative { color:#ff7d79; font-weight:700; }

        QPushButton#nav {
            text-align:left;
            background:transparent;
            border:none;
            border-radius:10px;
            padding:13px 15px;
            color:#7e9aa8;
            font-size:14px;
        }
        QPushButton#nav:hover { background:#0d2a3b; color:#eaffff; }
        QPushButton#nav:checked { background:#0c746e; color:white; }

        QPushButton#primary {
            background:#10a99b;
            color:white;
            border:none;
            border-radius:9px;
            padding:11px 18px;
            font-weight:800;
        }
        QPushButton#primary:hover { background:#16c1b1; }
        QPushButton#blue {
            background:#1769d0; color:white; border:none; border-radius:10px;
            padding:10px 16px; font-weight:800;
        }
        QPushButton#blue:hover { background:#287fe8; }
        QPushButton#coral {
            background:#d95d59; color:white; border:none; border-radius:10px;
            padding:10px 16px; font-weight:800;
        }
        QPushButton#coral:hover { background:#ed6d68; }
        QPushButton#secondary {
            background:#0c2637; color:#cfe2e4; border:1px solid #1b4355;
            border-radius:9px; padding:9px 15px;
        }
        QPushButton#secondary:hover { background:#123447; }
        QPushButton#actionGreen {
            background:#0b8f78; color:white; border:none; border-radius:12px;
            padding:16px; font-weight:800; text-align:left;
        }
        QPushButton#actionGreen:hover { background:#10a88d; }
        QPushButton#actionRed {
            background:#b84e54; color:white; border:none; border-radius:12px;
            padding:16px; font-weight:800; text-align:left;
        }
        QPushButton#actionRed:hover { background:#ce5d63; }
        QPushButton#actionBlue {
            background:#155eaa; color:white; border:none; border-radius:12px;
            padding:16px; font-weight:800; text-align:left;
        }
        QPushButton#actionBlue:hover { background:#1c70c8; }
        QPushButton#actionTeal {
            background:#087e87; color:white; border:none; border-radius:12px;
            padding:16px; font-weight:800; text-align:left;
        }
        QPushButton#actionTeal:hover { background:#0b9aa4; }

        QFrame#card {
            background:#0a2233;
            border:1px solid #143b4c;
            border-radius:14px;
        }
        QFrame#hero {
            background:qlineargradient(x1:0,y1:0,x2:1,y2:1,
                stop:0 #0b3b49, stop:0.48 #075b60, stop:1 #063b52);
            border:1px solid #1c7778;
            border-radius:16px;
        }
        QFrame#balanceCard {
            background:qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #064f52, stop:0.55 #087f77, stop:1 #075366);
            border:1px solid #15998f;
            border-radius:16px;
        }
        QFrame#infoCard {
            background:#09273a;
            border:1px solid #17465a;
            border-radius:14px;
        }

        QTableWidget {
            background:#092033;
            alternate-background-color:#071b2b;
            border:1px solid #163b4d;
            border-radius:12px;
            gridline-color:#113447;
            color:#e7f3f3;
            selection-background-color:#0b6864;
            selection-color:white;
        }
        QTableWidget::item { padding:7px; }
        QHeaderView::section {
            background:#0d293a;
            color:#83a6b2;
            border:none;
            padding:11px;
            font-weight:800;
        }
        QProgressBar {
            background:#092033;
            border:1px solid #194052;
            border-radius:7px;
            text-align:center;
            color:#dffefd;
            height:12px;
        }
        QProgressBar::chunk { background:#16b9a9; border-radius:6px; }
        QScrollBar:vertical { background:#061522; width:10px; }
        QScrollBar::handle:vertical { background:#1b4657; border-radius:5px; }
    )");

    QWidget *central = new QWidget;
    setCentralWidget(central);
    QHBoxLayout *root = new QHBoxLayout(central);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    QFrame *sidebar = new QFrame;
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(245);
    QVBoxLayout *side = new QVBoxLayout(sidebar);
    side->setContentsMargins(18,22,18,18);
    side->setSpacing(6);

    QLabel *brand = new QLabel("◈  BMS");
    brand->setObjectName("brand");
    QLabel *brandSub = new QLabel("ADMIN CONTROL CENTER");
    brandSub->setObjectName("brandSub");
    side->addWidget(brand);
    side->addWidget(brandSub);
    side->addSpacing(28);

    const QStringList navNames = {
        "⌂   Dashboard",
        "▣   Accounts",
        "♙   Customers",
        "⇄   Transactions",
        "◷   Monthly Update",
        "▤   About"
    };

    for (int i = 0; i < navNames.size(); ++i)
    {
        QPushButton *button = new QPushButton(navNames[i]);
        button->setObjectName("nav");
        button->setCheckable(true);
        if (i == 0) button->setChecked(true);
        side->addWidget(button);
        connect(button, &QPushButton::clicked, this, [this, i]() {
            showPage(i);
        });
    }

    side->addStretch();
    QLabel *admin = new QLabel("●  ADMIN SESSION\n   Authenticated & secure");
    admin->setStyleSheet("color:#4ec7b9;font-size:11px;padding:10px;");
    side->addWidget(admin);

    QPushButton *logout = new QPushButton("↪   Logout");
    logout->setObjectName("secondary");
    side->addWidget(logout);
    connect(logout, &QPushButton::clicked, this, &QWidget::close);

    QFrame *content = new QFrame;
    content->setStyleSheet("QFrame{background:#061522;}");
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);

    QFrame *topbar = new QFrame;
    topbar->setObjectName("topbar");
    topbar->setFixedHeight(64);
    QHBoxLayout *top = new QHBoxLayout(topbar);
    top->setContentsMargins(25,0,25,0);
    QLabel *topTitle = new QLabel("Bank Management System");
    topTitle->setStyleSheet("font-size:16px;font-weight:700;color:#eaf8f7;");
    top->addWidget(topTitle);
    top->addStretch();
    QLabel *session = new QLabel("ADMIN  •  Online");
    session->setStyleSheet("color:#55cfc0;font-size:12px;font-weight:700;");
    top->addWidget(session);
    contentLayout->addWidget(topbar);

    pages = new QStackedWidget;
    pages->addWidget(buildDashboard());
    pages->addWidget(buildAccountsPage());
    pages->addWidget(buildCustomersPage());
    pages->addWidget(buildTransactionsPage());
    pages->addWidget(buildMonthlyPage());
    pages->addWidget(buildAboutPage());
    contentLayout->addWidget(pages);

    root->addWidget(sidebar);
    root->addWidget(content,1);

    refreshDashboard();
    refreshAccounts();
    refreshCustomers();
    refreshTransactions();
}

QWidget* MainWindow::buildDashboard()
{
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(28,24,28,24);
    layout->setSpacing(16);

    QHBoxLayout *header = new QHBoxLayout;
    QVBoxLayout *titles = new QVBoxLayout;
    QLabel *title = new QLabel("Dashboard");
    title->setObjectName("pageTitle");
    QLabel *sub = new QLabel("Monitor customers, accounts and transactions from one secure admin workspace.");
    sub->setObjectName("muted");
    titles->addWidget(title);
    titles->addWidget(sub);
    header->addLayout(titles);
    header->addStretch();
    QPushButton *newCustomer = new QPushButton("+  Add Customer");
    newCustomer->setObjectName("secondary");
    QPushButton *newAccount = new QPushButton("+  New Account");
    newAccount->setObjectName("primary");
    header->addWidget(newCustomer);
    header->addWidget(newAccount);
    connect(newCustomer,&QPushButton::clicked,this,&MainWindow::addCustomer);
    connect(newAccount,&QPushButton::clicked,this,&MainWindow::createAccount);
    layout->addLayout(header);

    QFrame *hero = new QFrame;
    hero->setObjectName("hero");
    QHBoxLayout *heroLayout = new QHBoxLayout(hero);
    heroLayout->setContentsMargins(24,20,24,20);
    QVBoxLayout *heroText = new QVBoxLayout;
    QLabel *small = new QLabel("SECURE BANKING OPERATIONS");
    small->setStyleSheet("color:#6de2d2;font-size:11px;font-weight:800;letter-spacing:1px;");
    welcomeNameLabel = new QLabel("Welcome back, Admin");
    welcomeNameLabel->setObjectName("welcome");
    QLabel *heroSub = new QLabel("Manage your banking environment with confidence.");
    heroSub->setStyleSheet("color:#b7dddd;font-size:13px;");
    heroText->addWidget(small);
    heroText->addSpacing(5);
    heroText->addWidget(welcomeNameLabel);
    heroText->addWidget(heroSub);
    heroLayout->addLayout(heroText);
    heroLayout->addStretch();
    QLabel *shield = new QLabel("✓\nSECURE");
    shield->setAlignment(Qt::AlignCenter);
    shield->setStyleSheet("background:#0a6f69;border:1px solid #2bc8b7;border-radius:38px;color:white;font-size:13px;font-weight:800;padding:12px;");
    shield->setFixedSize(78,78);
    heroLayout->addWidget(shield);
    layout->addWidget(hero);

    QHBoxLayout *mainRow = new QHBoxLayout;
    mainRow->setSpacing(14);

    QVBoxLayout *left = new QVBoxLayout;
    left->setSpacing(14);

    QFrame *balance = new QFrame;
    balance->setObjectName("balanceCard");
    QVBoxLayout *bl = new QVBoxLayout(balance);
    bl->setContentsMargins(20,16,20,16);
    QLabel *bt = new QLabel("TOTAL AVAILABLE BALANCE");
    bt->setObjectName("cardTitle");
    totalBalanceLabel = new QLabel("₹0.00");
    totalBalanceLabel->setObjectName("balanceHero");
    QLabel *bsub = new QLabel("Across all active accounts");
    bsub->setStyleSheet("color:#b5efea;font-size:12px;");
    bl->addWidget(bt); bl->addWidget(totalBalanceLabel); bl->addWidget(bsub);
    left->addWidget(balance);

    QGridLayout *stats = new QGridLayout;
    stats->setSpacing(12);
    auto statCard = [](const QString &titleText, QLabel **value) {
        QFrame *card = new QFrame; card->setObjectName("card");
        QVBoxLayout *l = new QVBoxLayout(card); l->setContentsMargins(16,13,16,13);
        QLabel *t = new QLabel(titleText); t->setObjectName("cardTitle");
        *value = new QLabel("0"); (*value)->setObjectName("cardValue");
        l->addWidget(t); l->addWidget(*value); return card;
    };
    QFrame *s1 = statCard("SAVINGS BALANCE", &savingsLabel);
    QFrame *s2 = statCard("CHECKING BALANCE", &checkingLabel);
    QFrame *s3 = statCard("TOTAL ACCOUNTS", &accountCountLabel);
    QFrame *s4 = statCard("CUSTOMERS", &customerCountLabel);
    stats->addWidget(s1,0,0); stats->addWidget(s2,0,1); stats->addWidget(s3,1,0); stats->addWidget(s4,1,1);
    left->addLayout(stats);

    QLabel *qaTitle = new QLabel("QUICK ACTIONS");
    qaTitle->setObjectName("cardTitle");
    left->addWidget(qaTitle);
    QGridLayout *actions = new QGridLayout;
    actions->setSpacing(10);
    QPushButton *depositBtn = new QPushButton("↓  Deposit\nAdd funds to an account"); depositBtn->setObjectName("actionGreen");
    QPushButton *withdrawBtn = new QPushButton("↑  Withdraw\nRemove funds from an account"); withdrawBtn->setObjectName("actionRed");
    QPushButton *transferBtn = new QPushButton("⇄  Transfer\nMove money between accounts"); transferBtn->setObjectName("actionBlue");
    QPushButton *historyBtn = new QPushButton("▤  Transactions\nReview complete history"); historyBtn->setObjectName("actionTeal");
    actions->addWidget(depositBtn,0,0); actions->addWidget(withdrawBtn,0,1); actions->addWidget(transferBtn,1,0); actions->addWidget(historyBtn,1,1);
    connect(depositBtn,&QPushButton::clicked,this,&MainWindow::deposit);
    connect(withdrawBtn,&QPushButton::clicked,this,&MainWindow::withdraw);
    connect(transferBtn,&QPushButton::clicked,this,&MainWindow::transfer);
    connect(historyBtn,&QPushButton::clicked,this,[this](){showPage(3);});
    left->addLayout(actions);

    mainRow->addLayout(left,2);

    QVBoxLayout *right = new QVBoxLayout;
    QFrame *quick = new QFrame; quick->setObjectName("infoCard");
    QVBoxLayout *ql = new QVBoxLayout(quick); ql->setContentsMargins(18,17,18,17);
    QLabel *qt = new QLabel("QUICK INFO"); qt->setObjectName("cardTitle"); ql->addWidget(qt);
    QLabel *info = new QLabel("Admin access\nFull account controls\nPersistent SQLite storage\nTransaction audit trail\nCustomer database");
    info->setStyleSheet("color:#b5ced3;font-size:13px;line-height:150%;");
    ql->addWidget(info);
    right->addWidget(quick);

    QFrame *snapshot = new QFrame; snapshot->setObjectName("card");
    QVBoxLayout *sl = new QVBoxLayout(snapshot); sl->setContentsMargins(18,17,18,17);
    QLabel *st = new QLabel("FINANCIAL SNAPSHOT"); st->setObjectName("cardTitle"); sl->addWidget(st);
    QLabel *savText = new QLabel("Savings allocation"); savText->setObjectName("small"); sl->addWidget(savText);
    savingsProgress = new QProgressBar; savingsProgress->setRange(0,100); savingsProgress->setValue(0); savingsProgress->setTextVisible(false); sl->addWidget(savingsProgress);
    QLabel *chkText = new QLabel("Checking allocation"); chkText->setObjectName("small"); sl->addWidget(chkText);
    checkingProgress = new QProgressBar; checkingProgress->setRange(0,100); checkingProgress->setValue(0); checkingProgress->setTextVisible(false); sl->addWidget(checkingProgress);
    QLabel *snapNote = new QLabel("Allocation is calculated from current balances."); snapNote->setWordWrap(true); snapNote->setStyleSheet("color:#6f919c;font-size:11px;"); sl->addWidget(snapNote);
    right->addWidget(snapshot);

    QFrame *security = new QFrame; security->setObjectName("hero");
    QVBoxLayout *sec = new QVBoxLayout(security); sec->setContentsMargins(18,16,18,16);
    QLabel *secTitle = new QLabel("✓  Banking Made Safer"); secTitle->setStyleSheet("font-size:15px;font-weight:800;color:#f1ffff;");
    QLabel *secText = new QLabel("Customer records, balances and transactions are stored locally in the application's SQLite database."); secText->setWordWrap(true); secText->setStyleSheet("color:#b8dddd;font-size:12px;");
    sec->addWidget(secTitle); sec->addWidget(secText);
    right->addWidget(security);
    right->addStretch();
    mainRow->addLayout(right,1);

    layout->addLayout(mainRow,1);
    return page;
}

QWidget* MainWindow::buildAccountsPage()
{
    QWidget *page = new QWidget; QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(28,24,28,24); layout->setSpacing(14);
    QHBoxLayout *header = new QHBoxLayout;
    QLabel *title = new QLabel("Accounts"); title->setObjectName("pageTitle");
    QLabel *sub = new QLabel("Every account is loaded from persistent storage."); sub->setObjectName("muted");
    QVBoxLayout *titles = new QVBoxLayout; titles->addWidget(title); titles->addWidget(sub);
    QPushButton *customer = new QPushButton("+  Add Customer"); customer->setObjectName("secondary");
    QPushButton *newButton = new QPushButton("+  Create Account"); newButton->setObjectName("primary");
    header->addLayout(titles); header->addStretch(); header->addWidget(customer); header->addWidget(newButton); layout->addLayout(header);
    accountsTable = new QTableWidget(0,6);
    accountsTable->setHorizontalHeaderLabels({"Account No.","Type","Owner","Balance","Status","Action"});
    accountsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    accountsTable->setSelectionBehavior(QAbstractItemView::SelectRows); accountsTable->setEditTriggers(QAbstractItemView::NoEditTriggers); accountsTable->setAlternatingRowColors(true);
    layout->addWidget(accountsTable,1);
    connect(customer,&QPushButton::clicked,this,&MainWindow::addCustomer);
    connect(newButton,&QPushButton::clicked,this,&MainWindow::createAccount);
    connect(accountsTable,&QTableWidget::cellDoubleClicked,this,[this](int,int){showAccountDetails();});
    return page;
}

QWidget* MainWindow::buildCustomersPage()
{
    QWidget *page = new QWidget; QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(28,24,28,24); layout->setSpacing(14);
    QHBoxLayout *header = new QHBoxLayout;
    QVBoxLayout *titles = new QVBoxLayout;
    QLabel *title = new QLabel("Customers"); title->setObjectName("pageTitle");
    QLabel *sub = new QLabel("Names saved here become selectable account owners."); sub->setObjectName("muted");
    titles->addWidget(title); titles->addWidget(sub);
    QPushButton *add = new QPushButton("+  Add Customer"); add->setObjectName("primary");
    header->addLayout(titles); header->addStretch(); header->addWidget(add); layout->addLayout(header);
    customersTable = new QTableWidget(0,4);
    customersTable->setHorizontalHeaderLabels({"Customer ID","Name","Phone","Email"});
    customersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    customersTable->setEditTriggers(QAbstractItemView::NoEditTriggers); customersTable->setSelectionBehavior(QAbstractItemView::SelectRows); customersTable->setAlternatingRowColors(true);
    layout->addWidget(customersTable,1);
    connect(add,&QPushButton::clicked,this,&MainWindow::addCustomer);
    return page;
}

QWidget* MainWindow::buildTransactionsPage()
{
    QWidget *page = new QWidget; QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(28,24,28,24); layout->setSpacing(14);
    QHBoxLayout *header = new QHBoxLayout;
    QVBoxLayout *titles = new QVBoxLayout;
    QLabel *title = new QLabel("Transactions"); title->setObjectName("pageTitle");
    QLabel *sub = new QLabel("Persistent audit trail for every account movement."); sub->setObjectName("muted");
    titles->addWidget(title); titles->addWidget(sub);
    QPushButton *refresh = new QPushButton("↻  Refresh"); refresh->setObjectName("secondary");
    header->addLayout(titles); header->addStretch(); header->addWidget(refresh); layout->addLayout(header);
    transactionsTable = new QTableWidget(0,6);
    transactionsTable->setHorizontalHeaderLabels({"Date & Time","Account","Owner","Type","Amount","Balance After"});
    transactionsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    transactionsTable->setEditTriggers(QAbstractItemView::NoEditTriggers); transactionsTable->setSelectionBehavior(QAbstractItemView::SelectRows); transactionsTable->setAlternatingRowColors(true);
    layout->addWidget(transactionsTable,1);
    connect(refresh,&QPushButton::clicked,this,&MainWindow::refreshTransactions);
    return page;
}

QWidget* MainWindow::buildMonthlyPage()
{
    QWidget *page = new QWidget; QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(28,24,28,24);
    QLabel *title = new QLabel("Monthly Update"); title->setObjectName("pageTitle");
    QLabel *sub = new QLabel("Apply savings interest and checking service fees, then persist the results."); sub->setObjectName("muted");
    layout->addWidget(title); layout->addWidget(sub); layout->addSpacing(18);
    QFrame *card = new QFrame; card->setObjectName("card"); QVBoxLayout *c = new QVBoxLayout(card); c->setContentsMargins(22,20,22,20);
    QLabel *processDescription = new QLabel("MONTH-END PROCESS\n\nSavings accounts receive their configured monthly interest. Checking accounts receive their configured monthly service fee. Each change is written to the transaction history and saved to SQLite.");
    processDescription->setWordWrap(true); processDescription->setStyleSheet("color:#b7ced3;font-size:15px;");
    QPushButton *run = new QPushButton("▶  Run Monthly Update"); run->setObjectName("primary");
    c->addWidget(processDescription); c->addSpacing(20); c->addWidget(run,0,Qt::AlignLeft); layout->addWidget(card); layout->addStretch();
    connect(run,&QPushButton::clicked,this,&MainWindow::monthlyUpdate);
    return page;
}

QWidget* MainWindow::buildAboutPage()
{
    QWidget *page = new QWidget; QVBoxLayout *layout = new QVBoxLayout(page); layout->setContentsMargins(28,24,28,24);
    QLabel *title = new QLabel("About the Project"); title->setObjectName("pageTitle"); layout->addWidget(title); layout->addSpacing(16);
    QFrame *card = new QFrame; card->setObjectName("card"); QVBoxLayout *c = new QVBoxLayout(card); c->setContentsMargins(22,20,22,20);
    QLabel *text = new QLabel(
        "<h2>Bank Management System</h2>"
        "<p>This version combines the original C++ OOP model with a persistent SQLite data layer and an admin-focused Qt interface.</p>"
        "<p><b>OOP:</b> Account is abstract; SavingsAccount and CheckingAccount use inheritance and polymorphism.</p>"
        "<p><b>Storage:</b> customers, accounts and transactions survive application restarts.</p>"
        "<p><b>Security:</b> the admin PIN is stored as a SHA-256 hash rather than plain text.</p>"
        "<p><b>Database:</b> " + bank->databasePath() + "</p>");
    text->setWordWrap(true); text->setStyleSheet("color:#b9d0d4;font-size:14px;"); c->addWidget(text); layout->addWidget(card); layout->addStretch();
    return page;
}

void MainWindow::showPage(int index)
{
    pages->setCurrentIndex(index);
    if(index==0) refreshDashboard();
    if(index==1) refreshAccounts();
    if(index==2) refreshCustomers();
    if(index==3) refreshTransactions();
}

int MainWindow::selectedAccountNumber() const
{
    if(!accountsTable || accountsTable->currentRow()<0) return -1;
    QTableWidgetItem *item = accountsTable->item(accountsTable->currentRow(),0);
    if(!item) return -1;
    bool ok=false; int number=item->text().toInt(&ok); return ok?number:-1;
}

void MainWindow::refreshDashboard()
{
    if(!totalBalanceLabel) return;
    totalBalanceLabel->setText(money(bank->totalBalance()));
    savingsLabel->setText(money(bank->totalSavings()));
    checkingLabel->setText(money(bank->totalChecking()));
    accountCountLabel->setText(QString::number(bank->getAccounts().size()));
    QString error; customerCountLabel->setText(QString::number(bank->getCustomers(error).size()));
    const double total=bank->totalBalance();
    const int sav=total>0 ? qRound(bank->totalSavings()/total*100.0) : 0;
    const int chk=total>0 ? qRound(bank->totalChecking()/total*100.0) : 0;
    savingsProgress->setValue(sav); checkingProgress->setValue(chk);
}

void MainWindow::refreshAccounts()
{
    if(!accountsTable) return;
    accountsTable->setRowCount(0);
    // Account objects are restored from SQLite when BankManager starts.
    for(Account *a : bank->getAccounts())
    {
        int r=accountsTable->rowCount(); accountsTable->insertRow(r);
        accountsTable->setItem(r,0,new QTableWidgetItem(QString::number(a->getAccountNumber())));
        accountsTable->setItem(r,1,new QTableWidgetItem(a->getAccountType()));
        accountsTable->setItem(r,2,new QTableWidgetItem(a->getOwnerName()));
        accountsTable->setItem(r,3,new QTableWidgetItem(money(a->getBalance())));
        accountsTable->setItem(r,4,new QTableWidgetItem("Active"));
        QPushButton *view=new QPushButton("View"); view->setObjectName("secondary"); accountsTable->setCellWidget(r,5,view);
        int number=a->getAccountNumber();
        connect(view,&QPushButton::clicked,this,[this,number](){
            Account *acc=bank->findAccount(number);
            if(acc) QMessageBox::information(this,"Account Details",acc->displayInfo()+"\n\nTransactions: "+QString::number(acc->getHistory().size()));
        });
    }
}

void MainWindow::refreshCustomers()
{
    if(!customersTable) return;
    customersTable->setRowCount(0);
    QString error;
    const QList<CustomerRecord> people=bank->getCustomers(error);
    if(!error.isEmpty()) return;
    for(const CustomerRecord &c:people)
    {
        int r=customersTable->rowCount(); customersTable->insertRow(r);
        customersTable->setItem(r,0,new QTableWidgetItem(QString::number(c.id)));
        customersTable->setItem(r,1,new QTableWidgetItem(c.name));
        customersTable->setItem(r,2,new QTableWidgetItem(c.phone));
        customersTable->setItem(r,3,new QTableWidgetItem(c.email));
    }
}

void MainWindow::refreshTransactions()
{
    if(!transactionsTable) return;
    transactionsTable->setRowCount(0);
    QString error; const QList<QVariantMap> rows=bank->getTransactions(error);
    if(!error.isEmpty()) return;
    for(const QVariantMap &v:rows)
    {
        int r=transactionsTable->rowCount(); transactionsTable->insertRow(r);
        transactionsTable->setItem(r,0,new QTableWidgetItem(v["dateTime"].toString()));
        transactionsTable->setItem(r,1,new QTableWidgetItem(QString::number(v["accountNumber"].toInt())));
        transactionsTable->setItem(r,2,new QTableWidgetItem(v["owner"].toString()));
        transactionsTable->setItem(r,3,new QTableWidgetItem(v["type"].toString()));
        const double amount=v["amount"].toDouble();
        QTableWidgetItem *amountItem=new QTableWidgetItem((amount>=0?"+":"")+money(amount));
        amountItem->setForeground(amount>=0?QColor("#36d2b7"):QColor("#ff817b"));
        transactionsTable->setItem(r,4,amountItem);
        transactionsTable->setItem(r,5,new QTableWidgetItem(money(v["balanceAfter"].toDouble())));
    }
}

void MainWindow::deposit()
{
    if(bank->getAccounts().isEmpty()){ QMessageBox::information(this,"No Accounts","Create an account first."); return; }
    QStringList options; for(Account *a:bank->getAccounts()) options<<QString("%1 — %2 — %3").arg(a->getAccountNumber()).arg(a->getAccountType()).arg(a->getOwnerName());
    bool ok=false; QString selected=QInputDialog::getItem(this,"Deposit","Select account:",options,0,false,&ok); if(!ok)return;
    int number=selected.section(" ",0,0).toInt();
    double amount=QInputDialog::getDouble(this,"Deposit","Amount:",1000,0.01,100000000,2,&ok); if(!ok)return;
    QString message; const bool success=bank->deposit(number,amount,message);
    QMessageBox::information(this,success?"Deposit Successful":"Deposit Failed",message);
    refreshDashboard(); refreshAccounts(); refreshTransactions();
}

void MainWindow::withdraw()
{
    if(bank->getAccounts().isEmpty()){ QMessageBox::information(this,"No Accounts","Create an account first."); return; }
    QStringList options; for(Account *a:bank->getAccounts()) options<<QString("%1 — %2 — %3").arg(a->getAccountNumber()).arg(a->getAccountType()).arg(a->getOwnerName());
    bool ok=false; QString selected=QInputDialog::getItem(this,"Withdraw","Select account:",options,0,false,&ok); if(!ok)return;
    int number=selected.section(" ",0,0).toInt();
    double amount=QInputDialog::getDouble(this,"Withdraw","Amount:",500,0.01,100000000,2,&ok); if(!ok)return;
    QString message; const bool success=bank->withdraw(number,amount,message);
    QMessageBox::information(this,success?"Withdrawal Successful":"Withdrawal Failed",message);
    refreshDashboard(); refreshAccounts(); refreshTransactions();
}

void MainWindow::transfer()
{
    if(bank->getAccounts().size()<2){ QMessageBox::information(this,"Transfer","At least two accounts are required."); return; }
    QStringList options; for(Account *a:bank->getAccounts()) options<<QString("%1 — %2 — %3").arg(a->getAccountNumber()).arg(a->getOwnerName()).arg(a->getAccountType());
    bool ok=false; QString fromText=QInputDialog::getItem(this,"Transfer","From account:",options,0,false,&ok); if(!ok)return;
    QString toText=QInputDialog::getItem(this,"Transfer","To account:",options,0,false,&ok); if(!ok)return;
    int from=fromText.section(" ",0,0).toInt(); int to=toText.section(" ",0,0).toInt();
    double amount=QInputDialog::getDouble(this,"Transfer","Amount:",1000,0.01,100000000,2,&ok); if(!ok)return;
    QString message; const bool success=bank->transfer(from,to,amount,message);
    QMessageBox::information(this,success?"Transfer Successful":"Transfer Failed",message);
    refreshDashboard(); refreshAccounts(); refreshTransactions();
}

void MainWindow::addCustomer()
{
    QDialog dialog(this); dialog.setWindowTitle("Add Customer"); dialog.setMinimumWidth(440); styleDialog(dialog);
    QVBoxLayout *l=new QVBoxLayout(&dialog);
    QLabel *title=new QLabel("Add Customer"); title->setStyleSheet("font-size:22px;font-weight:800;"); l->addWidget(title); l->addSpacing(8);
    QLineEdit *name=new QLineEdit; name->setPlaceholderText("Full customer name");
    QLineEdit *phone=new QLineEdit; phone->setPlaceholderText("Phone (optional)");
    QLineEdit *email=new QLineEdit; email->setPlaceholderText("Email (optional)");
    l->addWidget(new QLabel("Name")); l->addWidget(name); l->addWidget(new QLabel("Phone")); l->addWidget(phone); l->addWidget(new QLabel("Email")); l->addWidget(email);
    QDialogButtonBox *buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel); l->addSpacing(8); l->addWidget(buttons);
    connect(buttons,&QDialogButtonBox::accepted,&dialog,&QDialog::accept); connect(buttons,&QDialogButtonBox::rejected,&dialog,&QDialog::reject);
    if(dialog.exec()!=QDialog::Accepted)return;
    int id=-1; QString message;
    if(!bank->addCustomer(name->text(),phone->text(),email->text(),id,message)){ QMessageBox::warning(this,"Customer Not Saved",message); return; }
    refreshCustomers(); refreshDashboard();
    QMessageBox::information(this,"Customer Saved",message+QString("\nCustomer ID: %1").arg(id));
}

void MainWindow::createAccount()
{
    QString error; const QList<CustomerRecord> people=bank->getCustomers(error);
    if(!error.isEmpty()){ QMessageBox::critical(this,"Database Error",error); return; }
    if(people.isEmpty()){
        QMessageBox::information(this,"No Customers","Add a customer first. The saved customer name will then appear in the account form.");
        addCustomer();
        error.clear();
        if(bank->getCustomers(error).isEmpty()) return;
    }

    const QList<CustomerRecord> updatedPeople=bank->getCustomers(error);
    if(updatedPeople.isEmpty()) return;

    QDialog dialog(this); dialog.setWindowTitle("Create New Account"); dialog.setMinimumWidth(470); styleDialog(dialog);
    QVBoxLayout *l=new QVBoxLayout(&dialog);
    QLabel *title=new QLabel("Create New Account"); title->setStyleSheet("font-size:22px;font-weight:800;"); l->addWidget(title); l->addSpacing(8);
    QComboBox *owner=new QComboBox;
    for(const CustomerRecord &c:updatedPeople) owner->addItem(QString("%1  •  ID %2").arg(c.name).arg(c.id),c.id);
    QComboBox *type=new QComboBox; type->addItems({"Savings Account","Checking Account"});
    QDoubleSpinBox *initial=new QDoubleSpinBox; initial->setRange(0,100000000); initial->setDecimals(2); initial->setValue(1000);
    QDoubleSpinBox *rate=new QDoubleSpinBox; rate->setRange(0,100); rate->setDecimals(2); rate->setValue(5);
    QDoubleSpinBox *minimum=new QDoubleSpinBox; minimum->setRange(0,100000000); minimum->setDecimals(2); minimum->setValue(500);
    QDoubleSpinBox *overdraft=new QDoubleSpinBox; overdraft->setRange(0,100000000); overdraft->setDecimals(2); overdraft->setValue(2000);
    QDoubleSpinBox *fee=new QDoubleSpinBox; fee->setRange(0,100000); fee->setDecimals(2); fee->setValue(10);
    l->addWidget(new QLabel("Customer / Account Owner")); l->addWidget(owner);
    l->addWidget(new QLabel("Account Type")); l->addWidget(type);
    l->addWidget(new QLabel("Initial Deposit")); l->addWidget(initial);
    l->addWidget(new QLabel("Interest Rate (Savings)")); l->addWidget(rate);
    l->addWidget(new QLabel("Minimum Balance (Savings)")); l->addWidget(minimum);
    l->addWidget(new QLabel("Overdraft Limit (Checking)")); l->addWidget(overdraft);
    l->addWidget(new QLabel("Monthly Fee (Checking)")); l->addWidget(fee);
    QDialogButtonBox *buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel); l->addSpacing(8); l->addWidget(buttons);
    connect(buttons,&QDialogButtonBox::accepted,&dialog,&QDialog::accept); connect(buttons,&QDialogButtonBox::rejected,&dialog,&QDialog::reject);
    if(dialog.exec()!=QDialog::Accepted)return;
    const int customerId=owner->currentData().toInt(); const QString ownerName=owner->currentText().section("  •",0,0).trimmed();
    QString message; Account *created=nullptr;
    if(type->currentIndex()==0) created=bank->createSavings(customerId,ownerName,initial->value(),rate->value(),minimum->value(),message);
    else created=bank->createChecking(customerId,ownerName,initial->value(),overdraft->value(),fee->value(),message);
    if(!created){ QMessageBox::warning(this,"Account Not Created",message); return; }
    refreshDashboard(); refreshAccounts(); refreshTransactions();
    QMessageBox::information(this,"Account Created",message+"\n\nThe account and its initial transaction are now stored in the SQLite database.");
}

void MainWindow::monthlyUpdate()
{
    QString report; bank->applyMonthlyUpdates(report); refreshDashboard(); refreshAccounts(); refreshTransactions();
    QMessageBox::information(this,"Monthly Update",report.isEmpty()?"Monthly update completed.":report);
}

void MainWindow::showAccountDetails()
{
    const int number=selectedAccountNumber(); if(number<0)return; Account *account=bank->findAccount(number); if(!account)return;
    QString details=account->displayInfo()+"\n\nTransaction History\n--------------------\n"+account->printHistory();
    QMessageBox::information(this,"Account Details",details);
}
