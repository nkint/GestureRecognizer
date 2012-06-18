/*
 * OnlineGestureDrawer.h
 *
 *  Created on: Feb 25, 2012
 *      Author: alberto
 */

#ifndef ONLINEGESTUREDRAWER_H_
#define ONLINEGESTUREDRAWER_H_

#include "ofxOsc.h"

// listen on port 12345
#define PORT 12345
#define NUM_MSG_STRINGS 20

#include <map>

#include "../ofxhUtils/src/hEvents/hEvents.h"
#include "../ofxhUtils/src/hEvents/hObject.h"
#include "../ofxhGui/src/hGui.h"
#include "../ofxhGui/src/hWidget.h"
#include "../ofxhGui/src/hGui_all.h"


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

	void reciveOSC();

protected:
    Gesture * g;
    vector<ofPoint> centroids;

    int frame_size;

    GestureHMMClassifier * c;

    ofxOscReceiver	receiver;

    map<int, ofColor> colors;
	map<int, vector<ofPoint> > paths;


};

#endif /* ONLINEGESTUREDRAWER_H_ */
