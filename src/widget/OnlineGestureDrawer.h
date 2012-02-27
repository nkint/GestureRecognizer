/*
 * OnlineGestureDrawer.h
 *
 *  Created on: Feb 25, 2012
 *      Author: alberto
 */

#ifndef ONLINEGESTUREDRAWER_H_
#define ONLINEGESTUREDRAWER_H_


#include "hObject.h"
#include "hEvents.h"
#include "hWidget.h"
#include "hGui.h"
#include "hGui_all.h"


#include "DrawerWidget.h"
#include "../gesture/Gesture.h"
#include "../gesture/GestureHMM.h"
#include "../gesture/GestureHMMClassifier.h"
#include "../utils.h"

class OnlineGestureDrawer: public DrawerWidget {
public:
	OnlineGestureDrawer(std::string name, hPanel * parent, int dispMode, int xx, int yy, int width, int height);
	virtual ~OnlineGestureDrawer();

	virtual void mousePressed(int xx, int yy, int btn);
	virtual void mouseReleased(int xx, int yy, int btn);
	virtual void mouseDragged(int xx, int yy, int btn);

	virtual void draw(void);

	void setClassifier(GestureHMMClassifier * c);

protected:
    Gesture * g;
    vector<ofPoint> centroids;

    int frame_size;

    GestureHMMClassifier * c;

};

#endif /* ONLINEGESTUREDRAWER_H_ */
