#pragma once
#include "Header.h"

void Result_all_stud(
	string Name_student,
	vector<Solution_stud>& sol_stud,
	vector<pair<string, int>>& All_stud)
{
	pair<string, int>info_one_stud;
	info_one_stud.first = Name_student;
	int count_accept = 0;
	for (int i = 0; i < sol_stud.size(); i++)
	{
		if (sol_stud[i].verdict == "OK")
		{
			count_accept++;
		}
		if (sol_stud[i].verdict == "Coincidence" &&
			sol_stud[i].dist_levenstein > DIST_LEVENSTEIN)
		{
			count_accept++;
		}
	}
	info_one_stud.second = count_accept;
	All_stud.push_back(info_one_stud);
}

string Generate_path_solution(string& Name_number_contest, string& Name_student, string& Name_tasks)
{
	string path = Folder_base_put + '/';
	path += Name_number_contest + '/';
	path += Name_student + "/OK/";
	path += Name_tasks;
	return path;
}

string Generate_path_coach(string Name_number_contest,
	string Name_tasks)
{
	string path = Folder_base_put + Folder_base_stud;
	path += Name_number_contest + '/';
	path += Nick_name_coach + "/OK/";
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

// Проверяет язык написания кода. Возвращает название языка программирования
string identif_compil(const string& code) {
	int cpp = 0, cs = 0, py = 0;
	const char* d = code.data();
	int n = code.size();

	auto match = [&](int i, const char* p, int len) {
		return i + len <= n && std::memcmp(d + i, p, len) == 0;
		};

	int limit = std::min(n, 500);
	for (size_t i = 0; i < limit || cpp < 4 || cs < 4 || py < 4; ++i) {
		switch (d[i]) {
		case '#':
			if (match(i, "#include", 8))              cpp += 5;
			break;
		case 's':
			if (match(i, "std::", 5))                 cpp += 4;
			else if (match(i, "self.", 5))            py += 2;
			break;
		case 'u':
			if (match(i, "using System", 12))         cs += 5;
			else if (match(i, "using namespace std", 19)) cpp += 3;
			break;
		case 'c':
			if (match(i, "cout", 4))                  cpp += 3;
			else if (match(i, "cin", 3))              cpp += 2;
			else if (match(i, "Console.", 8))         cs += 4;
			else if (match(i, "class ", 6))           cs += 1; 
			break;
		case 'd':
			if (match(i, "def ", 4))                  py += 3;
			break;
		case 'e':
			if (match(i, "elif ", 5))                 py += 4;
			break;
		case 'p':
			if (match(i, "print(", 6))                py += 2;
			break;
		case 'n':
			if (match(i, "nullptr", 7))               cpp += 2;
			else if (match(i, "namespace ", 10))      cs += 2;
			break;
		case 't':
			if (match(i, "template<", 9) ||
				match(i, "template <", 10))           cpp += 3;
			break;
		case '_':
			if (match(i, "__init__", 8))              py += 4;
			break;
		case 'T':
			if (match(i, "True", 4))                  py += 1;
			break;
		case 'F':
			if (match(i, "False", 5))                 py += 1;
			break;
		case 'N':
			if (match(i, "None", 4))                  py += 1;
			break;
		case 'M':
			if (match(i, "Main(", 5))                 cs += 2;
			break;
		}
	}

	// Победитель должен и набрать минимум, и строго опережать остальных
	if (cpp >= 3 && cpp > cs && cpp > py) return "C++";
	if (cs >= 3 && cs > cpp && cs > py) return "C#";
	if (py >= 3 && py > cpp && py > cs) return "Python";
	return "Unknown";
}