/*
 * GestureThrower.h
 *
 *  Created on: Feb 25, 2012
 *      Author: alberto
 */

#ifndef GESTURETHROWER_H_
#define GESTURETHROWER_H_

#include "DrawerWidget.h"
#include "../gesture/Gesture.h"


class GestureThrower: public DrawerWidget {
public:
	GestureThrower(std::string name, hPanel * parent, int dispMode, int xx, int yy, int width, int height);;
	virtual ~GestureThrower();

	virtual void draw(void);

	virtual void mousePressed(int xx, int yy, int btn);
	virtual void mouseReleased(int xx, int yy, int btn);
	virtual void mouseDragged(int xx, int yy, int btn);

	vector<int> getLabeledPoints();
    void setGesture(Gesture * g);
    Gesture *getGesture();

    vector<ofPoint> centroids;

protected:
    Gesture * g;
    vector<int> labeled_points; // TODO : delete this, is a repetiion, it could be getter from g
};

#endif /* GESTURETHROWER_H_ */
