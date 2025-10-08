#pragma once
#include"Header.h"

void scanf_file_contest(string fileName, vector<string>& Name_number_contest) 
{
	string number;
	ifstream in(fileName);
	while (getline(in, number))
	{
		Name_number_contest.push_back(number);
	}
	in.close();
}

//tasks
void scanf_file_tasks(string filename, vector<string>& Name_tasks) 
{
	string tasks;
	ifstream in(filename);
	while (getline(in, tasks)) 
	{
		Name_tasks.push_back(tasks);
	}
	in.close();
}

//student
void scanf_file_student(string filename, vector<string>& Name_student) 
{
	string student;
	ifstream in(filename, std::ios_base::binary);
	while (getline(in, student)) 
	{
		string name="";
		for (int i = 0; i < student.size(); i++) 
		{
			if (student[i] != '\r') {
				name += student[i];
			}
		}
		Name_student.push_back(name);
	}
	in.close();
}

//compiler
void scanf_file_compiler(string filename, vector<string>& Name_compiler)
{
	string compiler;
	ifstream in(filename);
	while (getline(in, compiler))
	{
		Name_compiler.push_back(compiler);
	}
	in.close();
}

//all_base_info
void scanf_all_info(vector<string>& Name_number_contest, vector<string>& Name_tasks, vector<string>& Name_student,  vector<string>& Name_compiler)
{
	string Loc_file_name_contest = Folder_base_put + File_contest;
	string Loc_file_tasks = Folder_base_put + File_tasks;
	string Loc_file_student = Folder_base_put + File_student;
	string Loc_file_compiler = Folder_base_put + File_compiler;
	scanf_file_contest(Loc_file_name_contest, Name_number_contest);
	scanf_file_tasks(Loc_file_tasks, Name_tasks);
	scanf_file_student(Loc_file_student, Name_student);
	scanf_file_compiler(Loc_file_compiler, Name_compiler);
}

void scan_info_solution(string fileName, Solution_stud& sol) 
{
	ifstream in(fileName);
	string str;
	getline(in, str);//id
	sol.Name_package = str;
	getline(in, str);//time
	sol.Date = str;
	getline(in, str);
	sol.Name_stud = str;
	getline(in, str);
	sol.Name_compiler = str;
	while (getline(in, str)) {
		sol.Solution += str + '\n';
	}
	in.close();
}