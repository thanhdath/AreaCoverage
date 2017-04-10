#include <conio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>

#include "hungarian.h"
#include "munkres.h"

using namespace std;

typedef struct sensor {
	float x;
	float y;
	float radius;
} sensor;
typedef struct individual {
	vector<sensor> sensors;
	float fitness;
} individual;

void standardlizeSensors(individual *idvd);

//test 
//const int total_sensors = 5;
//int sensor_types = 3;
//int number_sensors[] = { 1, 2, 2 };
//float radius_sensors[] = { 14.00, 11.20, 8.96 };
//float max_radius = 14.00f;
//float alpha = 0.68f;
//float WIDTH = 100.0f;
//float HEIGHT = 100.0f;


// s1-07
const int total_sensors = 17;
int sensor_types = 3;
int number_sensors[] = {5, 5, 7};
float radius_sensors[] = {14.00, 11.20, 8.96};
float max_radius = 14.00f;
float alpha = 0.68f;
float WIDTH = 100.0f;
float HEIGHT = 100.0f;


// s2-07
//
//const int total_sensors = 24;
//int sensor_types = 3;
//int number_sensors[] = { 6, 8, 10 };
//float radius_sensors[] = { 12.00, 9.60, 7.68 };
//float max_radius = 12.00f;
//float alpha = 0.69f;
//float WIDTH = 100.0f;
//float HEIGHT = 100.0f;


// s4-07

//const int total_sensors = 57;
//int sensor_types = 3;
//int number_sensors[] = { 12, 18, 27 };
//float radius_sensors[] = { 8.00, 6.40, 5.12 };
//float max_radius = 8.00f;
//float alpha = 0.70f;
//float WIDTH = 100.0f;
//float HEIGHT = 100.0f;


// s5-07

//const int total_sensors = 101;
//int sensor_types = 3;
//int number_sensors[] = { 22, 32, 47 };
//float radius_sensors[] = { 6.00, 4.80, 3.84 };
//float max_radius = 6.00;
//float alpha = 0.70f;
//float WIDTH = 100.0f;
//float HEIGHT = 100.0f;


// s5-09
//const int total_sensors = 130;
//int sensor_types = 3;
//int number_sensors[] = { 28, 41, 61 };
//float radius_sensors[] = { 6.00, 4.80, 3.84 };
//float max_radius = 6.00;
//float alpha = 0.90f;
//float WIDTH = 100.0f;
//float HEIGHT = 100.0f;


const int SIZE = 25;
individual *population[SIZE * 2];

Munkres *munkres;

void printPopulation() {
	ofstream file("result.txt", ios::trunc);
	/*
	WIDTH
	HEIGHT
	total_sensors
	number sensor types
	number_sensors array
	radius_sensors
	alpha
	SIZE

	*/
	;
	file << WIDTH << "\n" << HEIGHT << "\n";
	file << total_sensors << "\n";
	file << sensor_types << "\n";
	for (int i = 0; i < sensor_types; i++) {
		file << number_sensors[i] << " ";
	}
	file << "\n";
	for (int i = 0; i < sensor_types; i++) {
		file << radius_sensors[i] << " ";
	}

	file << "\n" << alpha;
	file << "\n" << SIZE << "\n";
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < population[i]->sensors.size(); j++) {
			file << population[i]->sensors[j].x << "," <<
				population[i]->sensors[j].y << "," <<
				population[i]->sensors[j].radius << " ";
		}
		file << "\nfitness " << population[i]->fitness << "\n";
	}

	cout << "Done";
	file.close();
}

float RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

int partitionFitness(individual **idvd, int p, int r) {
	float x = idvd[r]->fitness;
	int i = p - 1;
	individual *temp;
	for (int j = p; j <= r - 1; j++) {
		if (idvd[j]->fitness >= x) {
			i++;
			temp = idvd[i];
			idvd[i] = idvd[j];
			idvd[j] = temp;
		}
	}
	temp = idvd[i + 1];
	idvd[i + 1] = idvd[r];
	idvd[r] = temp;
	return i + 1;
}
void quickSortFitness(individual **idvd, int p, int r) {
	if (p < r) {
		int q = partitionFitness(idvd, p, r);
		quickSortFitness(idvd, p, q - 1);
		quickSortFitness(idvd, q + 1, r);
	}
}

int partition(individual *idvd, int p, int r) {
	float x = idvd->sensors[r].radius;
	int i = p - 1;
	sensor temp;
	for (int j = p; j <= r - 1; j++) {
		if (idvd->sensors[j].radius <= x) {
			i++;
			temp = idvd->sensors[i];
			idvd->sensors[i] = idvd->sensors[j];
			idvd->sensors[j] = temp;
		}
	}
	temp = idvd->sensors[i + 1];
	idvd->sensors[i + 1] = idvd->sensors[r];
	idvd->sensors[r] = temp;
	return i + 1;
}
void quickSort(individual *idvd, int p, int r) {
	if (p < r) {
		int q = partition(idvd, p, r);
		quickSort(idvd, p, q - 1);
		quickSort(idvd, q + 1, r);
	}
}

float distance(sensor c1, sensor c2) {
	float x = c2.x - c1.x;
	float y = c2.y - c1.y;
	return sqrt(x*x + y*y);
}

float min(float x, float y) {
	if (x < y) return x;
	return y;
}
float max(float x, float y) {
	if (x < y) return y;
	return x;
}

void VFA(individual *idvd) {
	for (int i = 0; i < total_sensors; i++) {
		float frx = 0.0f, fry = 0.0f, fax = 0.0f, fay = 0.0f;
		int nr = 0, na = 0;

		for (int j = 0; j < total_sensors; j++) {
			if (j == i) continue;
			float dt = distance(idvd->sensors[i], idvd->sensors[j]);
			float sum_radius = idvd->sensors[i].radius + idvd->sensors[j].radius;
			if (dt < sum_radius) {
				frx += (1 - sum_radius / dt) * (idvd->sensors[j].x - idvd->sensors[i].x);
				fry += (1 - sum_radius / dt) * (idvd->sensors[j].y - idvd->sensors[i].y);
				nr++;
			}
			if (dt > sum_radius) {
				fax += (1 - sum_radius / dt) * (idvd->sensors[j].x - idvd->sensors[i].x);
				fay += (1 - sum_radius / dt) * (idvd->sensors[j].y - idvd->sensors[i].y);
				na++;
			}
		}
		sensor barriers[] = {
			{idvd->sensors[i].x, 0},
			{idvd->sensors[i].x, HEIGHT},
			{0, idvd->sensors[i].y},
			{WIDTH, idvd->sensors[i].y}
		};
		
		for (int j = 0; j < 4; j++) {
			float dt = distance(idvd->sensors[i], barriers[j]);
			if (dt < idvd->sensors[i].radius) {
				frx += (1 - idvd->sensors[i].radius / dt)*(barriers[j].x - idvd->sensors[i].x);
				fry += (1 - idvd->sensors[i].radius / dt)*(barriers[j].y - idvd->sensors[i].y);
				nr++;
			}
			else if (dt > idvd->sensors[i].radius) {
				fax += (1 - idvd->sensors[i].radius / dt)*(barriers[j].x - idvd->sensors[i].x);
				fay += (1 - idvd->sensors[i].radius / dt)*(barriers[j].y - idvd->sensors[i].y);
				na++;
			}
		}
		/*if (nr != 0 && na != 0) {
			idvd->sensors[i].x += 0.9 * frx / nr + 0.1 * fax / na;
			idvd->sensors[i].y += 0.9 * fry / nr + 0.1 * fay / na;
		}
		else if (nr == 0) {
			idvd->sensors[i].x += 0.4 * fax / na;
			idvd->sensors[i].y += 0.4 * fay / na;
		}
		else if (na == 0) {
			idvd->sensors[i].x += frx / nr;
			idvd->sensors[i].y += fry / nr;
		}*/
		if (nr != 0) {
			idvd->sensors[i].x += frx / nr;
			idvd->sensors[i].y += fry / nr;
		}
		idvd->sensors[i].x = round(idvd->sensors[i].x * 1000) / 1000;
		idvd->sensors[i].y = round(idvd->sensors[i].y * 1000) / 1000;
	}
	standardlizeSensors(idvd);
}

float overlap(sensor s1, sensor s2) {
	float ol = 0;
	float dt = distance(s1, s2);
	float sum_radius = s1.radius + s2.radius;
	if (dt >= sum_radius) return 0;

	if (fabs(s1.radius - s2.radius) <= dt && dt < sum_radius) {
		float gamma = sum_radius*min(s1.radius, s2.radius) / (max_radius*max(s1.radius, s2.radius));
		return gamma*(sum_radius - dt);
	}

	// dt < fabs(s1.radius - s2.radius)
	float beta = max_radius * 2 + 0.1;
	return beta*min(s1.radius, s2.radius);
}

float fitness_fn(individual *idvd) {
	float olap = 0;
	for (int i = 0; i < total_sensors; i++) {

		for (int j = i + 1; j < total_sensors; j++) {
			olap += overlap(idvd->sensors[i], idvd->sensors[j]);
		}
		sensor barriers[] = {
			{ idvd->sensors[i].x, 0 },
			{ idvd->sensors[i].x, HEIGHT },
			{ 0, idvd->sensors[i].y },
			{ WIDTH, idvd->sensors[i].y }
		};
		for (int j = 0; j < 4; j++) {
			float dt = distance(idvd->sensors[i], barriers[j]);
			if(dt < idvd->sensors[i].radius)
				olap += (idvd->sensors[i].radius - dt)*idvd->sensors[i].radius;
		}
	}
	return 1 / olap;
}

// Random sensor sensors
individual* randomInitialization() {
	individual *idvd = new individual{};

	for (int type = 0; type < sensor_types; type++) {
		for (int j = 0; j < number_sensors[type]; j++) {
			float x = RandomFloat(radius_sensors[type], WIDTH - radius_sensors[type]);
			float y = RandomFloat(radius_sensors[type], HEIGHT - radius_sensors[type]);
			idvd->sensors.push_back({ x, y, radius_sensors[type] });
		}
	}
	VFA(idvd);
	return idvd;
}

void standardlizeSensors(individual *idvd) {
	for (int i = 0; i < total_sensors; i++) {
		if (idvd->sensors[i].x < idvd->sensors[i].radius)
			idvd->sensors[i].x = idvd->sensors[i].radius;
		if (idvd->sensors[i].y < idvd->sensors[i].radius)
			idvd->sensors[i].y = idvd->sensors[i].radius;
		float max_x = WIDTH - idvd->sensors[i].radius;
		float max_y = HEIGHT - idvd->sensors[i].radius;
		if (idvd->sensors[i].x > max_x) idvd->sensors[i].x = max_x;
		if (idvd->sensors[i].y > max_y) idvd->sensors[i].y = max_y;
	}
}

individual* heuristicInitialization() {
  individual *idvd = new individual{};

  float *setR = (float *)malloc(sizeof(float)*total_sensors);
  int index = 0;
  for(int i = 0; i < sensor_types; i++) {
    for(int j = 0; j < number_sensors[i]; j++) {
		setR[index++] = radius_sensors[i];
    }
  }
  // permutation
  int length = total_sensors / 2;
  float temp;
  for (int i = 0; i < total_sensors; i++) {
	  int j = (int) RandomFloat(0, total_sensors);
	  temp = setR[i];
	  setR[i] = setR[j];
	  setR[j] = temp;
  }

  idvd->sensors.push_back({setR[0], setR[0], setR[0]});
  int count1 = 1; // number sensors in current row
  int count2 = 0; // number sensors in row below
  for(int i = 1; i < total_sensors; i++) {
    float x = idvd->sensors[i-1].x + setR[i-1] + setR[i];
    float y = 0;
    if(x >= WIDTH) {
      count2 = count1;
      count1 = 1;
      x = setR[i];
      y = idvd->sensors[i-count2].y + setR[i-count2] + setR[i];
    } else {
      ++count1;
      if(count2 == 0) {
        y = setR[i];
      } else {
        y = idvd->sensors[i-count2].y + setR[i-count2] + setR[i];
      }
    }
    idvd->sensors.push_back({x, y, setR[i]});
  }

  standardlizeSensors(idvd);
  quickSort(idvd, 0, total_sensors - 1);
  return idvd;
}

void initializePopulation() {
	for (int i = 0; i < 2 * SIZE; i++) {
		population[i] = randomInitialization();
		//population[i] = heuristicInitialization();
		population[i]->fitness = fitness_fn(population[i]);
	}
	quickSortFitness(population, 0, 2 * SIZE - 1);
}

bool reduceDistance(individual *id_1, individual *id_2) {
	int start_index = 0;
	int total_cost = 0;
	// http://jsfiddle.net/jjcosare/6Lpz5gt9/2/
	for (int type = 0; type < sensor_types; type++) {
		int size = number_sensors[type];
		vector<vector<int>> costs;

		// calculate distance matrix
		for (int i = 0; i < size; i++) {
			vector<int> temp;
			costs.push_back(temp);
			for (int j = 0; j < size; j++) {
				costs[i].push_back(
					(int)(distance(id_1->sensors[i + start_index], id_2->sensors[j + start_index])*100)
				);
			}
		}

		//hungarian = new Hungarian(&costs, size, max(WIDTH, HEIGHT));
		delete munkres;
		munkres = new Munkres(&costs, size, max(WIDTH, HEIGHT));
		munkres->Run();

		vector<vector<int>> *Mask = munkres->getMask();
		
		for (int i = 0; i < size; i++) {
			int index_swap = -1;
			for (int j = 0; j < size; j++) {
				if ((*Mask)[i][j] == 1) {
					index_swap = j;
					total_cost += costs[i][j];
					break;
				}
			}
			sensor tmp = id_1->sensors[i];
			id_1->sensors[i] = id_1->sensors[index_swap];
			id_1->sensors[index_swap] = tmp;
		}

		start_index += size;
	}
	if (total_cost == 0)
		return false;
	return true;
}

individual* crossOver(individual *id_1, individual *id_2) {
	individual *id_new = new individual{};
	float a = 0.5;
	for (int i = 0; i < total_sensors; i++) {
		float min_range_x = min(id_1->sensors[i].x, id_2->sensors[i].x) - a*fabs(id_1->sensors[i].x - id_2->sensors[i].x);
		float max_range_x = max(id_1->sensors[i].x, id_2->sensors[i].x) + a*fabs(id_1->sensors[i].x - id_2->sensors[i].x);
		float min_range_y = min(id_1->sensors[i].y, id_2->sensors[i].y) - a*fabs(id_1->sensors[i].y - id_2->sensors[i].y);
		float max_range_y = max(id_1->sensors[i].y, id_2->sensors[i].y) + a*fabs(id_1->sensors[i].y - id_2->sensors[i].y);

		min_range_x = max(min_range_x, id_1->sensors[i].radius);
		max_range_x = min(max_range_x, WIDTH - id_1->sensors[i].radius);
		min_range_y = max(min_range_y, id_1->sensors[i].radius);
		max_range_y = min(max_range_y, HEIGHT - id_1->sensors[i].radius);
		float ux = RandomFloat(min_range_x, max_range_x);
		float uy = RandomFloat(min_range_y, max_range_y);

		id_new->sensors.push_back({ ux, uy, id_1->sensors[i].radius });
	}
	return id_new;
}

void mutation(individual *idvd, individual *father, individual *mother) {
	for (int i = 0; i < total_sensors;i++) {
		int r = (int)(rand() % (total_sensors));
		if (r == 1) {
			float variance = father->sensors[i].x - mother->sensors[i].x;
			if (variance > 0) {
				float x = idvd->sensors[i].x;
				float y = idvd->sensors[i].y;
				float inc_x = 1 / (variance*sqrt(2 * M_PI)) * exp((-x*x) / (2 * variance*variance));
				float inc_y = 1 / (variance*sqrt(2 * M_PI)) * exp((-y*y) / (2 * variance*variance));
				idvd->sensors[i].x += inc_x;
				idvd->sensors[i].y += inc_y;
			}
		}
	}
	VFA(idvd);
}

int main() {
	srand(time(NULL));
	rand();

	initializePopulation();
	//for (int i = 1; i < SIZE; i+=2) {
	//	
	//	for (int j = 0; j < total_sensors; j++) {
	//		population[i]->sensors[j].x = population[i - 1]->sensors[j].x;
	//		population[i]->sensors[j].y = population[i - 1]->sensors[j].y;
	//	}
	//	VFA(population[i]);
	//}

	for (int i = 0; i < 1000; i++) {
		for (int j = SIZE; j < 2 * SIZE; j++) {
			delete population[j];

			int father = (int)(rand() % SIZE);
			int mother = (int)(rand() % SIZE);
			while(father == mother) mother = (int)(rand() % SIZE);

			/*int index = SIZE;
			if (!reduceDistance(population[father], population[mother])) {
				population[j] = population[mother];
				population[mother] = population[index++];
				if (index == 2 * SIZE) break;
			}*/

			individual *new_individual = crossOver(population[father], population[mother]);

			int r = (int)(rand() % 100);
			if (r >= 0 && r <= 5)
				mutation(new_individual, population[father], population[mother]);
			new_individual->fitness = fitness_fn(new_individual);
			population[j] = new_individual;
		}
		quickSortFitness(population, 0, 2 * SIZE - 1);
	}
	printPopulation();
	//getchar();
	return 0;
}
