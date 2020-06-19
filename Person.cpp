#include "Person.h"
#include "stdafx.h"

Person::Person(int x, int y)
{
	StartingFloor = x;
	Destination = y;
	positionX = 0;
    positionY = ((6 - StartingFloor) * 80) + 10;
}

Person::~Person()
{
	
}


int Person::getDestination()
{
	return Destination;
}

int Person::getFloor()
{
	return StartingFloor;
}