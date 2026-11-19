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
#include <cctype>

using namespace std;
namespace fs = filesystem;

string Folder_base_put = "..\\..\\..\\submission\\submissions1";          // Корневая папка где лежат данные с соревнований
string File_contest = "contest.txt";						    // Файл с названиями соревнований
string File_tasks = "tasks.txt";							    // Файл с названиями заданий
string File_compiler = "compiler.txt";						    // Файл с названиями компиляторов
string File_student = "student.txt";						    // Файл со именами студентов
string Folder_base_stud = "stud_work\\";					    //
string Folder_result = "..\\..\\..\\Списки списавших\\submissions1"; // Корневая папка результатов

string Nick_name_coach = "Kirill-_-";
const int DIST_LEVENSTEIN = 5;
const int NUM_COMMENTS = 0;
bool Flag_ValidOkOrIgnor = false;			  // Cчитывать только Ok или Ignor

struct Solution_stud{
	string Name_contest = ""; 				  // Название контеста
	string Name_package = "";				  // Название файла
	string Name_compiler = "";				  // Название языка на котором написан код
	string Name_stud = "";					  // Имя студента
	string Solution = "";					  // Код
	string Date = "";						  // Дата
	int Time = 0;							  // Время посылки
	string verdict = "";					  // Вердикт
	int dist_levenstein = 0;				  // Расстояние Левенштейна
	string Name_stud_dist_levenstein = "";	  // Имя студента с наименьшим расстоянием Левенштейна
	string Name_dist_levenstein_package = ""; // Название задачи с наименьшим расстоянием Левенштейна
	int Id_stud_dist_levenstein = -1; 		  // Порядковый номер студента с похожим решением
	int Number_comments = 0;				  // Количество комментариев
};