#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <fstream>

#define _NumberofBins 15
#define _DegNum 360
#define INFINITY 99999999
#define _NumberofRows 25
#define _NumberofCols 100
#define _Distance_TH_H 15
#define _Distance_TH_L 10

using namespace std;
using namespace cv;

inline int min(int a, int b) {
	return ((a <= b) * a) + ((b < a) * b);
}

inline int max(int a, int b) {
	return ((a >= b) * a) + ((b > a) * b);
}

inline float min(float a, float b) {
	return ((a <= b) * a) + ((b < a) * b);
}

inline float max(float a, float b) {
	return ((a >= b) * a) + ((b > a) * b);
}

inline double min(double a, double b) {
	return ((a <= b) * a) + ((b < a) * b);
}

inline double max(double a, double b) {
	return ((a >= b) * a) + ((b > a) * b);
}