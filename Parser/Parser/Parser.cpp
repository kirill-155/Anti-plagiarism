#include "Header.h"
#include "Write_file.h"
#include "Levenstein.h"
#include "Scanf_file.h"

int main()
{
	auto start = chrono::steady_clock::now();
	int count_comparisons = 0;

	setlocale(LC_ALL, "Russian");
	vector<pair<string, vector<pair<string, map<char, vector<string>>>>>> puths;// контест, студенты, задачи
	scanf_all_info(puths);

	for (int id_contest = 0; id_contest < puths.size(); id_contest++) // Перебираем по контестам
	{
		map<char, vector<Solution_stud>> sol_stud;
		for (int id_stud = 0; id_stud < puths[id_contest].second.size(); id_stud++) // Перебираем по студентам
		{
			for (pair<char, vector<string>> tasks : puths[id_contest].second[id_stud].second) // Перебираем по заданиям
			{
				for (string task : tasks.second) // Перебираем по решениям задач
				{
					Solution_stud sol;
					scan_info_solution(sol, puths, id_contest, id_stud, task);

					for (Solution_stud& sol_con : sol_stud[task[0]])	 // Перебираем по заданиям с конкретной буквой
					{
						if (sol.Name_compiler == sol_con.Name_compiler && sol.Name_stud != sol_con.Name_stud)
						{
							count_comparisons++;
							int dist = Algo_levenstein(sol.Solution, sol_con.Solution, type_leng(sol.Name_compiler));
							if (sol.dist_levenstein > dist)
							{
								sol.dist_levenstein = dist;
								sol.Name_stud_dist_levenstein = sol_con.Name_stud;
								sol.Name_dist_levenstein_package = sol_con.Name_package;
							}
							if (sol_con.dist_levenstein > dist)
							{
								sol_con.dist_levenstein = dist;
								sol_con.Name_stud_dist_levenstein = sol.Name_stud;
								sol_con.Name_dist_levenstein_package = task;
							}
						}
					}
					sol_stud[task[0]].push_back(sol);
				}
			}
		}
		int total = 0;
		for (auto& [key, vec] : sol_stud)
			total += vec.size();

		vector<Solution_stud> all_sol;
		all_sol.reserve(total);

		for (auto& [key, vec] : sol_stud)
			all_sol.insert(all_sol.end(), vec.begin(), vec.end());

		write_info_cheat_stud(puths, all_sol, id_contest);
	}

	auto end = chrono::steady_clock::now();

	auto sec = chrono::duration<double>(end - start).count();
	ofstream out("system logs.txt", ios::app);
	out << "Время выполнения: " << sec << " сек. \tКоличество сравниваний: " << count_comparisons <<"\n";
	out.close();
}