/*
 * MultiGestureDrawer.h
 *
 *  Created on: Feb 20, 2012
 *      Author: alberto
 */

#ifndef MULTIDRAWERWIDGET_H_
#define MULTIDRAWERWIDGET_H_

#include "ofMain.h"
#include "ofPoint.h"

#include "hGui.h"
#include "hEvents.h"
#include "hWidget.h"

#include "../gesture/Gesture.h"


#include <iostream>
using namespace std;

class MultiGestureDrawer: public hWidget {
public:
	MultiGestureDrawer(std::string name, hPanel * parent, int dispMode, int xx, int yy, int width, int height);
	virtual ~MultiGestureDrawer();

	void draw();
	void setGestures(vector<Gesture* > gs);

protected:
	vector<Gesture* > gestures;
};

#endif /* MULTIDRAWERWIDGET_H_ */
