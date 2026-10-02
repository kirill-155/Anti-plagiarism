#pragma once
#include"Header.h"

void write_info_stud(string Name_student, vector<Solution_stud>& Stud)
{
	string file_info = Folder_base_put + Folder_base_stud;
	file_info += Name_student + '/';
	file_info += Name_student + ".txt";
	ofstream out(file_info);
	for (int i = 0; i < Stud.size(); i++) 
	{
		out << Stud[i].Name_contest << " ";
		out << Stud[i].Name_problem << " ";
		out << Stud[i].Name_package << " : ";
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

// Выывод пар решений с наименьшим расстоянием Левенштейна
void write_info_cheat_stud_levenstein(vector<pair<string, vector<pair<string, map<char, vector<string>>>>>>& puths, vector<Solution_stud>& sol_stud, int id_contest)
{
	set<string> st;
	ofstream out(Folder_result + "\\" + puths[id_contest].first + " " + to_string(DIST_LEVENSTEIN) + ".txt");
	for (int i = 0; i < sol_stud.size(); i++)
	{
		if (sol_stud[i].dist_levenstein <= DIST_LEVENSTEIN && sol_stud[i].Name_dist_levenstein_num != -1
			&& !st.count(sol_stud[i].Name_stud + sol_stud[i].Name_stud_dist_levenstein + sol_stud[i].Name_problem[0]))
		{
			st.insert(sol_stud[i].Name_stud_dist_levenstein + sol_stud[i].Name_stud + sol_stud[i].Name_problem[0]);
			out << sol_stud[i].Name_stud << "\\" << sol_stud[i].Name_problem << "\tCoincidence\t" << sol_stud[i].Name_stud_dist_levenstein << "\\"
				<< sol_stud[i].Name_dist_levenstein_package << "\n";
		}
	}
	out.close();
}

// Вывод количества комментариев для решений
void write_info_cheat_stud_comments(vector<pair<string, vector<pair<string, map<char, vector<string>>>>>>& puths, vector<Solution_stud>& sol_stud, int id_contest)
{
	ofstream out_comment(Folder_result + "\\" + puths[id_contest].first + " Comment " + to_string(NUM_COMMENTS) + ".txt");
	for (int i = 0; i < sol_stud.size(); i++)
	{
		if (sol_stud[i].Number_comments > NUM_COMMENTS)
		{
			out_comment << sol_stud[i].Name_stud << "\\" << sol_stud[i].Name_problem << "\tGreat comment\t" << sol_stud[i].Number_comments << "\n";
		}
	}
	out_comment.close();
}

// Вывод результатов.
void write_info_cheat_stud(vector<pair<string, vector<pair<string, map<char, vector<string>>>>>>& puths, vector<Solution_stud>& sol_stud, int id_contest)
{
	write_info_cheat_stud_levenstein(puths, sol_stud, id_contest);

	write_info_cheat_stud_comments(puths, sol_stud, id_contest);
}

// Проверяет названия файлов. С вердиктом OK или Ignor возвращает true
bool isValidOkOrIgnor1(string_view str) {
	static const regex r(R"((OK))");
	static const regex r2(R"((Ignored))");
	return regex_search(str.data(), r) || regex_search(str.data(), r2);
}

// Проверяет названия файлов. С компилятором от с++ возвращает true
bool is_valid_cpp(string_view str) {
	static const regex r(R"((clang14_cpp20)|(gcc14_cpp23))");
	return regex_search(str.data(), r);
}

// Проверяет названия файлов. С компилятором от с# возвращает true
bool is_valid_csh(string_view str) {
	static const regex r(R"((dotnet8))");
	return regex_search(str.data(), r);
}

void write_info_cheat_stud_comment(vector<pair<string, vector<pair<string, map<char, vector<string>>>>>>& puths, vector<Solution_stud>& sol_stud, int& id_contest) {
	vector<int> f = { 0,5,10,20,30,40,50,100 }; // Количество комментариев
	for (int p : f) 
	{
		ofstream out_comment(Folder_result + "\\Test\\" + puths[id_contest].first + " Comment " + to_string(p) + ".txt");
		for (int i = 0; i < sol_stud.size(); i++)
			if (sol_stud[i].Number_comments > p && is_valid_cpp(sol_stud[i].Name_problem))
			{
				out_comment << sol_stud[i].Name_stud << "\\" << sol_stud[i].Name_problem << "\tGreat comment\t" << sol_stud[i].Number_comments << "\n";
			}
		out_comment.close();
	}
}

void write_info_cheat_stud1(vector<pair<string, vector<pair<string, map<char, vector<string>>>>>>& puths, vector<Solution_stud>& sol_stud, int id_contest)
{
	write_info_cheat_stud_comment(puths, sol_stud, id_contest);
	//vector<int> f = { 5,10,20,30,40,50 };
	//string pov = "С повторами\\";
	//for (int p : f)
	//{
	//	//set<string> st;
	//	ofstream out(Folder_result + "\\" + pov + puths[id_contest].first + " " + to_string(p) + ".txt");
	//	for (int i = 0; i < sol_stud.size(); i++)
	//		if (sol_stud[i].dist_levenstein <= p && sol_stud[i].Name_dist_levenstein_num != -1)
	//			//&& !st.count(sol_stud[i].Name_stud + sol_stud[i].Name_stud_dist_levenstein + sol_stud[i].Name_problem[0]))
	//		{
	//			//st.insert(sol_stud[i].Name_stud_dist_levenstein + sol_stud[i].Name_stud + sol_stud[i].Name_problem[0]);
	//			out << sol_stud[i].Name_stud << "\\" << sol_stud[i].Name_problem << "\tCoincidence\t" << sol_stud[i].Name_stud_dist_levenstein << "\\"
	//				<< sol_stud[i].Name_dist_levenstein_package << "\n";
	//		}
	//	
	//	out.close();
	//}

	//for (int p : f)
	//{
	//	//set<string> st;
	//	ofstream out(Folder_result + "\\" + pov + "Ok\\" + puths[id_contest].first + " " + to_string(p) + ".txt");
	//	//ofstream out_comment(Folder_result + "\\" + puths[id_contest].first + " Comment " + to_string(NUM_COMMENTS) + ".txt");
	//	for (int i = 0; i < sol_stud.size(); i++)
	//	{
	//		if (!isValidOkOrIgnor(sol_stud[i].Name_problem))
	//			continue;
	//		if (sol_stud[i].dist_levenstein <= p && sol_stud[i].Name_dist_levenstein_num != -1)
	//			//&& !st.count(sol_stud[i].Name_stud + sol_stud[i].Name_stud_dist_levenstein + sol_stud[i].Name_problem[0]))
	//		{
	//			//st.insert(sol_stud[i].Name_stud_dist_levenstein + sol_stud[i].Name_stud + sol_stud[i].Name_problem[0]);
	//			out << sol_stud[i].Name_stud << "\\" << sol_stud[i].Name_problem << "\tCoincidence\t" << sol_stud[i].Name_stud_dist_levenstein << "\\"
	//				<< sol_stud[i].Name_dist_levenstein_package << "\n";
	//		}
	//	}
	//	out.close();
	//}
}