#pragma once
#include "Header.h"

vector<vector<int>> Matrix_levenstein(5000, vector<int>(5000));

void Claer_matrix(int mi, int mj)
{
	for (int i = 0; i <= mi; ++i)
	{
		for (int j = 0; j <= mj; ++j)
			Matrix_levenstein[i][j] = 0;
	}
}

int Algo_levenstein(string str1, string str2)
{
	if (str1.size() >= Matrix_levenstein.size() || str2.size() >= Matrix_levenstein[0].size())
		return 1000;
	int max_len = str2.size() - str1.size();
	if (abs(max_len) > 40)
		return 1000;
	Claer_matrix(str1.size(), str2.size());
	for (int i = 0; i < str1.size(); ++i)
		Matrix_levenstein[i][0] = i;
	for (int j = 0; j < str2.size(); ++j)
		Matrix_levenstein[0][j] = j;

	for (int i = 1; i <= str1.size(); ++i)
	{
		for (int j = 1; j <= str2.size(); ++j)
		{
			if (str1[i] == str2[j])
			{
				Matrix_levenstein[i][j] = Matrix_levenstein[i - 1][j - 1];
			}
			else
			{
				Matrix_levenstein[i][j] = min(Matrix_levenstein[i - 1][j], min(Matrix_levenstein[i - 1][j - 1], Matrix_levenstein[i][j - 1]));
				Matrix_levenstein[i][j]++;
			}
		}
	}
	return Matrix_levenstein[str1.size()][str2.size()];
}