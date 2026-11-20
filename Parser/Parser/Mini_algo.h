#pragma once
#include "Header.h"

string Generate_path_solution(string& Name_number_contest, string& Name_student, string& Name_tasks)
{
	string path = Folder_base_put + '/';
	path += Name_number_contest + '/';
	path += Name_student + "/";
	if (Scan_mode == 1) path += "OK/";
	path += Name_tasks;
	return path;
}

string Generate_path_coach(string Name_number_contest,
	string Name_tasks)
{
	string path = Folder_base_put + Folder_base_stud;
	path += Name_number_contest + '/';
	path += Nick_name_coach + "/";
	if (Scan_mode == 1) path += "OK/";
	path += Name_tasks + ".txt";
	return path;
}

int Time_solution(string Date)
{
	string day = "";
	day += Date[0];
	day += Date[1];
	string month = "";
	month += Date[3];
	month += Date[4];
	string year = "";
	year += Date[6];
	year += Date[7];
	year += Date[8];
	year += Date[9];


	string clock = "";
	clock += Date[11];
	clock += Date[12];
	string minute = "";
	minute += Date[14];
	minute += Date[15];

	int time = (stoi(year) * 365 + stoi(month) * 31 + stoi(day));
	time = (time * 24 + stoi(clock)) * 60 + stoi(minute);
	return time;
}

void check_time_solution(vector<Solution_stud>& sol_stud)
{
	for (int i = 0; i < sol_stud.size(); i++)
	{
		if (sol_stud[i].verdict != "OK")
		{
			int raz = 10000;
			for (int j = 0; j < sol_stud.size(); j++)
			{
				if (i != j)
				{
					if (raz > abs(sol_stud[i].Time - sol_stud[j].Time))
					{
						raz = abs(sol_stud[i].Time - sol_stud[j].Time);
					}
				}
			}
			if (raz <= 3)
			{
				sol_stud[i].verdict = "Time";
			}
		}
	}
}

// Проверяет названия файлов. С компилятором от с++ возвращает true
bool is_valid_cpp(const string& code) {
	static const regex r(R"((clang14_cpp20)|(gcc14_cpp23))");
	return regex_search(code.data(), r);
}

// Проверяет названия файлов. С компилятором от с# возвращает true
bool is_valid_csh(const string& code) {
	static const regex r(R"((dotnet8))");
	return regex_search(code.data(), r);
}
