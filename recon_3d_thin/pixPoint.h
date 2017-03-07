#ifndef PIXPOINT_H
#define PIXPOINT_H

using namespace std;

class pixPoint {
public:
	int x;
	int y;
	int z;

	int pixType;   //0 - slab / 1 - junction / 2 - endpoint

	int visitNum;
	int parent;

	pixPoint();
	pixPoint(int , int, int, int); // x,y,z,pixType


};

#endif