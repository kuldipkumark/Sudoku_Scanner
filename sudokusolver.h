#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <iostream>
using namespace std;

class sudokusolver
{
private:
	pair<int,int> findempty(int board[9][9]);
	int row(int board[9][9], int row, int num);
	int cols(int board[9][9], int col, int num);
	int box(int board[9][9], int row, int col, int num);
	int check(int board[9][9], pair<int,int> p, int num);

public:
	int cansolve(int board[9][9]);
	void showBoard(int board[9][9]);

};

#endif