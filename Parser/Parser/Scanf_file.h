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
	ifstream in(filename, ios_base::binary);
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

// Возвращает названия папок
void scanf_folder_names(const string& path, vector<string>& folders) {

	try {
		for (const auto& entry : fs::directory_iterator(path)) {
			if (entry.is_directory()) {
				folders.push_back(entry.path().filename().string());
			}
		}
	}
	catch (const fs::filesystem_error& ex) {
		cerr << "Ошибка доступа к папке: " << ex.what() << endl;
	}
}

// Возвращает названия файлов
void scanf_files_names(const string& path, vector<string>& files) {

	try {
		for (const auto& entry : fs::directory_iterator(path)) {
			if (entry.is_regular_file()) {
				files.push_back(entry.path().filename().string());
			}
		}
	}
	catch (const fs::filesystem_error& ex) {
		cerr << "Ошибка доступа к папке: " << ex.what() << endl;
	}
}

// Проверяет названия файлов. С вердиктом OK или Ignor возвращает true
bool isValidOkOrIgnor(string_view str) {
	static const regex r(R"((OK))");
	static const regex r2(R"((Ignored))");
	return regex_search(str.data(), r) || regex_search(str.data(), r2);
}

// Считывает все папки и файлы по заданной структуре
void scanf_all_info(vector<pair<string, vector<pair<string, map<char, vector<string>>>>>>& puth)
{
	vector<string> Name_contests;
	scanf_folder_names(Folder_base_put, Name_contests);

	for (string Name_contest : Name_contests) {
		string Loc_file_student = Folder_base_put + '/' + Name_contest;
		vector<string> Name_students;
		scanf_folder_names(Loc_file_student, Name_students);
		vector<pair<string, map<char, vector<string>>>> Name_student_tasks;

		for (string Name_student : Name_students) {
			string Loc_file_tasks = Loc_file_student + '/' + Name_student;
			vector<string> Name_tasks, OK;
			if (Scan_mode == 1) {
				scanf_folder_names(Loc_file_tasks, OK);
				if (OK.size() > 0 && OK[0] == "OK" || OK.size() > 1 && OK[1] == "OK")
					Loc_file_tasks += "/OK";
				else
					continue;
			}
			scanf_files_names(Loc_file_tasks, Name_tasks);
			map<char, vector<string>> Name_tasks_map;

			for (string Name_task : Name_tasks) {
				if (Scan_mode != 0 || !Flag_ValidOkOrIgnor || isValidOkOrIgnor(Name_task))
					Name_tasks_map[Name_task[0]].push_back(Name_task);
			}
			Name_student_tasks.push_back({ Name_student ,Name_tasks_map });
		}
		puth.push_back({ Name_contest, Name_student_tasks });
	}
}

// Заполнение информации из посылки
void scan_info_solution(Solution_stud& sol, vector<pair<string, vector<pair<string, map<char, vector<string>>>>>>& puths, 
	int id_contest, int id_stud, string task)
{
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
}

// Кодировка: одна unicode-точка -> байт CP1251
static char to_cp1251(unsigned u)
{
	if (u < 0x80) return (char)u;
	if (u >= 0x0410 && u <= 0x044F) return (char)(u - 0x0410 + 0xC0);	// А-Я, а-я
	switch (u)
	{
	case 0x0401: return (char)0xA8;	// Ё
	case 0x0451: return (char)0xB8;	// ё
	case 0x2116: return (char)0xB9;	// №
	case 0x00AB: return (char)0xAB;	// «
	case 0x00BB: return (char)0xBB;	// »
	case 0x2014: return (char)0x97;	// —
	case 0x2013: return (char)0x96;	// –
	case 0x2026: return (char)0x85;	// …
	case 0x00A0: return (char)0xA0;	// неразрывный пробел
	default:     return '?';
	}
}

// Читает UTF-8 файл целиком и переводит в CP1251
inline std::string read_file(const std::string& path)
{
	std::ifstream in(path, std::ios::binary);
	if (!in.is_open()) return "";

	in.seekg(0, std::ios::end);
	std::string data((size_t)in.tellg(), '\0');
	in.seekg(0, std::ios::beg);
	in.read(&data[0], (std::streamsize)data.size());

	size_t i = (data.compare(0, 3, "\xEF\xBB\xBF") == 0) ? 3 : 0;	// пропускаем BOM
	size_t n = data.size();
	std::string out;
	out.reserve(n);

	while (i < n)
	{
		unsigned char c = data[i];
		unsigned u; int len;
		if (c < 0x80) { u = c;        len = 1; }
		else if ((c & 0xE0) == 0xC0) { u = c & 0x1F; len = 2; }
		else if ((c & 0xF0) == 0xE0) { u = c & 0x0F; len = 3; }
		else if ((c & 0xF8) == 0xF0) { u = c & 0x07; len = 4; }
		else { ++i; continue; }		// битый байт
		for (int k = 1; k < len && i + k < n; k++)
			u = (u << 6) | ((unsigned char)data[i + k] & 0x3F);
		i += len;
		out += to_cp1251(u);
	}

	if (!out.empty() && out.back() != '\n')
		out += '\n';
	return out;
}