/*
 * MultiGestureDrawer.cpp
 *
 *  Created on: Feb 20, 2012
 *      Author: alberto
 */

#include "MultiGestureDrawer.h"

MultiGestureDrawer::MultiGestureDrawer(std::string name, hPanel * parent, int dispMode, int xx, int yy, int width, int height)
	: hWidget(name, parent, dispMode, xx, yy, width, height) {
}

MultiGestureDrawer::~MultiGestureDrawer() {
	// TODO Auto-generated destructor stub
}

void MultiGestureDrawer::draw(void) {
	int size, n_gesture;

	hSetHexColor(hGui::getInstance()->borderColor);
	hFrameRect(x, y, w, h);

	ofColor(255,0,0);
	ofPushMatrix();
	ofTranslate(this->x, this->y);

	ofSetColor(255,0,0, 100);

	n_gesture = this->gestures.size();
	for(int i=0; i<n_gesture; ++i) {
		Gesture * g = gestures.at(i);

		ofBeginShape();

		size = g->points().size();
		for (int j = 0; j < size; ++j) {
			ofPoint p = g->points()[j];
			ofVertex(p.x, p.y);
		}

		ofEndShape();
	}

	ofPopMatrix();
}

void MultiGestureDrawer::setGestures(vector <Gesture *> gs) {
	this->gestures = gs;
}
