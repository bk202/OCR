#include <Blob.h>

Blob::Blob(int x, int y) {
	min_x = x;
	min_y = y;
	max_x = x;
	max_y = y;
	character = " ";
}

bool Blob::isNear(int x, int y, float threshold) {
	bool near = false;
	int cx = max(min(x, max_x), min_x);
	int cy = max(min(y, max_y), min_y);
	Point2d p1 = Point2d(cx, cy);
	Point2d p2 = Point2d(x, y);
	float distance = norm(Mat(p1), Mat(p2));
	if (distance <= threshold){
		min_x = min(min_x, x);
		min_y = min(min_y, y);
		max_x = max(max_x, x);
		max_y = max(max_y, y);
		near = true;
	}
	return near;
}

void Blob::CalcArea(){
	int width = max_x - min_x;
	int height = max_y - min_y;
	area = width * height;
	cx = (min_x + max_x)/2;
	cy = (min_y + max_y)/2;
}

Blob::~Blob() {};