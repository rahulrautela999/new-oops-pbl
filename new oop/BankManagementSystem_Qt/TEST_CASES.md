# Test Cases — Persistent Bank Management System

## 1. First run

- Start the application.
- Login with `admin / 1234`.
- Confirm the dashboard opens.
- Confirm demo data appears on a fresh database.

## 2. Wrong admin login

- Enter an incorrect PIN.
- Expected: Access Denied message.
- Dashboard must not open.

## 3. Add customer

- Open Customers.
- Click Add Customer.
- Enter a unique name.
- Save.
- Expected: customer appears in the table.

## 4. Duplicate customer

- Add the same name again.
- Expected: customer is rejected as a duplicate.

## 5. Create account from saved customer

- Add a customer.
- Open Accounts > Create Account.
- Confirm the customer appears in the owner dropdown.
- Create an account.
- Expected: account is displayed and an initial deposit transaction is stored when the initial amount is greater than zero.

## 6. Persistence after restart

- Create a customer.
- Create an account.
- Deposit money.
- Close the application.
- Reopen it.
- Login again.
- Expected: customer, account, balance and transaction history remain.

## 7. Deposit

- Deposit a positive amount.
- Expected: balance increases and a Deposit transaction is stored.

## 8. Withdrawal

- Withdraw an allowed amount.
- Expected: balance decreases and a Withdrawal transaction is stored.

## 9. Insufficient savings balance

- Try to withdraw enough to go below the configured minimum balance.
- Expected: withdrawal is rejected and balance does not change.

## 10. Transfer

- Create two accounts.
- Transfer money from account A to account B.
- Expected: A decreases, B increases, and both sides receive transaction records.

## 11. Invalid transfer

- Transfer to the same account.
- Expected: operation is rejected.

## 12. Monthly update

- Run Monthly Update.
- Expected: savings accounts receive interest and checking accounts receive their configured fee; the result is saved.

## 13. Transaction audit

- Open Transactions.
- Expected: records are loaded from SQLite and sorted newest first.

## 14. Database path

- Open About.
- Expected: exact SQLite database path is displayed.
