#pragma once


#include "ofMain.h"
#include "ofImage.h"
#include "ofSystemUtils.h"

#include "ofxhUtils/src/hEvents/hEvents.h"
#include "ofxhUtils/src/hEvents/hObject.h"
#include "ofxhGui/src/hGui_all.h"
#include "ofxhGui/src/hScrollBar.h"

#include "utils.h"
#include "widget/DrawerWidget.h"
#include "widget/GestureThrower.h"
#include "widget/MultiGestureDrawer.h"
#include "widget/OnlineGestureDrawer.h"
#include "gesture/Gesture.h"
#include "gesture/GestureHMM.h"
#include "gesture/GestureHMMClassifier.h"
#include "test.h"

#include <iostream>
#include <iterator>
#include <set>
#include <stdio.h>

#include <armadillo>

class testApp : public ofBaseApp, public hObject {

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		void _event_new_gesture(hEventArgs& args);
		void _event_add_gesture(hEventArgs& args);
		void _event_visualize_gesture(hEventArgs& args);
		void _event_train(hEventArgs& args);
		void _event_clear_gestures(hEventArgs& args);
		void _event_visualize_hmm(hEventArgs& args);
		void _event_hmm_name(hEventArgs& args);
		void _event_classify(hEventArgs& args);
		void _event_save(hEventArgs& args);
		void _event_load(hEventArgs& args);
		void _event_classify_online(hEventArgs& args);

		void visualize_gesture(Gesture * g);
		void visualize_hmm(GestureHMM * hmm);
		void clear_trainset();
		void add_hmm(GestureHMM *hmm);

	private:
		hGui * gui;
		void setupGui(void);
		void setDarkColors(void);
		hPanel * panel1(hGui * gui, hEvents * events);
		hPanel * panel2(hGui * gui, hEvents * events);
		hPanel * panel3(hGui * gui, hEvents * events);
		hPanel * panel4(hGui * gui, hEvents * events);

//		void skin1();
//    void toOf(vector<Point3f> & v);
    vector<Gesture* > train_set;
		int gesture_number;

		int hmm_number;
		int selected_hmm;

		GestureHMMClassifier c;
};
