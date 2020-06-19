#pragma once
#include <vector>

class Person
{
	public:
		Person(int x, int y);
		int getFloor();
		int getDestination();
		int positionX = 0;
		int positionY = 10;
	private:
		int Destination;
		int StartingFloor;
};

