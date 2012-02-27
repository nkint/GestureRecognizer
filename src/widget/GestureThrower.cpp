/*
 * GestureThrower.cpp
 *
 *  Created on: Feb 25, 2012
 *      Author: alberto
 */

#include "GestureThrower.h"

GestureThrower::GestureThrower(std::string name, hPanel * parent, int dispMode, int xx, int yy, int width, int height)
	: DrawerWidget(name, parent, dispMode, xx, yy, width, height) {

	float x,y;
	for (int i = 1; i < 4; ++i) {
		x = i*width/4;
		for (int j = 1; j < 4; ++j) {
			y = j*height/4;
			this->centroids.push_back(ofPoint(x,y));
		}
	}
}

GestureThrower::~GestureThrower() {
	// TODO Auto-generated destructor stub
}

//--------------------------------------------------------------

/* virtual */
void GestureThrower::draw(void) {

	this->initDraw();

	// draw centroids
	int size;
	size = this->centroids.size();
	for (int i = 0; i < size; ++i) {
		ofRect(this->centroids[i].x, this->centroids[i].y, 1,1);
	}

	// draw the labeling
	ofSetColor(255,255,255, 50);
	if(labeled_points.size()==points.size()){
		size = labeled_points.size();
		for (int i = 0; i < size; ++i) {
			int label = labeled_points[i] -1; 	//// -1 because labels begin from 1 and not from 0
			ofLine(points[i], centroids[label]);
		}
	}

	ofSetColor(255,0,0);
	this->drawPath(this->points);

	this->endDraw();
}

//--------------------------------------------------------------

/* virtual */
void GestureThrower::mousePressed(int xx, int yy, int btn)
{
	DrawerWidget::mousePressed(xx, yy, btn);

	if (active) {
		this->labeled_points.clear();
	}
}

/* virtual */
void GestureThrower::mouseReleased(int xx, int yy, int btn)
{
	if (active) {
												 // TODO, edge instead w and h
		Gesture::rescale(this->w, this->points); // DrawerWidget is suppose to have width = height (a square)

		this->g = Gesture::build_from_centroids(getPoints(), centroids);
		this->setGesture(g);
	}

	DrawerWidget::mouseReleased(xx, yy, btn);
}

/* virtual */
void GestureThrower::mouseDragged(int xx, int yy, int btn) {
	DrawerWidget::mouseDragged(xx, yy, btn);
}

//--------------------------------------------------------------

vector<int> GestureThrower::getLabeledPoints() {
	return this->labeled_points;
}

void GestureThrower::setGesture(Gesture * g) {
	this->g = g;
	this->points = g->getPoints();
	this->labeled_points = g->getLabeledPoints();
}

Gesture *GestureThrower::getGesture() {
	return this->g;
}

