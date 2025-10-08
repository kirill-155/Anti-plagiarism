#include "Header.h"
#include "Scanf_file.h"
#include "Write_file.h"
#include "Levenstein.h"
#include "Change_string.h"
#include "Mini_algo.h"

int main()
{
	setlocale(LC_ALL, "Russian");
	vector<string> Name_number_contest;
	vector<string> Name_tasks;
	vector<string> Name_student;
	vector<string> Name_compiler;
	vector < pair < string, int>> All_stud;
	scanf_all_info(Name_number_contest, Name_tasks, Name_student, Name_compiler);

	//string path;
	for (int id_stud = 0; id_stud < Name_student.size(); id_stud++)
	{
		vector<Solution_stud> sol_stud;
		for (int id_contest = 0; id_contest < Name_number_contest.size(); id_contest++)
		{
			for (int id_task = 0; id_task < Name_tasks.size(); id_task++)
			{
				Solution_stud sol;
				string path = Generate_path_solution(Name_student[id_stud], Name_number_contest[id_contest], Name_tasks[id_task]);
				ifstream fin(path);
				if (fin.is_open())
				{
					scan_info_solution(path, sol);
					sol.Name_contest = Name_number_contest[id_contest];
					sol.Name_problem = Name_tasks[id_task];
					sol.Time = Time_solution(sol.Date);
					sol.verdict = "Coincidence";
					sol.dist_levenstein = 10000;
					string path_coach = Generate_path_coach(Name_number_contest[id_contest], Name_tasks[id_task]);
					ifstream fin_coath(path_coach);
					if (fin_coath.is_open())
					{
						sol.verdict = "OK";
					}
					else
					{
						sol.Solution = change_str(sol.Solution);
						for (int id_stud_con = 0; id_stud_con < Name_student.size(); id_stud_con++)
						{
							if (id_stud != id_stud_con)
							{
								string path_con = Generate_path_solution(Name_student[id_stud_con], Name_number_contest[id_contest], Name_tasks[id_task]);
								ifstream fin_con(path_con);
								if (fin_con.is_open())
								{
									Solution_stud sol_con;
									scan_info_solution(path_con, sol_con);
									sol_con.Name_compiler = Name_number_contest[id_contest];
									sol_con.Name_problem = Name_tasks[id_task];
									sol_con.Time = Time_solution(sol_con.Date);
									sol_con.verdict = "Coincidence";
									sol_con.dist_levenstein = 10000;
									sol_con.Solution = change_str(sol_con.Solution);

									int dist = Algo_levenstein(sol.Solution, sol_con.Solution);
									if (sol.dist_levenstein > dist)
									{
										sol.dist_levenstein = dist;
										sol.Name_dist_levenstein_package = sol_con.Name_package;
										sol.Name_stud_dist_levenstein = Name_student[id_stud_con];
									}
								}
								fin_con.close();
							}
						}
					}
					fin_coath.close();
					sol_stud.push_back(sol);
					cout << path << "\n";
				}
				fin.close();
			}
		}
		check_time_solution(sol_stud);
		write_info_stud(Name_student[id_stud], sol_stud);
		Result_all_stud(Name_student[id_stud], sol_stud, All_stud);
	}
	write_info_all_stud(All_stud);
}
