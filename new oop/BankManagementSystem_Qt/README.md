# Bank Management System — Qt Creator

## Redesigned + Persistent Database Version

This version upgrades the original Qt/C++ OOP demo into an admin-focused banking management application with a persistent SQLite database.

### Main login
- Username: `admin`
- PIN: `1234`

The PIN is stored in the SQLite database as a SHA-256 hash, not as plain text.

## What was added

### Modern GUI
- Dark navy/teal banking interface
- Admin Control Center layout
- Dashboard hero section
- Balance and account statistics
- Quick action cards
- Financial allocation snapshot
- Customers page
- Accounts page
- Transactions page
- Monthly update page
- Responsive table-based management screens

### Persistent storage
The application now stores data in SQLite:

- `admin_users` — admin authentication
- `customers` — customer names, phone and email
- `accounts` — account number, owner, type, balance, rates, limits, fees and status
- `transactions` — complete transaction audit history

The database is automatically created the first time the application starts.

Qt stores it under the application's standard data directory. The exact path is shown on the About page.

## Customer → Account workflow

1. Login as admin.
2. Open **Customers**.
3. Click **Add Customer**.
4. Save a name, phone and email.
5. Open **Accounts** and click **Create Account**.
6. The account-owner combo box is populated from the saved customer records.
7. Select the customer and create a Savings or Checking account.
8. Close and reopen the application. The customer, account, balance and transactions remain available.

This means you no longer have to type an arbitrary owner name every time an account is created.

## Banking functionality

- Create Savings Account
- Create Checking Account
- Deposit
- Withdraw
- Transfer between accounts
- Transaction history
- Monthly savings interest
- Monthly checking service fee
- Savings minimum-balance rule
- Checking overdraft rule
- Persistent account balances
- Persistent transaction history
- Persistent customer records
- Admin-only access
- Database-backed authentication

## Qt Creator build

1. Extract the ZIP.
2. Open `BankManagementSystem.pro` in Qt Creator.
3. Select a Desktop kit with Qt SQL/SQLite support.
4. Run **Build > Clean All**.
5. Run **Build > Run qmake**.
6. Run **Build Project**.
7. Press **Run**.

The `.pro` file now contains:

```text
QT += widgets sql
```

Your Qt installation must include the SQLite SQL driver (`QSQLITE`).

## Project structure

```text
BankManagementSystem_Qt/
│
├── main.cpp
├── database.h / database.cpp
├── account.h / account.cpp
├── savingsaccount.h / savingsaccount.cpp
├── checkingaccount.h / checkingaccount.cpp
├── bankmanager.h / bankmanager.cpp
├── loginwindow.h / loginwindow.cpp
├── mainwindow.h / mainwindow.cpp
├── BankManagementSystem.pro
└── README.md
```

## Important note

This is a realistic educational banking application, not production banking software. A real bank would additionally require hardened security, MFA, KYC/AML, encrypted secrets, secure APIs, audit infrastructure, concurrency controls, backups and regulatory compliance.
