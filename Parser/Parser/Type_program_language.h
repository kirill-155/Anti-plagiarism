#pragma once
#include "Header.h"
#include "Standardizer_cpp.h"
#include "Standardizer_csh.h"
#include "Standardizer_python.h"

// Проверяет язык написания кода. Возвращает название языка программирования
string identif_compil(const string& code) {
	int cpp = 0, cs = 0, py = 0;
	const char* d = code.data();
	int n = code.size();

	auto match = [&](int i, const char* p, int len) {
		return i + len <= n && memcmp(d + i, p, len) == 0;
		};

	int limit = min(n, 500);
	for (int i = 0; i < limit && cpp < 4 && cs < 4 && py < 4; ++i) {
		switch (d[i]) {
		case '#':
			if (match(i, "#include", 8))              cpp += 5;
			break;
		case 's':
			if (match(i, "std::", 5))                 cpp += 4;
			else if (match(i, "self.", 5))            py += 2;
			break;
		case 'u':
			if (match(i, "using System", 12))         cs += 5;
			else if (match(i, "using namespace std", 19)) cpp += 3;
			break;
		case 'c':
			if (match(i, "cout", 4))                  cpp += 3;
			else if (match(i, "cin", 3))              cpp += 2;
			else if (match(i, "Console.", 8))         cs += 4;
			else if (match(i, "class ", 6))           cs += 1;
			break;
		case 'd':
			if (match(i, "def ", 4))                  py += 3;
			break;
		case 'e':
			if (match(i, "elif ", 5))                 py += 4;
			break;
		case 'p':
			if (match(i, "print(", 6))                py += 2;
			break;
		case 'n':
			if (match(i, "nullptr", 7))               cpp += 2;
			else if (match(i, "namespace ", 10))      cs += 2;
			break;
		case 't':
			if (match(i, "template<", 9) ||
				match(i, "template <", 10))           cpp += 3;
			break;
		case '_':
			if (match(i, "__init__", 8))              py += 4;
			break;
		case 'T':
			if (match(i, "True", 4))                  py += 1;
			break;
		case 'F':
			if (match(i, "False", 5))                 py += 1;
			break;
		case 'N':
			if (match(i, "None", 4))                  py += 1;
			break;
		case 'M':
			if (match(i, "Main(", 5))                 cs += 2;
			break;
		}
	}

	// Победитель должен и набрать минимум, и строго опережать остальных
	if (cpp >= 3 && cpp > cs && cpp > py) return "C++";
	if (cs >= 3 && cs > cpp && cs > py) return "C#";
	if (py >= 3 && py > cpp && py > cs) return "Python";
	return "Unknown";
}

string standardize(string& Code, const string& compiler) {
	if (compiler == "C++") {
		CppStandardizer stdz_cpp;
		return stdz_cpp.standardize(Code);
	}
	else if (compiler == "C#") {
		CSharpStandardizer stdz_csh;
		return stdz_csh.standardize(Code);
	}
	else if (compiler == "Python") {
		PythonStandardizer stdz_py;
		return stdz_py.standardize(Code);
	}
	return Code;
}

int type_leng(string type) {
	if (type == "C++") {
		return 0;
	}
	else if (type == "C#") {
		return 1;
	}
	else if (type == "Python") {
		return 2;
	}
	else return 0;
}