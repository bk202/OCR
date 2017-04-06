#pragma once
#include <Header.h>
#include <Utility.h>


class Blob {

public:
	int min_x, min_y;
	int max_x, max_y;
	int cx, cy;
	string character;
	int area;
	Blob(int x, int y);
	~Blob();
	bool isNear(int x, int y, float threshold);
	void CalcArea();
};


