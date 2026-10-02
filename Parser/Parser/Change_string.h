#pragma once
#include "Header.h"

vector<int> Mark_char(10000);

void Clear_Mark_char()
{
	for (int i = 0; i < Mark_char.size(); ++i)
		Mark_char[i] = 0;
}

string change_str(string s, int& comments)
{
	const char* forbidden = "(){}[]<>;";
	int k = 0;
	Clear_Mark_char();
	if (s.size() > 10000)
		return s;
	for (int i = 0; i < s.size(); ++i)
	{
		if (s[i] == '\n' || s[i] == ' ' || s[i] == '\t' || s[i] == '\r')
			Mark_char[i] = 1;
		else if (s[i] == '#')
		{
			while (s[i] != '\n' && i < s.size())
			{
				Mark_char[i] = 1;
				++i;
			}
			--i;
		}
		else if (s[i] == '/')
		{
			if (s[i + 1] == '/')
			{
				bool f = 1;
				Mark_char[i] = 2;
				++i;
				while (s[i] != '\n' && i < s.size())
				{
					Mark_char[i] = 2;
					++i;
					k++;
					if (f && strchr(forbidden, s[i]) != nullptr && i < s.size())
						f = 0;
				}
				if (f)
					comments += k - 1;
				k = 0;
				--i;
			}
			else if (s[i + 1] == '*')
			{
				bool f = 1;
				Mark_char[i] = 2;
				++i;
				while ((s[i] != '*' || s[i + 1] != '/') && i < s.size())
				{
					Mark_char[i] = 2;
					++i;
					if (s[i] != '\n')
						k++;
					if (f && strchr(forbidden, s[i]) != nullptr && i < s.size())
						f = 0;
				}
				if (s[i] == '*' && s[i + 1] == '/') {
					Mark_char[i] = 2;
					Mark_char[i + 1] = 2;
					i += 2;
				}
				if (f)
					comments += k - 1;
				k = 0;
				--i;
			}
		}
	}
	string result_str = "";
	for (int i = 0; i < s.size(); ++i)
	{
		if (Mark_char[i] == 0)
			result_str += s[i];
		//if (Mark_char[i] == 2)
		//	comments++;
	}
	return result_str;
}
