/*
 * DrawerWidget.h
 *
 *  Created on: Jan 30, 2012
 *      Author: alberto
 */

#ifndef DRAWERWIDGET_H_
#define DRAWERWIDGET_H_

#include "ofMain.h"
#include "ofPoint.h"

#include "../ofxhGui/src/hGui.h"
#include "../ofxhGui/src/hWidget.h"
#include "../ofxhUtils/src/hEvents/hEvents.h"

#include <iostream>
using namespace std;

class DrawerWidget : public hWidget{
public:
	DrawerWidget(std::string name, hPanel * parent, int dispMode, int xx, int yy, int width, int height);
	virtual ~DrawerWidget();

    virtual void draw(void);

	virtual void mousePressed(int xx, int yy, int btn);
    virtual void mouseReleased(int xx, int yy, int btn);
    virtual void mouseDragged(int xx, int yy, int btn);

    virtual void setMessage(string msg);

    vector<ofPoint> getPoints();

    bool active;

protected:

    void initDraw();
    void endDraw();
    void drawPath(vector<ofPoint> points);

    vector<ofPoint> points;
    bool drawing;

private:
};

#endif /* DRAWERWIDGET_H_ */
