#pragma once
#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <map>
#include <filesystem>
#include <regex>
#include <set>

using namespace std;
namespace fs = filesystem;

string Folder_base_put = "..\\..\\..\\Соревнование\\"; // базовая папка где лежит все
string File_contest = "contest.txt";
string File_tasks = "tasks.txt";
string File_compiler = "compiler.txt";
string File_student = "student.txt";
string Folder_base_stud = "stud_work\\";
string Folder_result = "..\\..\\..\\Списки списавших\\Муницип";

string Nick_name_coach = "Kirill-_-";
const int DIST_LEVENSTEIN = 5;
const int NUM_COMMENTS = 0;

struct Solution_stud{
	string Name_contest = ""; 				  // Название контеста
	string Name_problem = "";				  // Название задачи
	string Name_package = "";				  // 
	string Name_compiler = "";				  // Компилятор
	string Name_stud = "";					  // Имя студента
	string Solution = "";					  // Код
	string Date = "";						  // Дата
	int Time = 0;							  // Время посылки
	string verdict = "";					  // Вердикт
	int dist_levenstein = 0;				  // Расстояние Левенштейна
	string Name_stud_dist_levenstein = "";	  // Имя студента наименьшего расстояния Левенштейна
	string Name_dist_levenstein_package = ""; // Название задачи наименьшего расстояния Левенштейна
	int Name_dist_levenstein_num = -1; 		  // Порядковый номер студента с похожим решением
	int Number_comments = 0;				  // Количество комментариев
};