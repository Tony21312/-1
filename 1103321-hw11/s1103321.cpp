#include <iostream>
using std::cin;
using std::cout;
using std::endl;

#include <iomanip>
using std::setw;
using std::setfill;

#include <fstream>
using std::ifstream;
using std::ofstream;
using std::ios;
//#include <ctime>

#include <vector>
using std::vector;

struct Date
{
	int year = 0;
	int month = 0;
	int day = 0;
};

struct AvailRoom
{
	Date date;
	int numAvailRooms[6] = {}; // number of available rooms
	// numAvailRooms[ 0 ]: not used
	// numAvailRooms[ 1 ]: the number of available superior rooms
	// numAvailRooms[ 2 ]: the number of available deluxe rooms
	// numAvailRooms[ 3 ]: the number of available deluxe twin rooms
	// numAvailRooms[ 4 ]: the number of available superior suites
	// numAvailRooms[ 5 ]: the number of available deluxe suites
};

struct Reservation
{
	char id[12] = "";
	char name[12] = "";
	char mobile[12] = "";
	int roomType = 0; // 1: superior rooms, 2: deluxe rooms, 3: deluxe twin rooms,
					  // 4: superior suites, 5: deluxe suites
	int numRooms = 0; // number of rooms reserved
	Date checkInDate;
	Date checkOutDate;
};

const int roomRate[6] = { 0, 5390, 6270, 6270, 6820, 8470 }; // room rate per night for each room type
const int totalNumRooms[6] = { 0, 9, 5, 3, 5, 4 }; // total number of rooms for each type
const char roomTypeName[6][20] = { "", "Superior Room", "Deluxe Room", "Deluxe Twin Room",
										   "Superior Suite", "Deluxe Suite" };
// return the current date
Date computeCurrentDate();

// return true if and only if year is a leap year
inline bool leapYear(int year);

// return true if and only if date1 == date2
bool equal(Date date1, Date date2);

// return true if and only if date1 < date2
bool less(const Date& date1, const Date& date2);

// return true if and only if date1 <= date2
bool lessEqual(Date date1, Date date2);

// read reservations information whose checkInDate are after or equal to currentDate,
// from the file Reservations.dat
void loadReservations(vector< Reservation >& reservations, const Date& currentDate);

// load all available rooms information after or equal to currentDate,
// from the file Available Rooms.dat
void loadAvailRooms(vector< AvailRoom >& availRooms, const Date& currentDate);

// create available rooms information for 6 months, starting from currentDate
// provided that availRooms.size() == 0
void initAvailRooms(vector< AvailRoom >& availRooms, const Date& currentDate);

// append new available rooms information, provided that availRooms.size() != 0
void completeAvailRooms(vector< AvailRoom >& availRooms, const Date& currentDate);

// input an integer in the range [ begin, end ]
int inputAnInteger(int begin, int end);

void makeReservation(vector< AvailRoom >& availRooms, vector< Reservation >& reservations, const Date& currentDate);

// input check in date and check out date by user, and
// put them into reservation.checkInDate and reservation.checkOutDate, respectively.
void inputDates(const Date& currentDate, Reservation& newReservation);

// return the minimum between availRooms[ first .. last ].numAvailRooms[ roomType ],
// where availRooms[ first ].date == reservation.checkInDate and
// availRooms[ last ].date == reservation.checkOutDate.
int compMinNumRooms(const Reservation& reservation, vector< AvailRoom >& availRooms, int roomType);

void displayAvailRooms(const AvailRoom& availableRoom);

void displayReservations(const Reservation& reservation);

// return the number of nights for reservation
int numNights(Reservation reservation);

void reservationInquiry(const vector< Reservation >& reservations, const Date& currentDate);

bool exist(const vector< Reservation >& reservations, const Date& currentDate, char id[]);

// save all available rooms information into the file Available Rooms.dat
void saveAvailRooms(const vector< AvailRoom >& availRooms);

// save all reservations information into the file Reservations.dat
void saveReservations(const vector< Reservation >& reservations);

int main()
{
	cout << "Evergreen Laurel Hotel Online Reservation System\n";

	srand(static_cast<unsigned int>(time(0)));
	Date currentDate = computeCurrentDate();

	vector< Reservation > reservations;
	loadReservations(reservations, currentDate); // load all reservation informations

	vector< AvailRoom > availRooms;
	loadAvailRooms(availRooms, currentDate); // load all available rooms information

 //   for( size_t i = 0; i < availRooms.size(); i++ )
 //      displayAvailRooms( availRooms[ i ] );

	if (availRooms.size() == 0)
		initAvailRooms(availRooms, currentDate); // create available rooms information for 6 months
	else
		completeAvailRooms(availRooms, currentDate); // append new available rooms information

	bool userExited = false; // user has not chosen to exit

	// loop while user has not chosen option to exit system
	while (!userExited)
	{
		// show main menu and get user selection
		cout << "\n1 - Making Reservation\n";
		cout << "2 - Reservation Inquiry\n";
		cout << "3 - End\n\n";

		int mainMenuSelection;
		do cout << "Enter your choice: (1~3): ";
		while ((mainMenuSelection = inputAnInteger(1, 3)) == -1);

		Date temp;
		// decide how to proceed based on user's menu selection
		switch (mainMenuSelection)
		{
		case 1:
			makeReservation(availRooms, reservations, currentDate); // make reservation
			break;

		case 2:
			reservationInquiry(reservations, currentDate); // reservation inquiry
			break;

		case 3: // user chose to terminate session
			saveAvailRooms(availRooms);
			saveReservations(reservations);
			userExited = true; // this session should end
			cout << endl;
			break;

		default: // user did not enter an integer from 1-3
			cout << "\nIncorrect choice!" << endl;
		}
	}

	system("pause");
}

Date computeCurrentDate()
{
	tm structuredTime;
	time_t rawTime = time(0);
	localtime_s(&structuredTime, &rawTime);

	Date currentDate;
	currentDate.year = structuredTime.tm_year + 1900;
	currentDate.month = structuredTime.tm_mon + 1;
	currentDate.day = structuredTime.tm_mday;

	return currentDate;
}

inline bool leapYear(int year)
{
	return (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0));
}

bool equal(Date date1, Date date2)
{
	return (date1.year == date2.year &&
		date1.month == date2.month &&
		date1.day == date2.day);
}

bool less(const Date& date1, const Date& date2)
{
	if (date1.year < date2.year)
		return true;
	if (date1.year > date2.year)
		return false;

	if (date1.month < date2.month)
		return true;
	if (date1.month > date2.month)
		return false;

	if (date1.day < date2.day)
		return true;
	else
		return false;
}

bool lessEqual(Date date1, Date date2)
{
	return less(date1, date2) || equal(date1, date2);
}

void loadReservations(vector< Reservation >& reservations, const Date& currentDate)
{
	ifstream inFile("Reservations.dat", ios::in | ios::out | ios::binary);
	if (!inFile)
	{
		cout << "Reservations.dat could not be open" << endl;

	}

	Reservation temp;
	while (inFile.read(reinterpret_cast<char*>(&temp), sizeof(temp)))
	{
		if (less(currentDate, temp.checkInDate))
		{
			reservations.push_back(temp);
		}
	}
/*	for (int n = 0; n < reservations.size(); n++)
		cout << reservations[n].checkInDate.year << "  " << reservations[n].checkInDate.month << "  " << reservations[n].checkInDate.day << endl;
	*/
	inFile.close();
}

void loadAvailRooms(vector< AvailRoom >& availRooms, const Date& currentDate)
{
	ifstream inFile("Available Rooms.dat", ios::in | ios::out | ios::binary);
	if (!inFile)
	{
		cout << "Available Rooms.dat could not be open" << endl;

	}

	AvailRoom temp;
	while (inFile.read(reinterpret_cast<char*>(&temp), sizeof(temp)))
	{
		if (less(currentDate, temp.date))
		{
			availRooms.push_back(temp);
		}
	}
	inFile.close();
}

void initAvailRooms(vector< AvailRoom >& availRooms, const Date& currentDate)
{
	// number of days in each month
	int days[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	
	if (leapYear(currentDate.year))
		days[2] = 29;

	int totalDays = 0;

	for (int n = currentDate.month; n <= 6; n++)
		totalDays += days[n];

	totalDays -= currentDate.day;

	AvailRoom temp;

	for (int n = 0; n <= totalDays; n++)
	{
		for(int k = 1; k <= 5; k++)
		{
			temp.date.year = currentDate.year;
			temp.date.month = currentDate.month;
			temp.date.day = currentDate.day + n;

			while (temp.date.day > days[temp.date.month])
			{
				temp.date.day -= days[temp.date.month];
				temp.date.month++;

				if (temp.date.month > 12)
				{
					temp.date.month -= 12;
					temp.date.year++;

					if (leapYear(temp.date.year))
						days[2] = 29;
					else
						days[2] = 28;
				}
			}
			temp.numAvailRooms[k] = totalNumRooms[k];

		}
		availRooms.push_back(temp);
	}
}

void completeAvailRooms(vector< AvailRoom >& availRooms, const Date& currentDate)
{
	// number of days in each month
	int days[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (leapYear(currentDate.year))
		days[2] = 29;

	int totalDays = 0;

	for (int n = currentDate.month; n <= 6; n++)
		totalDays += days[n];

	totalDays -= currentDate.day;

	AvailRoom temp;

	for (int n = 0; n <= totalDays; n++)
	{
		for (int k = 1; k <= 5; k++)
		{
			temp.date.year = currentDate.year;
			temp.date.month = currentDate.month;
			temp.date.day = currentDate.day + n;

			while (temp.date.day > days[temp.date.month])
			{
				temp.date.day -= days[temp.date.month];
				temp.date.month++;

				if (temp.date.month > 12)
				{
					temp.date.month -= 12;
					temp.date.year++;

					if (leapYear(temp.date.year))
						days[2] = 29;
					else
						days[2] = 28;
				}
			}
			temp.numAvailRooms[k] = totalNumRooms[k];

		}			
		if(less(availRooms[availRooms.size() - 1].date, temp.date))
			availRooms.push_back(temp);
	}
}

int inputAnInteger(int begin, int end)
{
	char string[80];
	cin.getline(string, 80, '\n');

	if (strlen(string) == 0)
		exit(0);

	for (unsigned int i = 0; i < strlen(string); i++)
		if (string[i] < '0' || string[i] > '9')
			return -1;

	int number = atoi(string);

	if (number >= begin && number <= end)
		return number;
	else
		return -1;
}

void makeReservation(vector< AvailRoom >& availRooms, vector< Reservation >& reservations, const Date& currentDate)
{
	Reservation newReservation;

	// input check in date and check out date by user, and
	// put them into reservation.checkInDate and reservation.checkOutDate, respectively.
	inputDates(currentDate, newReservation);

	cout << "\nCheck in date: " << newReservation.checkInDate.year << '-'
		<< setw(2) << setfill('0') << newReservation.checkInDate.month << '-'
		<< setw(2) << newReservation.checkInDate.day << endl;

	cout << "Check out date: " << newReservation.checkOutDate.year << '-'
		<< setw(2) << newReservation.checkOutDate.month << '-'
		<< setw(2) << newReservation.checkOutDate.day << endl;


	// input room type and put it into newReservation.roomType

	cout << "The number of available rooms of each room type:\n\n";
	cout << setfill(' ') << setw(10) << "Date"
		<< setw(16) << setfill(' ') << "Superior Room"
		<< setw(14) << "Deluxe Room"
		<< setw(19) << "Deluxe Twin Room"
		<< setw(17) << "Superior Suite"
		<< setw(15) << "Deluxe Suite" << endl;

	for (int n = 0; n < availRooms.size(); n++)
	{
		if (equal(newReservation.checkOutDate, availRooms[n].date))
			break;
		if(lessEqual(newReservation.checkInDate, availRooms[n].date))
			displayAvailRooms(availRooms[n]);
	}
	cout << "\nSelect Room Type¡G\n";


	int numOfRoom[6] = {};
	for (int n = 1; n < 6; n++)
		numOfRoom[n] = compMinNumRooms(newReservation, availRooms, n);

	int num = 1;
	if(numOfRoom[num] > 0)
		cout << num << ". Superior Room\n";
	num++;
	if (numOfRoom[num] > 0)
		cout << num << ". Deluxe Room\n";
	num++;
	if (numOfRoom[num] > 0)
		cout << num << ". Deluxe Twin Room\n";
	num++;
	if (numOfRoom[num] > 0)
		cout << num << ". Superior Suite\n";
	num++;
	if (numOfRoom[num] > 0)
		cout << num << ". Deluxe Suite\n";
	num++;
	cout << num << ". Give up\n";

	do 
	{
		cout << "? ";
		cin >> newReservation.roomType;
		cin.ignore();
	} while (newReservation.roomType > 6 || newReservation.roomType < 1);

	if (newReservation.roomType == 6)
		return;

	// input number of rooms and put it into newReservation.numRooms
	do
	{
		cout << "\nNumber of rooms ( " << numOfRoom[newReservation.roomType] << " rooms available ):";
		cin >> newReservation.numRooms;
		cin.ignore();

	} while (newReservation.numRooms > numOfRoom[newReservation.roomType]);
	
	for (int n = 0; n < availRooms.size(); n++)
	{
		if (equal(availRooms[n].date, newReservation.checkOutDate))
			break;
		if (lessEqual(newReservation.checkInDate, availRooms[n].date))
			availRooms[n].numAvailRooms[newReservation.roomType] -= newReservation.numRooms;
	}

	cout << "\nID Number¡G";
	cin >> newReservation.id;
	cout << "\nName¡G";
	cin >> newReservation.name;
	cout << "\nMobile Phone¡G";
	cin >> newReservation.mobile;
	cin.ignore();

	cout << "\nYour reservations:\n\n";
	cout << "    Name   Fare       Mobile          Room type"
		<< "   Number of rooms   Check in date   Check out date\n";
	displayReservations(newReservation);

	reservations.push_back(newReservation);
}

void inputDates(const Date& currentDate, Reservation& newReservation)
{
//--------------------input check in date-------------------------------
	int year = currentDate.year;
	int month = currentDate.month;
	int days[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	cout << "\nPlease Input Check In Date\n";
	cout << "Month¡G\n";
	for (int n = 1; n <= 6; n++)
	{
		cout << n <<". " << year << " - " << setfill('0') << setw(2) << month++ << endl;
		if (month > 12)
		{
			month -= 12;
			year++;
		}
	}
	if (leapYear(year))
		days[2] = 29;

	int checkInNum;
	do
	{
		cout << "? ";
		cin >> checkInNum;
	} while (checkInNum > 6 || checkInNum < 1);

	newReservation.checkInDate.month = currentDate.month + checkInNum - 1;
	newReservation.checkInDate.year = currentDate.year;
	if (newReservation.checkInDate.month > 12)
	{
		newReservation.checkInDate.month -= 12;
		newReservation.checkInDate.year++;
	}

	if (newReservation.checkInDate.month == currentDate.month)
	{
		do
		{
			cout << "\nDay( " << currentDate.day << " ~ " << days[newReservation.checkInDate.month] << " ): ";
			cin >> newReservation.checkInDate.day;
		} while (newReservation.checkInDate.day > days[newReservation.checkInDate.month] || newReservation.checkInDate.day < currentDate.day);
	}
	else if (newReservation.checkInDate.month == currentDate.month + 6 - 1)
	{
		do
		{
			cout << "\nDay( 1 ~ " << days[newReservation.checkInDate.month] - 1 << " ): ";
			cin >> newReservation.checkInDate.day;
		} while (newReservation.checkInDate.day > days[newReservation.checkInDate.month] - 1 || newReservation.checkInDate.day < 1);
	}
	else
	{
		do
		{
			cout << "\nDay( 1 ~ " << days[newReservation.checkInDate.month] << " ): ";
			cin >> newReservation.checkInDate.day;
		} while (newReservation.checkInDate.day > days[newReservation.checkInDate.month] || newReservation.checkInDate.day < 1);
	}

	
//--------------------input check out date-------------------------------
	year = currentDate.year;
	month = currentDate.month;
	days[2] = 28;

	cout << "\nPlease Input Check Out Date\n";
	cout << "Month¡G\n";
	if (newReservation.checkInDate.month == currentDate.month + 6 - 1 && newReservation.checkInDate.day == days[newReservation.checkInDate.month] - 1)
	{
		for (int n = 1; n <= 6; n++)
		{
			if (month == newReservation.checkInDate.month)
				cout << n << ". " << year << " - " << setfill('0') << setw(2) << month++ << endl;
			else
				month++;

			if (month > 12)
			{
				month -= 12;
				year++;
			}
		}
	}

	else if (newReservation.checkInDate.day != days[newReservation.checkInDate.month])
	{
		for (int n = 1; n <= 6; n++)
		{
			cout << n << ". " << year << " - " << setfill('0') << setw(2) << month++ << endl;
			if (month > 12)
			{
				month -= 12;
				year++;
			}
		}
	}
	else
	{
		for (int n = 1; n <= 6; n++)
		{
			if(month > newReservation.checkInDate.month)
				cout << n << ". " << year << " - " << setfill('0') << setw(2) << month++ << endl;
			else
				month++;
			
			if (month > 12)
			{
				month -= 12;
				year++;
			}
		}
	}
	if (leapYear(year))
		days[2] = 29;

	int checkOutNum;
	do
	{
		cout << "? ";
		cin >> checkOutNum;
	} while (checkOutNum > 6 || checkOutNum < 1);

	newReservation.checkOutDate.month = currentDate.month + checkOutNum - 1;
	newReservation.checkOutDate.year = currentDate.year;
	if (newReservation.checkOutDate.month > 12)
	{
		newReservation.checkOutDate.month -= 12;
		newReservation.checkOutDate.year++;
	}

	if (newReservation.checkOutDate.month == currentDate.month)
	{
		do
		{
			cout << "\nDay( " << newReservation.checkInDate.day + 1 << " ~ " << days[newReservation.checkInDate.month] << " ): ";
			cin >> newReservation.checkOutDate.day;
		} while (newReservation.checkOutDate.day > days[newReservation.checkInDate.month] || newReservation.checkOutDate.day < newReservation.checkInDate.day + 1);
	}
	else if (newReservation.checkOutDate.month == currentDate.month + 6 - 1 && newReservation.checkOutDate.day == days[newReservation.checkInDate.month] - 1)
	{
		do
		{
			cout << "\nDay( "<< days[newReservation.checkInDate.month]<<" ~ "  << days[newReservation.checkInDate.month] << " ): ";
			cin >> newReservation.checkOutDate.day;
		} while (newReservation.checkOutDate.day > days[newReservation.checkInDate.month] - 1 || newReservation.checkOutDate.day < 1);
	}
	else
	{
		do
		{
			cout << "\nDay( 1 ~ " << days[newReservation.checkOutDate.month] << " ): ";
			cin >> newReservation.checkOutDate.day;
		} while (newReservation.checkOutDate.day > days[newReservation.checkInDate.month] || newReservation.checkOutDate.day < 1);
	}

}

int compMinNumRooms(const Reservation& reservation, vector< AvailRoom >& availRooms, int roomType)
{
	int num = 10;

	for (int n = 0; n < availRooms.size(); n++)
	{		
		if (equal(reservation.checkOutDate, availRooms[n].date))
			break;
		if(lessEqual(reservation.checkInDate, availRooms[n].date))
			if (num > availRooms[n].numAvailRooms[roomType])
				num = availRooms[n].numAvailRooms[roomType];
	}

//	cout << num << endl;
	return num;
}

void displayAvailRooms(const AvailRoom& availableRoom)
{
	cout << availableRoom.date.year << "-"
		<< setw(2) << setfill('0') << availableRoom.date.month << "-"
		<< setw(2) << availableRoom.date.day
		<< setw(16) << setfill(' ') << availableRoom.numAvailRooms[1]
		<< setw(14) << availableRoom.numAvailRooms[2]
		<< setw(19) << availableRoom.numAvailRooms[3]
		<< setw(17) << availableRoom.numAvailRooms[4]
		<< setw(15) << availableRoom.numAvailRooms[5] << endl;
}

void displayReservations(const Reservation& reservation)
{
	cout << setw(8) << setfill(' ') << reservation.name
		<< setw(7) << reservation.numRooms * roomRate[reservation.roomType] * numNights(reservation)
		<< setw(13) << reservation.mobile
		<< setw(19) << roomTypeName[reservation.roomType]
		<< setw(18) << reservation.numRooms
		<< setw(10) << reservation.checkInDate.year << "-"
		<< setw(2) << setfill('0') << reservation.checkInDate.month << "-"
		<< setw(2) << reservation.checkInDate.day
		<< setw(11) << setfill(' ') << reservation.checkOutDate.year << "-"
		<< setw(2) << setfill('0') << reservation.checkOutDate.month << "-"
		<< setw(2) << reservation.checkOutDate.day << endl;
}

int numNights(Reservation reservation)
{
	tm time1 = { 0, 0, 8, 0, 0, 0, 0, 0, 0 };
	tm time2 = { 0, 0, 8, 0, 0, 0, 0, 0, 0 };
	time1.tm_year = reservation.checkInDate.year - 1900;
	time1.tm_mon = reservation.checkInDate.month - 1;
	time1.tm_mday = reservation.checkInDate.day;
	time2.tm_year = reservation.checkOutDate.year - 1900;
	time2.tm_mon = reservation.checkOutDate.month - 1;
	time2.tm_mday = reservation.checkOutDate.day;
	int numSeconds = static_cast<int>(mktime(&time2) - mktime(&time1));
	return numSeconds / (24 * 3600);
}

void reservationInquiry(const vector< Reservation >& reservations, const Date& currentDate)
{
	cout << "\nEnter Your ID Number¡G";
	char id[12];
	cin >> id;
	cin.ignore();

	if (!exist(reservations, currentDate, id))
	{
		cout << "\nSorry! You do not have reservations!\n";
		return;
	}

	cout << "\n    Name   Fare       Mobile          Room type"
		<< "   Number of rooms   Check in date   Check out date\n";
	for (size_t i = 0; i < reservations.size(); i++)
		if (strcmp(reservations[i].id, id) == 0)
			displayReservations(reservations[i]);
}

bool exist(const vector< Reservation >& reservations, const Date& currentDate, char id[])
{
	for (size_t i = 0; i < reservations.size(); i++)
		if (strcmp(reservations[i].id, id) == 0 &&
			lessEqual(currentDate, reservations[i].checkInDate))
			return true;

	return false;
}

void saveAvailRooms(const vector< AvailRoom >& availRooms)
{
	ofstream outFile("Available Rooms.dat", ios::in | ios::out | ios::binary);
	if (!outFile)
	{
		cout << "File could not be open" << endl;

	}
	for (int n = 0; n < availRooms.size(); n++)
	{
		outFile.write(reinterpret_cast<const char*>(&availRooms[n]), sizeof(availRooms[n]));
	}
	outFile.close();
}

void saveReservations(const vector< Reservation >& reservations)
{
	ofstream outFile("Reservations.dat", ios::in | ios::out | ios::binary);
	if (!outFile)
	{
		cout << "File could not be open" << endl;

	}
	for (int n = 0; n < reservations.size(); n++)
	{
		outFile.write(reinterpret_cast<const char*>(&reservations[n]), sizeof(reservations[n]));
	}
	outFile.close();
}