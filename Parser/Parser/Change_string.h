#pragma once
#include "Header.h"

string change_str(string s, int& comments)
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
				if (s[i] == '\\') ++i;              // экранирующая кавычка
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
