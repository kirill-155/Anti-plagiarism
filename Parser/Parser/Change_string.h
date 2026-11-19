#pragma once
#include "Header.h"

// Обработка C++ / C#
inline string change_str_cpp(string& s, int& comments)
{
	int k = 0, n = s.size();
	if (n > 20000)
		return s;
	vector<int> Mark_char(n);
	const char* forbidden = "{}[]<>;";
	for (int i = 0; i < n; ++i)
	{
		if (s[i] == '\n' || s[i] == ' ' || s[i] == '\t' || s[i] == '\r')
			Mark_char[i] = 1;
		else if (s[i] == '#')						 // директивы
		{
			while (i < n && s[i] != '\n')
			{
				Mark_char[i] = 1;
				++i;
			}
			--i;
		}
		else if (s[i] == '"' || s[i] == '\'')       // пропускаем строки целиком
		{
			char q = s[i];
			++i;
			while (i < n && s[i] != q)
			{
				if (s[i] == '\\') ++i;              // экранирующий символ
				++i;
			}
		}
		else if (s[i] == '/')
		{
			if (s[i + 1] == '/')					// однострочные комментарии
			{
				bool f = 1;
				Mark_char[i] = 2;
				Mark_char[++i] = 2;
				++i;
				while (i < n && s[i] != '\n')
				{
					Mark_char[i] = 2;
					k++;
					if (f && i < n && strchr(forbidden, s[i]) != nullptr)
						f = 0;
					++i;
				}
				if (f)
					comments += k;
				k = 0;
				--i;
			}
			else if (s[i + 1] == '*')			   // многострочные комментарии
			{
				bool f = 1;
				Mark_char[i] = 2;
				Mark_char[++i] = 2;
				++i;
				while (i < n && (s[i] != '*' || s[i + 1] != '/'))
				{
					Mark_char[i] = 2;
					if (s[i] != '\n')
						k++;
					if (f && i < n && strchr(forbidden, s[i]) != nullptr)
						f = 0;
					++i;
				}
				if (s[i] == '*' && s[i + 1] == '/') {
					Mark_char[i] = 2;
					Mark_char[i + 1] = 2;
					i += 2;
				}
				if (f)
					comments += k;
				k = 0;
				--i;
			}
		}
	}
	string result_str = "";
	for (int i = 0; i < n; ++i)
	{
		if (Mark_char[i] == 0)
			result_str += s[i];
	}
	return result_str;
}

// Обработка Python
inline string change_str_python(string& s, int& comments)
{
	vector<int> Mark_char(10000);
	const char* forbidden = "{}[]<>;";
	int k = 0, n = s.size();
	if (n > 10000)
		return s;
	for (int i = 0; i < n; ++i)
	{
		if (s[i] == '\n' || s[i] == ' ' || s[i] == '\t' || s[i] == '\r')
			Mark_char[i] = 1;
		else if (s[i] == '#')						// однострочные комментарии
		{
			bool f = 1;
			Mark_char[i] = 2;
			++i;
			while (i < n && s[i] != '\n')
			{
				Mark_char[i] = 2;
				k++;
				if (f && strchr(forbidden, s[i]) != nullptr)
					f = 0;
				++i;
			}
			if (f)
				comments += k;
			k = 0;
			--i;
		}
		else if (s[i] == '"' || s[i] == '\'')		// пропускаем строки
		{
			char q = s[i];
			if (i + 2 < n && s[i + 1] == q && s[i + 2] == q)	// тройные кавычки ''' или """
			{
				i += 3;
				while (i < n && !(s[i] == q && i + 2 < n && s[i + 1] == q && s[i + 2] == q))
				{
					if (s[i] == '\n' || s[i] == '\r')
						Mark_char[i] = 1;			// переносы строк внутри строки тоже убираем
					if (s[i] == '\\') ++i;			// экранирующий символ
					++i;
				}
				if (i < n)
					i += 2;							
			}
			else
			{
				++i;
				while (i < n && s[i] != q)
				{
					if (s[i] == '\\') ++i;			// экранирующая кавычка
					++i;
				}
			}
		}
	}
	string result_str = "";
	for (int i = 0; i < n; ++i)
	{
		if (Mark_char[i] == 0)
			result_str += s[i];
	}
	return result_str;
}

// Выбор обработчика по языку
inline string change_str(string& s, int& comments, const string& language)
{
	if (language == "Python")
		return change_str_python(s, comments);
	else if (language == "C++" || language == "C#")
		return change_str_cpp(s, comments);	
	return s;
}