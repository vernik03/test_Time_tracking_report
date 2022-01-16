#include <iostream>
#include <fstream>
#include <map>
#include <iomanip>
#include <string>
#include <sstream>
#include <chrono>

using namespace std;

struct Date {
	int year;
	int month;
	string month_str;
};

struct FullName {
	string name;
	string surname;
};

bool operator<(FullName a, FullName b) {
	if (a.name == b.name)
	{
		return a.surname < b.surname;
	}
	else
	{
		return a.name < b.name;
	}
}

bool operator<(Date a, Date b) {
	if (a.year == b.year)
	{
		
		return a.month < b.month;
	}
	else
	{
		return a.year < b.year;
	}
}

class Log {
public:
	Log(string file = "log.txt") {
		filename = file;
	}

	void LoggingStart() {
		f.open(filename, ios::out);
		LoggingAdd("Program started");
	}
	
	void LoggingAdd(string info) {
		t = time(0);
		f << std::ctime(&t) << "\t" << info << std::endl;
	}

	void LoggingFinish() {
		LoggingAdd("Program finished");
		f.close();
	}

	int ReturnYear() {
		t = time(0);
		tm* date = localtime(&t);
		return date->tm_year+1900;
	}

private:
	string filename;
	ofstream f;
	time_t t;
};

class Main_info
{
public:
	Main_info(string filename, char separator = ';') {
		l.LoggingStart();
		AddInInfo(filename, separator);
	}

	void AddInInfo(string filename, char separator = ';') {
		try {
			ifstream in(filename);
			if (in)
			{
				string str_big;
				int name_num_col, date_num_col, hours_num_col, strings_count = -1, col_count = -1, col_num;
				FullName temp_fn;
				Date temp_d;
				int temp_h;
				auto begin = std::chrono::steady_clock::now();
				while (getline(in, str_big)) {
					strings_count++;
					string str;
					stringstream stream_small(str_big);
					while (getline(stream_small, str, separator)) {
						if (strings_count == 0)
						{
							col_num = col_count + 1;
						}
						if (col_count >= col_num && strings_count == 1)
						{
							col_count = -1;
						}

						col_count++;
						if (strings_count == 0)
						{
							if (str == "Name")
							{
								name_num_col = col_count;
							}
							else if (str == "date")
							{
								date_num_col = col_count;
							}
							else if (str == "logged hours")//?
							{
								hours_num_col = col_count;
							}
						}
						else
						{
							if (col_count == name_num_col)
							{
								stringstream stream_temp(str);
								stream_temp >> temp_fn.name >> temp_fn.surname;
							}
							else if (col_count == date_num_col)
							{

								string temp_str = "";
								int temp_counter = 0;
								for (auto s : str) {
									if (s != '-')
									{
										temp_str += s;
									}
									else
									{
										if (temp_counter == 0)
										{

											if (atoi(temp_str.c_str()) <= l.ReturnYear())
											{
												temp_d.year = atoi(temp_str.c_str());
											}
											else
											{
												l.LoggingAdd("Invalid year");
												throw invalid_argument("Invalid year");
											}
										}
										else if (temp_counter == 1)
										{

											temp_d.month = atoi(temp_str.c_str());
											temp_d.month_str = ReturnMonth(temp_d.month);
										}
										temp_str = "";
										temp_counter++;
									}
								}
								if (!CheckDay(atoi(temp_str.c_str()), temp_d.month, temp_d.year))
								{
									l.LoggingAdd("Invalid year");
									throw invalid_argument("Invalid year");
								}
							}
							else if (col_count == hours_num_col)
							{
								stringstream stream_temp(str);
								stream_temp >> temp_h;
							}
						}
						if (col_count >= col_num && strings_count > 0)
						{
							col_count = -1;
							out_info[temp_fn][temp_d] += temp_h;
						}
					}
				}
				auto end = std::chrono::steady_clock::now();
				auto result_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
				l.LoggingAdd("Information added, " + to_string(strings_count) + " lines for " + to_string(result_time.count()) + " microseconds");
			}
			else
			{
				l.LoggingAdd("Open file error");
				throw invalid_argument("Open file error");
			}
		}
		catch (exception& e) {
			cout << e.what() << endl;
			ClearInfo();
			return;
		}
	}

	

	void ClearInfo() {
		out_info.clear();
		l.LoggingAdd("Database cleared");
	}

	void PrintOutInfo() {
		cout << left << setw(15) << "Name: "
			<< setw(14) << "Month "
			<< " - Total hours" << endl << endl;
		for (auto& [name_out, other_info] : out_info) {
			for (auto& [date_out, hours_out] : out_info.at(name_out)) {
				cout << left << setw(15) << name_out.name + " " + name_out.surname + ": "
					<< setw(14) << date_out.month_str + " " + to_string(date_out.year) << " - " 
					<< hours_out << endl;
			}
		}
		cout << endl;
		l.LoggingAdd("Information printed to console");
	}

	void WriteOutInfo(string filename, char separator = ';') {
		ofstream out(filename);
		out << "Name" << separator << "Month" << separator << "Total hours" << endl;
		for (auto& [name_out, other_info] : out_info) {
			for (auto& [date_out, hours_out] : out_info.at(name_out)) {
				out << name_out.name + " " + name_out.surname << separator
					<< date_out.month_str << " " << date_out.year << separator
					<< hours_out << endl;
			}
		}
		l.LoggingAdd("Information printed to file '" + filename + "'");
	}

private:
	map<FullName, map<Date, int>> out_info;
	Log l;

	bool CheckDay(int day, int month, int year) {
		
		return (day > 0 && day <= 31 && (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12))
			|| (day > 0 && day <= 30 && (month == 4 || month == 6 || month == 9 || month == 11))
			|| (day > 0 && day <= 29 && month == 2 && year % 4 == 0)
			|| (day > 0 && day <= 28 && month == 2);
		
	}

	string ReturnMonth(int month) {
		switch (month)
		{
		case 1:
			return "January";
			break;
		case 2:
			return "February";
			break;
		case 3:
			return "March";
			break;
		case 4:
			return "April";
			break;
		case 5:
			return "May";
			break;
		case 6:
			return "June";
			break;
		case 7:
			return "July";
			break;
		case 8:
			return "August";
			break;
		case 9:
			return "September";
			break;
		case 10:
			return "October";
			break;
		case 11:
			return "November";
			break;
		case 12:
			return "December";
			break;
		default:
			l.LoggingAdd("Invalid month");
			throw invalid_argument("Invalid month");
			break;
		}
	}

};



int main()
{
	Main_info m("info.csv");
	m.WriteOutInfo("report.csv");
	m.PrintOutInfo();
	m.AddInInfo("info+.csv", '+');
	m.PrintOutInfo();
	m.ClearInfo();
	m.AddInInfo("info+.csv", '+');
	m.PrintOutInfo();

}
