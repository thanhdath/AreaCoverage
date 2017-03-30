#include "hungarian.h"
#include <stdio.h>

Hungarian::Hungarian(vector<vector<int>> *costs, int size, int max_value) {
	this->costs = costs;
	this->size = size;
	this->max_value = max_value;
	printCosts();

	for (int i = 0; i < size; i++) {
		zeroRows.push_back(-1);
		zeroCols.push_back(-1);
	}
	substractRowMinimal();
	substractColMinimal();
	printCosts();
	int i = 0;
	int one = 0;
	while (i < 5) {
		i++;
		int numLines = minimumLinescoverZeros();
		printf("\nnumlines: %d", numLines);
		if (numLines >= size) {
			one = true;
			break;
		}
		addMinimumNumberUncovered();
		printCosts();
	}
	if(one) selectZeros();
}

void Hungarian::substractRowMinimal() {
	for (int row = 0; row < size; row++) {
		int min = (*costs)[row][0];
		for (int col = 1; col < size; col++) {
			if ((*costs)[row][col] < min) 
				min = (*costs)[row][col];
		}
		for (int col = 0; col < size; col++) {
			(*costs)[row][col] -= min;
		}
	}
}
void Hungarian::substractColMinimal() {
	for (int col = 0; col < size; col++) {
		int min = (*costs)[0][col];
		for (int row = 1; row < size; row++) {
			if ((*costs)[row][col] < min) 
				min = (*costs)[row][col];
		}
		for (int row = 0; row < size; row++) {
			(*costs)[row][col] -= min;
		}
	}
}

/*
int Hungarian::minimumLinescoverZeros() {
	http://csclab.murraystate.edu/~bob.pilgrim/445/munkres.html
	// https://en.wikipedia.org/wiki/Hungarian_algorithm#Matrix_interpretation
	map<int, vector<int>> assignedByRows;
	map<int, vector<int>> unassignedByRows;
	map<int, vector<int>> assignedByCols;
	map<int, vector<int>> unassignedByCols;
	//unassignedByRows, assignedByRows = {{rowIndex, {cols has Zero}}, ...}

	// Assign zero
	for (int row = 0; row < size; row++) {
		// if found zero, assign it
		for (int col = 0; col < size; col++) {
			if ((*costs)[row][col] == 0) {
				if (assignedByCols[col].size() == 0) {
					assignedByCols[col].push_back(row);
					assignedByRows[row].push_back(col);
				}
				else {
					unassignedByRows[row].push_back(col);
					unassignedByCols[col].push_back(row);
				}
			}
		}
	}

	vector<int> markedRows;
	vector<int> markedCols;

	// initialize markedRows, markedCols
	for (int i = 0; i < size; i++) {
		markedRows.push_back(0);
		markedCols.push_back(0);
	}

	//mark all rows having no assignments
	for (auto &elm : unassignedByRows) {
		int row = elm.first;
		if (markedRows[row] == 1) continue;
		markedRows[row] = 1;
		// mark all columns having zeros in newly marked row
		for (auto &col : assignedByRows[row]) {
			if (markedCols[col] == 0) {
				markedCols[col] = 1;
				// mark all rows having assignments in newly marked col
				for (auto &i : assignedByCols[col]) {
					markedRows[i] = 1;
					for (auto &k : assignedByRows[i]) {
						markedCols[k] = 1;
					}
				}
			}
		}
		for (auto &col : unassignedByRows[row]) {
			if (markedCols[col] == 0) {
				markedCols[col] = 1;
				for (auto &i : assignedByCols[col]) {
					markedRows[i] = 1;
					for (auto &k : assignedByRows[i]) {
						markedCols[k] = 1;
					}
				}
			}
		}
	}

	for (int row = 0; row < size; row++) {
		vector<int> temp;
		lines.push_back(temp);
		for (int col = 0; col < size; col++) {
			if (markedRows[row] == 0 || markedCols[col] == 1) {
				lines[row].push_back(1);
			}
			else {
				lines[row].push_back(0);
			}
		}
	}

	int numLines = 0;
	for (int i : markedRows) {
		if (i == 0) numLines++;
	}
	for (int i : markedCols) {
		if (i == 1) numLines++;
	}
	return numLines;
}
*/
int Hungarian::getZeroNumberCountInColLine(int colIndex) {
	int zeroNumberCount = 0;
	int currentNumber = 0;
	for (int i = 0; i < size; i++) {
		currentNumber = (*costs)[i][colIndex];
		if (currentNumber == 0 && (rowLines[i] == 0)) {
			zeroNumberCount++;
		}
	}
	return zeroNumberCount;
}
int Hungarian::getZeroNumberCountInRowLine(int rowIndex) {
	int zeroNumberCount = 0;
	int currentNumber = 0;
	for (int i = 0; i < size; i++) {
		currentNumber = (*costs)[rowIndex][i];
		if (currentNumber == 0 && (colLines[i] == 0)) {
			zeroNumberCount++;
		}
	}
	return zeroNumberCount;
}
int Hungarian::getZeroNumberCountInCol(int index) {
	int zeroNumberCount = 0;
	int currentNumber = 0;
	for (int i = 0; i < size; i++) {
		currentNumber = (*costs)[i][index];
		if (currentNumber == 0)
			zeroNumberCount++;
	}
	return zeroNumberCount;
}
int Hungarian::getZeroNumberCountInRow(int index) {
	int zeroNumberCount = 0;
	int currentNumber = 0;
	for (int i = 0; i < size; i++) {
		currentNumber = (*costs)[index][i];
		if (currentNumber == 0)
			zeroNumberCount++;
	}
	return zeroNumberCount;
}
int Hungarian::minimumLinescoverZeros() {
	int zeroNumberCountRow = 0;
	int zeroNumberCountCol = 0;

	rowLines = {};
	colLines = {};
	for (int i = 0; i < size; i++) {
		rowLines.push_back(0);
		colLines.push_back(0);
	}

	for (int i = 0; i < size; i++) {
		zeroNumberCountRow = getZeroNumberCountInRow(i);
		zeroNumberCountCol = getZeroNumberCountInCol(i);
		zeroRows[i] = zeroNumberCountRow;
		zeroCols[i] = zeroNumberCountCol;
		if (zeroNumberCountRow >= zeroNumberCountCol) {
			rowLines[i] = 1;
			if (zeroNumberCountCol > 1) {
				colLines[i] = 1;
			}
		} else if (zeroNumberCountRow < zeroNumberCountCol) {
			colLines[i] = 1;
			if (zeroNumberCountRow > 1) {
				rowLines[i] = i;
			}
		}/* else {
			if ((zeroNumberCountRow + zeroNumberCountCol) > 1) {
				rowLines[i] = 1;
				colLines[i] = 1;
			}
		}*/
	}

	printf("\nRow Lines:");
	for (auto &i : rowLines) {
		printf("\t%d", i);
	}
	printf("\nCol Lines:");
	for (auto &i : colLines) {
		printf("\t%d", i);
	}

	int numLines = 0;
	int zeroCount = 0;
	for (int i = 0; i < size; i++) {
		if (colLines[i] == 1) {
			numLines++;
			zeroCount = getZeroNumberCountInColLine(i);
			if (zeroCount == 0) {
				colLines[i] = 0;
				numLines--;
			}
		}
	}
	for (int i = 0; i < size; i++) {
		if (rowLines[i] == 1) {
			numLines++;
			zeroCount = getZeroNumberCountInRowLine(i);
			if (zeroCount == 0) {
				rowLines[i] = 0;
				numLines--;
			}
		}
	}

	printf("\nRow Lines:");
	for (auto &i : rowLines) {
		printf("\t%d", i);
	}
	printf("\nCol Lines:");
	for (auto &i : colLines) {
		printf("\t%d", i);
	}
	//int numLines = 0;
	//for (int i : rowLines) {
	//	if (i == 1) numLines++;
	//}
	//for (int i : colLines) {
	//	if (i == 1) numLines++;
	//}
	return numLines;
}

int Hungarian::minimumNumberUncovered() {
	int min = max_value;
	for (int i = 0; i < size; i++) {
		if (rowLines[i]) continue;
		for (int j = 0; j < size; j++) {
			if (colLines[j]) continue;

			if ((*costs)[i][j] < min)
				min = (*costs)[i][j];
		}
	}
	return min;
}
void Hungarian::addMinimumNumberUncovered() {
	int minNumberUncovered = minimumNumberUncovered();
	printf("\n%d", minNumberUncovered);

	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++) {
			if (rowLines[row] && colLines[col]) {
				(*costs)[row][col] += minNumberUncovered;
			}
			else if(!rowLines[row] && !colLines[col]){
				(*costs)[row][col] -= minNumberUncovered;
			}
		}
	}
}

void Hungarian::selectZeros() {


	//vector<int> selects;
	//for (int i = 0; i < size; i++) {
	//	selects.push_back(-1);
	//}

	/*int loop = 0;
	while (loop < size) {
		loop++;
		int min = zeroRows[0];
		int index = 0;
		string type = "row";
		for (int i = 1; i < size; i++) {
			if (zeroRows[i] < min) {
				min = zeroRows[i];
				index = i;
			}
		}
		for (int i = 0; i < size; i++) {
			if (zeroCols[i] < min) {
				min = zeroCols[i];
				index = i;
				type = "col";
			}
		}

		if (type == "row") {
			int col;
			for (col = 0; col < size; col++) {
				if ((*costs)[index][col] == 0) {
					selects[index] = col;
					(*costs)[index][col] = 1;
					break;
				}
			}
			for (int i = col; i < size; i++) {
				if ((*costs)[index][col] == 0)
					(*costs)[index][col] = 1;
			}
			zeroRows[index] = size + 1;
			int s = selects[index];
			for (int i = 0; i < size; i++) {
				(*costs)[i][s] = 1;
			}
			zeroCols[s] = size + 1;
		}
		else {
			int row;
			int select;
			for (row = 0; row < size; row++) {
				if ((*costs)[row][index] == 0) {
					selects[row] = index;
					select = row;
					(*costs)[row][index] = 1;
					break;
				}
			}
			for (int i = row; i < size; i++) {
				if ((*costs)[i][index] == 0)
					(*costs)[i][index] = 1;
			}
			zeroCols[index] = size + 1;
			for (int i = 0; i < size; i++) {
				(*costs)[select][i] = 1;
			}
		}
	}*/

	//printf("\nSelects: ");
	//for (int i = 0; i < size; i++) {
	//	printf("\t%d", selects[i]);
	//}

	vector<int> selects;
	vector<int> markedRow;
	vector<int> markedCol;
	
	for (int i = 0; i < size; i++) {
		markedRow.push_back(-1);
		markedCol.push_back(-1);
	}

	int loop = 0;
	while (loop < size) {
		loop++;
		int min = zeroRows[0];
		int index;
		string type = "row";
		if (min > 1) {
			for (int i = 1; i < size; i++) {
				if (markedRow[i] == -1 && zeroRows[i] < min) {
					min = zeroRows[i];
					index = i;
				}
			}
		}
		if (min > 1) {
			for (int i = 1; i < size; i++) {
				if (markedCol[i] == -1 && zeroCols[i] < min) {
					min = zeroCols[i];
					index = i;
					type = "col";
				}
			}
		}

		if (type == "row") {
			int col;
			for (col = 0; col < size; col++) {
				printf("\nCol: %d", col);
				printf("\n%d", markedCol[col]);
				if (markedCol[col] == -1 && (*costs)[index][col] == 0) {
					markedRow[index] = col;
					markedCol[col] = index;
					zeroCols[col] -= 1;
					zeroRows[index] = 0;
					break;
				}
			}
			for (int i = col; i < size; i++) {
				if ((*costs)[index][i] == 0) {
					zeroCols[col] -= 1;
				}
			}
		}
		else {
			int row;
			for (row = 0; row < size; row++) {
				if (markedRow[row] == -1 && (*costs)[row][index] == 0) {
					markedRow[row] = index;
					markedCol[index] = row;
					zeroRows[row] -= 1;
					zeroCols[index] = 0;
					break;
				}
			}
			for (int i = row; i < size; i++) {
				if ((*costs)[i][index] == 0) {
					zeroRows[i] -= 1;
				}
			}
		}
	}
	printf("\nmarked row");
	for (int i : markedRow) {
		printf("\t%d", i);
	}
	printf("\nmarked col");
	for (int i : markedCol) {
		printf("\t%d", i);
	}
}

void Hungarian::printCosts() {
	printf("\nCosts:");
	for (int i = 0; i < size; i++) {
		printf("\n");
		for (int j = 0; j < size; j++)
			printf("\t%d", (*costs)[i][j]);
	}
}
