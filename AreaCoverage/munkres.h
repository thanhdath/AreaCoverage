#include <stdio.h>
#include <vector>

using namespace std;

class Munkres {
private:
	int nrow;
	int ncol;
	int max_value;
	vector<vector<int>> costs;
	vector<int> RowCover;
	vector<int> ColCover;
	vector<vector<int>> Mask;
	vector<vector<int>> path;
	int step = 1;

	int path_row_0;
	int path_col_0;
	int path_count;

	void ShowCostMatrix() {
		printf("\nCosts");
		for (int i = 0; i < nrow; i++) {
			printf("\n");
			for (int j = 0; j < ncol; j++) {
				printf("\t%d", costs[i][j]);
			}
		}
	}
	void ShowMaskMatrix() {
		printf("\nMask");
		for (int i = 0; i < nrow; i++) {
			printf("\n");
			for (int j = 0; j < ncol; j++) {
				printf("\t%d", Mask[i][j]);
			}
		}
	}
	void ShowPathMatrix() {
		printf("\nRowCover");
		for (int j = 0; j < ncol; j++) {
			printf("\t%d", RowCover[j]);
		}
		printf("\nColCover");
		for (int j = 0; j < ncol; j++) {
			printf("\t%d", ColCover[j]);
		}
		printf("\nPath");
		for (int i = 0; i < nrow*2 + 1; i++) {
			printf("\n");
			for (int j = 0; j < 2; j++) {
				printf("\t%d", path[i][j]);
			}
		}
	}

	void step_one() {
		// for each row of the cost matrix, find the smallest element and
		// substract it from every element in its row.
		int min_in_row;
		for (int r = 0; r < nrow; r++) {
			min_in_row = costs[r][0];
			for (int c = 1; c < ncol; c++) {
				if (costs[r][c] < min_in_row)
					min_in_row = costs[r][c];
			}
			for (int c = 0; c < ncol; c++) {
				costs[r][c] -= min_in_row;
			}
		}
		step = 2;
	}
	void step_two() {
		/*find a zero(Z) in the resulting matrix. If there is no starred zero
			in its row or column, star Z. Repeat for each element in the 
			matrix. Go to step 3*/
		for(int r=0; r<nrow; r++)
			for (int c = 0; c < ncol; c++) {
				if (costs[r][c] == 0 && RowCover[r] == 0 && ColCover[c] == 0) {
					Mask[r][c] = 1;
					RowCover[r] = 1;
					ColCover[c] = 1;
				}
			}
		for (int r = 0; r < nrow; r++) {
			RowCover[r] = 0;
		}
		for (int c = 0; c < ncol; c++) {
			ColCover[c] = 0;
		}
		step = 3;
	}
	void step_three() {
		/*Cover each column containing a starred zero. If K columns are covered,
			the starred zeros describe a complete set of unique assignments. In
			this case. Go to Done, otherwise, Go to step 4*/
		int colcount;
		for (int r = 0; r < nrow; r++)
			for (int c = 0; c < ncol; c++)
				if (Mask[r][c] == 1)
					ColCover[c] = 1;
		colcount = 0;
		for (int c = 0; c < ncol; c++)
			if (ColCover[c] == 1)
				colcount += 1;
		if (colcount >= ncol || colcount >= nrow)
			step = 7;
		else
			step = 4;
	}
	void step_four() {
		/*find a noncovered zero and prime it. If there is no starred zero
		in the row containing this primed zero, go to step 5. Otherwise, 
		cover this row and uncover the column containing the starred zero.
		Continue in this manner until there are no uncovered zeros left.
		Save the smallest uncovered value and go to step 6*/
		int row = -1;
		int col = -1;
		bool done;
		done = false;
		while (!done) {
			find_a_zero(&row, &col);
			if (row == -1) {
				done = true;
				step = 6;
			}
			else {
				Mask[row][col] = 2;
				if (star_in_row(row)) {
					find_star_in_row(row, &col);
					RowCover[row] = 1;
					ColCover[col] = 0;
				}
				else {
					done = true;
					step = 5;
					path_row_0 = row;
					path_col_0 = col;
				}
			}
		}
	}
	void find_a_zero(int *row, int *col) {
		int r = 0;
		int c;
		bool done;
		(*row) = -1;
		(*col) = -1;
		done = false;
		while (!done) {
			c = 0;
			while (1) {
				if (costs[r][c] == 0 && RowCover[r] == 0 && ColCover[c] == 0) {
					(*row) = r;
					*col = c;
					done = true;
				}
				c += 1;
				if (c >= ncol || done)
					break;
			}
			r += 1;
			if (r >= nrow)
				done = true;
		}
	}
	bool star_in_row(int row) {
		bool tmp = false;
		for (int c = 0; c < ncol; c++)
			if (Mask[row][c] == 1)
				tmp = true;
		return tmp;
	}
	void find_star_in_row(int row, int* col) {
		*col = -1;
		for (int c = 0; c < ncol; c++)
			if (Mask[row][c] == 1)
				*col = c;
	}
	void step_five() {
		/*construct a series of alternating primed and starred zeros as follows.
		let Z0 represent the uncovered primed zero found in step 4. Let Z1 denote
		the starred zero in the column of Z0 (if any). Let Z2 denote the primed 
		zero in the row of Z1 (there will always be one). Continue until the series
		terminates at a primed zero that has no starred zero in its column.
		Unstar each starred zero of the serires, star each primed zero of the series,
		erase all primes and uncover every line in the matrix. Return to step 3*/
		bool done;
		int r = -1;
		int c = -1;

		path_count = 1;
		path[path_count - 1][0] = path_row_0;
		path[path_count - 1][1] = path_col_0;
		done = false;
		while (!done) {
			find_star_in_col(path[path_count - 1][1], &r);
			if (r > -1) {
				path_count += 1;
				path[path_count - 1][0] = r;
				path[path_count - 1][1] = path[path_count - 2][1];
			}
			else {
				done = true;
			}
			if (!done) {
				find_prime_in_row(path[path_count - 1][0], &c);
				path_count += 1;
				path[path_count - 1][0] = path[path_count - 2][0];
				path[path_count - 1][1] = c;
			}
		}
		augment_path();
		clear_covers();
		erase_primes();
		step = 3;
	}
	void find_star_in_col(int c, int *r) {
		*r = -1;
		for (int i = 0; i < nrow; i++)
			if (Mask[i][c] == 1)
				*r = i;
	}
	void find_prime_in_row(int r, int *c) {
		for (int j = 0; j < ncol; j++)
			if (Mask[r][j] == 2)
				*c = j;
	}
	void augment_path() {
		for (int p = 0; p < path_count; p++)
			if (Mask[path[p][0]][path[p][1]] == 1)
				Mask[path[p][0]][path[p][1]] = 0;
			else
				Mask[path[p][0]][path[p][1]] = 1;
	}
	void clear_covers() {
		for (int r = 0; r < nrow; r++)
			RowCover[r] = 0;
		for (int c = 0; c < ncol; c++)
			ColCover[c] = 0;
	}
	void erase_primes() {
		for (int r = 0; r < nrow; r++)
			for (int c = 0; c < ncol; c++)
				if (Mask[r][c] == 2)
					Mask[r][c] = 0;
	}
	void step_six() {
		/*Add the value found in step 4 to every element of each covered row, and
		substract if from every element of each uncovered column. Return to step 4
		without altering any stars, primes, or covered lines.*/
		int minval = max_value;
		find_smallest(&minval);
		for(int r=0; r<nrow; r++)
			for (int c = 0; c < ncol; c++) {
				if (RowCover[r] == 1)
					costs[r][c] += minval;
				if (ColCover[c] == 0)
					costs[r][c] -= minval;
			}
		step = 4;
	}
	void find_smallest(int *minval) {
		for (int r = 0; r < nrow; r++)
			for (int c = 0; c < ncol; c++)
				if (RowCover[r] == 0 && ColCover[c] == 0)
					if (*minval > costs[r][c])
						*minval = costs[r][c];
	}
	void step_seven() {}

public:
	Munkres(vector<vector<int>> *costs, int size, int max_value) {
		for (int r = 0; r < size; r++) {
			vector<int> temp;
			this->costs.push_back(temp);
			for (int c = 0; c < size; c++) {
				this->costs[r].push_back((*costs)[r][c]);
			}
		}
		this->nrow = size;
		this->ncol = size;
		this->max_value = max_value;

		for (int i = 0; i < size; i++) {
			RowCover.push_back(0);
			ColCover.push_back(0);
			vector<int> temp, tmp;
			Mask.push_back(temp);
			for (int j = 0; j < size; j++) {
				Mask[i].push_back(0);
			}
		}
		for (int i = 0; i < 2 * size + 1; i++) {
			vector<int> temp = { 0, 0 };
			path.push_back(temp);
		}
	}
	void Run() {
		bool done = false;
		while (!done) {
			/*ShowCostMatrix();
			ShowMaskMatrix();
			ShowPathMatrix(); */
			switch (step)
			{
			case 1:
				step_one();
				//printf("\n1.");
				break;
			case 2:
				step_two();
				//printf("\n2.");
				break;
			case 3:
				step_three();
				//printf("\n3.");
				break;
			case 4:
				step_four();
				//printf("\n4.");
				break;
			case 5:
				step_five();
				//printf("\n5.");
				break;
			case 6:
				step_six();
				//printf("\n6.");
				break;
			case 7:
				step_seven();
				done = true;
				break;
			default:
				break;
			}
		}

		//printf("\nEnd");
		//ShowCostMatrix();
		//ShowMaskMatrix();
		//ShowPathMatrix();
	}

	vector<vector<int>>* getMask() {
		return &Mask;
	}
};