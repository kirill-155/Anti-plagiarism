#include "Header.h"
#include "Scanf_file.h"
#include "Write_file.h"
#include "Levenstein.h"
#include "Change_string.h"
#include "Mini_algo.h"
#include "Type_program_language.h"

int main()
{
	auto start = chrono::steady_clock::now();
	int count_comparisons = 0;

	setlocale(LC_ALL, "Russian");
	vector<pair<string, vector<pair<string, map<char, vector<string>>>>>> puths;// контест, студенты, задачи
	scanf_all_info(puths);

	for (int id_contest = 0; id_contest < puths.size(); id_contest++) // Перебираем по контестам
	{
		vector<Solution_stud> sol_stud;
		for (int id_stud = 0; id_stud < puths[id_contest].second.size(); id_stud++) // Перебираем по студентам
		{
			for (pair<char, vector<string>> tasks : puths[id_contest].second[id_stud].second) // Перебираем по заданиям
			{
				for (string task : tasks.second) // Перебираем по решениям задач
				{
					Solution_stud sol;
					string path = Generate_path_solution(puths[id_contest].first,
						puths[id_contest].second[id_stud].first, task);
					sol.Solution = read_file(path);
					sol.Name_stud = puths[id_contest].second[id_stud].first;
					sol.Name_package = task;
					sol.dist_levenstein = 1000;
					string Code = sol.Solution;
					sol.Name_compiler = identif_compil(Code);
					Code = standardize(Code, sol.Name_compiler);
					sol.Solution = change_str(Code, sol.Number_comments, sol.Name_compiler);

					for (int id_stud_con = 0; id_stud_con < id_stud; id_stud_con++)					 // Перебираем по студентам, но только тех кого уже занесли
					{
						for (string task_con : puths[id_contest].second[id_stud_con].second[task[0]])	 // Перебираем по заданиям с конкретной буквой
						{
							Solution_stud& sol_con = sol_stud[id_stud_con];
							if (sol.Name_compiler == sol_con.Name_compiler)
							{
								count_comparisons++;
								int dist = Algo_levenstein(sol.Solution, sol_con.Solution, type_leng(sol.Name_compiler));
								if (sol.dist_levenstein > dist)
								{
									sol.dist_levenstein = dist;
									sol.Name_stud_dist_levenstein = puths[id_contest].second[id_stud_con].first;
									sol.Name_dist_levenstein_package = task_con;
									sol.Id_stud_dist_levenstein = id_stud_con;
								}
								if (sol_con.dist_levenstein > dist)
								{
									sol_con.dist_levenstein = dist;
									sol_con.Name_stud_dist_levenstein = puths[id_contest].second[id_stud].first;
									sol_con.Name_dist_levenstein_package = task;
									sol_con.Id_stud_dist_levenstein = id_stud;
								}
							}
						}
					}
					sol_stud.push_back(sol);
					//cout << path << "\n";
				}
			}
		}
		write_info_cheat_stud(puths, sol_stud, id_contest);
	}

	auto end = std::chrono::steady_clock::now();

	auto sec = chrono::duration<double>(end - start).count();
	ofstream out("system logs.txt", std::ios::app);
	out << "Время выполнения: " << sec << " сек. \tКоличество сравниваний: " << count_comparisons <<"\n";
	out.close();
}