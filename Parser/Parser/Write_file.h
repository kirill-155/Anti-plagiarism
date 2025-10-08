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
