#include <iostream>
#include <fstream>
using namespace std;

struct Account
{
    int accountNumber;    // account number
    int pin;              // personal identification number
    int availableBalance; // funds available for withdrawal
    int totalBalance;     // funds available + funds waiting to clear
};

// retrieve Account object (from Accounts.dat) containing specified accountNumber and pin,
// put this object to currentAccount,
// and put the record number (in Accounts.dat) of this object into recordNumber.
// if this object exists, returns true; otherwise returns false.
bool loadAccount(int accountNumber, int pin, Account& currentAccount, int& recordNumber);

// display the main menu and perform transactions
void performTransactions(Account& currentAccount, int& recordNumber, int& remainingBills);

// perform withdrawal transaction
void withdrawal(Account& currentAccount, int& remainingBills);

// perform deposit transaction
void deposit(Account& currentAccount);

// write currentAccount into the n-th record of Accounts.dat
void saveAccount(Account currentAccount, int n);

const int INITIAL_BILLS = 5; // the number bills loaded each day

int main()
{
    // the number of remaining bills in the cash dispenser
    int remainingBills = INITIAL_BILLS;
    int recordNumber;
    Account currentAccount;

    // welcome and authenticate user; perform transactions
    while (true)
    {
        int accountNumber;
        int pin;

        do {
            cout << "Welcome!\n";
            cout << "\nPlease enter your account number: ";
            cin >> accountNumber;
            cout << "\nEnter your PIN: ";
            cin >> pin; // input PIN
        } while (!loadAccount(accountNumber, pin, currentAccount, recordNumber));

        performTransactions(currentAccount, recordNumber, remainingBills);
        cout << "\nThank you! Goodbye!\n\n";
    }
} // end function main

// retrieve Account object (from Accounts.dat) containing specified accountNumber and pin,
// put this object to currentAccount,
// and put the record number of this object into recordNumber.
// if this object exists, returns true; otherwise returns false.
bool loadAccount(int accountNumber, int pin, Account& currentAccount, int& recordNumber)
{
    ifstream ioFile("Accounts.dat", ios::binary);
    if (!ioFile)
    {
        cout << "File could not be opened" << endl;
        exit(1);
    }
    
    recordNumber = 0;

    while(!ioFile.eof())
    {
        ioFile.read(reinterpret_cast<char*>(&currentAccount.accountNumber), sizeof(currentAccount.accountNumber));
        ioFile.read(reinterpret_cast<char*>(&currentAccount.pin), sizeof(currentAccount.pin));
        ioFile.read(reinterpret_cast<char*>(&currentAccount.availableBalance), sizeof(currentAccount.availableBalance));
        ioFile.read(reinterpret_cast<char*>(&currentAccount.totalBalance), sizeof(currentAccount.totalBalance));

//        cout << currentAccount.accountNumber << "  " << currentAccount.pin << "  " << currentAccount.availableBalance << "  " << currentAccount.totalBalance << endl;

        if (currentAccount.accountNumber == accountNumber && currentAccount.pin == pin)
        {
            ioFile.close();
            return true;
        }        
        recordNumber++;
    }
    recordNumber = 0;
    
    cout << "\nInvalid account number or PIN. Please try again.\n";

    ioFile.close();
    return false;
} // end function loadAccount

// display the main menu and perform transactions
void performTransactions(Account& currentAccount, int& recordNumber, int& remainingBills)
{
    cout << "\nMain meum:" << endl;
    cout << "1 - View my balance" << endl;
    cout << "2 - Withdraw cash" << endl;
    cout << "3 - Deoisit funds" << endl;
    cout << "4 - Exit" << endl;
    cout << "\nEnter a choice: ";
    int cmd;
    cin >> cmd;

    if (cmd == 1)
    {
        cout << "Balance Information:" << endl;
        cout << " - Available balance: $" << currentAccount.availableBalance << endl;
        cout << " - Total balance:   $" << currentAccount.totalBalance << endl;
        performTransactions(currentAccount, recordNumber, remainingBills);
    }

    else if (cmd == 2)
    {
        withdrawal(currentAccount, remainingBills);
        performTransactions(currentAccount, recordNumber, remainingBills);
    }
        
    else if (cmd == 3)
    {
        deposit(currentAccount);
        performTransactions(currentAccount, recordNumber, remainingBills);
    }
        
    else if (cmd == 4)
        cout << "\nExiting the system..." << endl;
    else
    {
        cout << "\nYou did not enter a valid selection. Try again.\n";
        performTransactions(currentAccount,recordNumber,remainingBills);
    }
    saveAccount(currentAccount,recordNumber);

} // end function performTransactions

// perform withdrawal transaction
void withdrawal(Account& currentAccount, int& remainingBills)
{
    cout << "\nWithdrawal options:" << endl;
    cout << "1 - $20" << endl;
    cout << "2 - $40" << endl;
    cout << "3 - $60" << endl;
    cout << "4 - $100" << endl;
    cout << "5 - $200" << endl;
    cout << "6 - Cancel transaction" << endl;
    cout << "\nChoose a withdrawal option (1-6): ";
    int cmd;
    cin >> cmd;
    if (cmd == 1)
    {        
        if (remainingBills == 0)
        {
            cout << "\nInsufficient cash available in the ATM.\n";
            cout << "\nPlease choose a smaller amount.\n";
        }
        else if (currentAccount.availableBalance < 20)
        {
            cout<<"\nInvalid selection. Try again.\n";
            cout << "\nPlease choose a smaller amount.\n";
            withdrawal(currentAccount, remainingBills);
        }
        else
        {
            remainingBills--;
            currentAccount.availableBalance -= 20;
            currentAccount.totalBalance -= 20;
            cout << "\nPlease take your cash from the cash dispenser.\n";
        }
    }
    else if (cmd == 2)
    {
        if (remainingBills == 0)
        {
            cout << "\nInsufficient cash available in the ATM.\n";
            cout << "\nPlease choose a smaller amount.\n";
        }
        else if (currentAccount.availableBalance < 40)
        {
            cout << "\nInvalid selection. Try again.\n";
            cout << "\nPlease choose a smaller amount.\n";
            withdrawal(currentAccount, remainingBills);
        }
        else
        {
            remainingBills -= 2;
            currentAccount.availableBalance -= 40;
            currentAccount.totalBalance -= 40;
            
            cout << "\nPlease take your cash from the cash dispenser.\n";
        }
    }
    else if (cmd == 3)
    {
        if (remainingBills == 0)
        {
            cout << "\nInsufficient cash available in the ATM.\n";
            cout << "\nPlease choose a smaller amount.\n";
        }
        else if (currentAccount.availableBalance < 60)
        {
            cout << "\nInvalid selection. Try again.\n";
            cout << "\nPlease choose a smaller amount.\n";
            withdrawal(currentAccount, remainingBills);
        }
        else
        {
            remainingBills--;
            currentAccount.availableBalance -= 60;
            currentAccount.totalBalance -= 60;
            cout << "\nPlease take your cash from the cash dispenser.\n";
        }
    }
    else if (cmd == 4)
    {
        if (remainingBills == 0)
        {
            cout << "\nInsufficient cash available in the ATM.\n";
            cout << "\nPlease choose a smaller amount.\n";
        }
        else if (currentAccount.availableBalance < 100)
        {
            cout << "\nInvalid selection. Try again.\n";
            cout << "\nPlease choose a smaller amount.\n";
            withdrawal(currentAccount, remainingBills);
        }
        else
        {
            remainingBills -= 5;
            currentAccount.availableBalance -= 100;
            currentAccount.totalBalance -= 100;
            cout << "\nPlease take your cash from the cash dispenser.\n";
        }
    }
    else if (cmd == 5)
    {
        cout << "\nInsufficient funds in your account.\n";
        cout << "\nPlease choose a smaller amount.\n";
        withdrawal(currentAccount, remainingBills);
    }
    else if (cmd == 6)
    {
        cout << "\nCanceling transaction...\n";
        return;
    }
    
    else
    {
        cout << "\nInvalid selection. Try again." << endl;
        withdrawal(currentAccount, remainingBills);
    }

} // end function withdrawal

// perform deposit transaction
void deposit(Account& currentAccount)
{
    cout << "\nPlease enter a deposit amount in dollars (or 0 to cancel): ";
    int amount;
    cin >> amount; // receive input of deposit amount

    // check whether user entered a deposit amount or canceled
    if (amount == 6)
        cout << "\nCanceling transaction...\n";
    else // user canceled instead of entering amount
    {
        // request deposit envelope containing specified amount
        cout << "\nPlease insert a deposit envelope containing ";
        cout << "$" << amount;
        cout << " in the deposit slot.\n";

        cout << "\nYour envelope has been received."
            << "\nNOTE: The money deposited will not be available until we"
            << "\nverify the amount of any enclosed cash, and any enclosed checks clear.\n";

        // credit account to reflect the deposit
        currentAccount.totalBalance += amount; // add to total balance
    } // end if 
} // end function deposit

void saveAccount(Account currentAccount, int n)
{
    fstream outFile("Accounts.dat", ios::in | ios::out | ios::binary);
    if (!outFile)
    {
        cout << "File could be open\n";
        exit(1);
    }

    outFile.seekg(8 + 16 * n, ios::beg);

    outFile.write(reinterpret_cast<char*>(&currentAccount.availableBalance), sizeof(currentAccount.availableBalance));
    outFile.write(reinterpret_cast<char*>(&currentAccount.totalBalance), sizeof(currentAccount.totalBalance));

    outFile.close();
}