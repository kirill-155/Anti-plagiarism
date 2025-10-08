#pragma once
#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

string Folder_base_put = ""; // базовая папка где лежит парсер сайта
string File_contest = "contest.txt";
string File_tasks = "tasks.txt";
string File_compiler = "compiler.txt";
string File_student = "student.txt";
string Folder_base_stud = "stud_work/";

string Nick_name_coach = "Kirill-_-";

struct Solution_stud{
	string Name_contest = "";
	string Name_problem = "";
	string Name_package = "";
	string Name_compiler = "";
	string Name_stud = "";
	string Solution = "";
	string Date = "";
	int Time = 0;
	string verdict = "";
	int dist_levenstein = 0;
	string Name_stud_dist_levenstein = "";
	string Name_dist_levenstein_package = "";
};