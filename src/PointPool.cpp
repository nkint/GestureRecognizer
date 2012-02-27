/*
 * PointPool.cpp
 *
 *  Created on: Feb 26, 2012
 *      Author: alberto
 */
#include "PointPool.h"

PointPool* PointPool::instance = 0;

/* static */
PointPool* PointPool::getInstance()
{
	if (instance == 0)
	{
		instance = new PointPool();
	}
	return instance;
}

ofPoint* PointPool::getPoint()
{
	if (resources.empty())
	{
		//std::cout << "Creating new." << std::endl;
		return new ofPoint();
	}
	else
	{
		//std::cout << "Reusing existing." << std::endl;
		ofPoint* resource = resources.front();
		resources.pop_front();
		return resource;
	}
}

void PointPool::disposePoint(ofPoint* object)
{
	object->x = 0;
	object->y = 0;
	object->z = 0;
	resources.push_back(object);
}
