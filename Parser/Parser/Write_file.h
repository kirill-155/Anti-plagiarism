#pragma once
#include "Header.h"
#include "Type_program_language.h"

void write_info_stud(string Name_student, vector<Solution_stud>& Stud)
{
	string file_info = Folder_base_put + Folder_base_stud;
	file_info += Name_student + '/';
	file_info += Name_student + ".txt";
	ofstream out(file_info);
	for (int i = 0; i < Stud.size(); i++) 
	{
		out << Stud[i].Name_contest << " ";
		out << Stud[i].Name_package << " ";
		out << Stud[i].verdict << " ";
		if (Stud[i].verdict=="Coincidence")
		{
			out << Stud[i].Name_stud_dist_levenstein << " ";
			out << Stud[i].Name_dist_levenstein_package << " ";
			out << Stud[i].dist_levenstein << " ";
		}
		out << '\n';
	}
	out.close();
}

void write_info_all_stud(vector<pair<string, int>> All_stud)
{
	string file_info_all = Folder_base_put + "result.txt";
	ofstream out(file_info_all);
	for (int i = 0 ; i < All_stud.size(); i++) 
	{
		out << All_stud[i].first << " : " << All_stud[i].second << '\n';
	}
	out.close();
}

// Компаратор по дистанции Левенштейна
bool dist_lev_sort(Solution_stud& a, Solution_stud& b) {
	return a.dist_levenstein < b.dist_levenstein;
}

// Вывод пар решений с наименьшим расстоянием Левенштейна
void write_info_cheat_stud_levenstein(vector<pair<string, vector<pair<string, map<char, vector<string>>>>>>& puths, vector<Solution_stud>& sol_stud, int id_contest)
{
	sort(sol_stud.begin(), sol_stud.end(), dist_lev_sort);
	set<string> st;
	ofstream out(Folder_result + "\\" + puths[id_contest].first + " Levenstein.txt");

	// собираем подходящие строки и считаем ширину каждого столбца
	vector<vector<string>> rows;
	int width[5] = { 0, 0, 0, 0, 0 };

	for (int i = 0; i < sol_stud.size(); i++)
	{
		if (sol_stud[i].dist_levenstein <= DIST_LEVENSTEIN[type_leng(sol_stud[i].Name_compiler)] && sol_stud[i].Name_stud_dist_levenstein != ""
			&& !st.count(sol_stud[i].Name_stud + sol_stud[i].Name_stud_dist_levenstein + sol_stud[i].Name_package[0]))
		{
			st.insert(sol_stud[i].Name_stud_dist_levenstein + sol_stud[i].Name_stud + sol_stud[i].Name_package[0]);

			vector<string> row = {
				sol_stud[i].Name_stud + "\\" + sol_stud[i].Name_package,
				"Coincidence",
				sol_stud[i].Name_stud_dist_levenstein + "\\" + sol_stud[i].Name_dist_levenstein_package,
				to_string(sol_stud[i].dist_levenstein),
				sol_stud[i].Name_compiler
			};
			for (int k = 0; k < 5; k++)
				width[k] = max(width[k], (int)row[k].size());
			rows.push_back(row);
		}
	}

	// выводим, выравнивая каждый столбец по его ширине (+2 пробела зазор)
	for (const auto& row : rows)
	{
		for (int k = 0; k < 4; k++)
			out << left << setw(width[k] + 2) << row[k];
		out << row[4] << "\n";
	}
	out.close();
}

// Компаратор по длинне коментариев
bool num_com_sort(Solution_stud& a, Solution_stud& b) {
	return a.Number_comments > b.Number_comments;
}

// Вывод количества комментариев для решений
void write_info_cheat_stud_comments(vector<pair<string, vector<pair<string, map<char, vector<string>>>>>>& puths, vector<Solution_stud>& sol_stud, int id_contest)
{
	sort(sol_stud.begin(), sol_stud.end(), num_com_sort);
	ofstream out_comment(Folder_result + "\\" + puths[id_contest].first + " Comment " + to_string(NUM_COMMENTS) + ".txt");

	// собираем подходящие строки и считаем ширину каждого столбца
	vector<vector<string>> rows;
	int width[3] = { 0, 0, 0 };

	for (int i = 0; i < sol_stud.size(); i++)
	{
		if (sol_stud[i].Number_comments > NUM_COMMENTS)
		{
			vector<string> row = {
				sol_stud[i].Name_stud + "\\" + sol_stud[i].Name_package,
				"Great comment",
				to_string(sol_stud[i].Number_comments)
			};
			for (int k = 0; k < 3; k++)
				width[k] = max(width[k], (int)row[k].size());
			rows.push_back(row);
		}
	}

	// выводим, выравнивая каждый столбец по его ширине (+2 пробела зазор)
	for (const auto& row : rows)
	{
		for (int k = 0; k < 2; k++)		
			out_comment << left << setw(width[k] + 2) << row[k];
		out_comment << row[2] << "\n";
	}
	out_comment.close();
}

// Вывод результатов.
void write_info_cheat_stud(vector<pair<string, vector<pair<string, map<char, vector<string>>>>>>& puths, vector<Solution_stud>& sol_stud, int id_contest)
{
	fs::create_directories(Folder_result);

	write_info_cheat_stud_levenstein(puths, sol_stud, id_contest);
	write_info_cheat_stud_comments(puths, sol_stud, id_contest);
}
