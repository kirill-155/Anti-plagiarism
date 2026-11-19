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

int Algo_levenstein(string str1, string str2, int limit)
{
	int n = str1.size(), m = str2.size();
	if (n >= Matrix_levenstein.size() || m >= Matrix_levenstein[0].size())
		return 1000;
	int max_len = abs(m - n);
	if (max_len > DIST_LEVENSTEIN || max_len > limit)
		return 1000;
	Claer_matrix(n, m);
	for (int i = 0; i < n; ++i)
		Matrix_levenstein[i][0] = i;
	for (int j = 0; j < m; ++j)
		Matrix_levenstein[0][j] = j;

	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= m; ++j)
		{
			if (str1[i - 1] == str2[j - 1])
			{
				Matrix_levenstein[i][j] = Matrix_levenstein[i - 1][j - 1];
			}
			else
			{
				Matrix_levenstein[i][j] = min(Matrix_levenstein[i - 1][j], 
					min(Matrix_levenstein[i - 1][j - 1], Matrix_levenstein[i][j - 1]));
				Matrix_levenstein[i][j]++;
			}
		}
	}
	return Matrix_levenstein[n][m];
}