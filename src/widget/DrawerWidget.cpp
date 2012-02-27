/*
 * DrawerWidget.cpp
 *
 *  Created on: Jan 30, 2012
 *      Author: alberto
 */

#include <vector>
#include <algorithm>

#include "DrawerWidget.h"

//--------------------------------------------------------------

DrawerWidget::DrawerWidget(std::string name, hPanel * parent, int dispMode, int xx, int yy, int width, int height)
	: hWidget(name, parent, dispMode, xx, yy, width, height){

	//cout << "DrawerWidget" << endl;
	this->drawing = false;
	this->active = true;
}

DrawerWidget::~DrawerWidget() {
	// TODO Auto-generated destructor stub
}

//--------------------------------------------------------------

void DrawerWidget::initDraw() {
	// draw border
	hSetHexColor(hGui::getInstance()->borderColor);
	hFrameRect(x, y, w, h);

	// prepare the draw of path
	ofColor(255,0,0);
	ofPushMatrix();
	ofTranslate(this->x, this->y);

}

void DrawerWidget::endDraw() {
	ofPopMatrix();
}

void DrawerWidget::drawPath(vector<ofPoint> points) {
	int size;
	ofBeginShape();
	size = points.size();
	for (int i = 0; i < size; ++i) {
		ofPoint p = points[i];
		ofVertex(p.x, p.y);
	}
	ofEndShape();

//	ofSetColor(255,255,255);
//	float min_x=0, min_y=0, max_x=0, max_y=0;
//	if(_points.size()>1 && drawing) {
//		min_x = (*std::min_element(_points.begin(), _points.end(), &_compare_min_x)).x;
//		min_y = (*std::min_element(_points.begin(), _points.end(), &_compare_min_y)).y;
//		max_x = (*std::max_element(_points.begin(), _points.end(), &_compare_min_x)).x;
//		max_y = (*std::max_element(_points.begin(), _points.end(), &_compare_min_y)).y;
//
//		ofEllipse(min_x, min_y, 5,5);
//		ofEllipse(max_x, max_y, 5,5);
//	}
}

/* virtual */
void DrawerWidget::draw(void)
{
	//cout << "DrawerWidget::draw" << endl;

	this->initDraw();
	ofSetColor(255,0,0);
	this->drawPath(this->points);
	this->endDraw();
}

//--------------------------------------------------------------

void DrawerWidget::mousePressed(int xx, int yy, int btn)
{
	if (active) {
		this->drawing = true;
		this->points.clear();
	}
}

//--------------------------------------------------------------

void DrawerWidget::mouseReleased(int xx, int yy, int btn)
{
	this->drawing = false;
	if (active) {
		stringstream ss;
		ss << this->data->message;
		string s = ss.str();

		hEvents::getInstance()->sendEvent(s);
	}
}

void DrawerWidget::mouseDragged(int xx, int yy, int btn) {
	if (active) {
		this->points.push_back(ofPoint(xx-this->x,yy-this->y));
	}
}

//--------------------------------------------------------------

vector<ofPoint> DrawerWidget::getPoints() {
	return this->points;
}

void DrawerWidget::setMessage(string msg) {
	this->data->message = msg;
}

