#include "sudokusolver.h"
#include <bits/stdc++.h>
using namespace std;

// to find the empty space in sudoku board to fill in
pair<int,int>  sudokusolver::findempty(int board[9][9]){

	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			if (board[i][j] == 0)
				return make_pair(i,j);
	
	return make_pair(-1,-1); // every space is filled

}
// check the row element 
int sudokusolver::row(int board[9][9], int row, int num){

	for (int i = 0 ; i < 9; ++i)
		if (board[row][i] == num)
			return 0;

	return 1;	
}
// check the columun element
int sudokusolver::cols(int board[9][9], int col, int num){

	for (int i = 0; i < 9; ++i)
		if (board[i][col] == num)
			return 0; 

	return 1;	
}
// check element in 3X3 small box  in the board
int sudokusolver::box(int board[9][9], int row, int col, int num){

	int boxrow = (row/3)*3, boxcol = (col/3)*3;
	for (int i = boxrow; i < boxrow+3; ++i)
		for (int j = boxcol; j < boxcol+3; ++j)
			if (board[i][j] == num)
				return 0;

	return 1;		
}
// check whether number is valid to fill in the place
int sudokusolver::check(int board[9][9], pair<int,int> p, int num){

	if (!row(board,p.first, num))
		return 0;
	if (!cols(board,p.second, num))
		return 0;
	if(!box(board,p.first,p.second, num))
		return 0;

	return 1;
}
// Can we solve the sudoku
int sudokusolver::cansolve(int board[9][9]){

	pair<int,int> p = findempty(board);
	if (p.first == -1)
		return 1; // solved

	for (int i = 1; i < 10; ++i){

		if (check(board,p,i)){
			
			board[p.first][p.second] = i;
			if (cansolve(board))
				return 1;
			board[p.first][p.second] = 0;	
		}
	}
	return 0;

}
void sudokusolver::showBoard(int board[9][9]){

	cout << "\n";
	for (int i = 0; i < 9; ++i){
		for (int j = 0; j < 9; ++j)
			cout << board[i][j] << " ";
		cout << "\n";
	}

}