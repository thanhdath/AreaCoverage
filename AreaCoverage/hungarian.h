#ifndef HUNGARIAN_H
#define HUNGARIAN_H

#include <vector>
#include <map>
using namespace std;

class Hungarian {
private:
	vector<vector<int>> *costs;
	int size;
	int max_value;

	vector<int> zeroRows;
	vector<int> zeroCols;

	vector<int> rowLines;
	vector<int> colLines;

	void substractRowMinimal();
	void substractColMinimal();

	int getZeroNumberCountInRow(int index);
	int getZeroNumberCountInCol(int index);
	int getZeroNumberCountInColLine(int colIndex);
	int getZeroNumberCountInRowLine(int rowIndex);
	int minimumLinescoverZeros();

	int minimumNumberUncovered();
	void addMinimumNumberUncovered();

	void selectZeros();

	void printCosts();

public:
	Hungarian(vector<vector<int>> *costs, int size, int max_value);
};

#endif