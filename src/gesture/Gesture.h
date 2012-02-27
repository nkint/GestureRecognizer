/*
 * Gesture.h
 *
 *  Created on: Feb 3, 2012
 *      Author: alberto
 */

#ifndef GESTURE_H_
#define GESTURE_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

#include "ofMain.h"
#include "ofPoint.h"

class Gesture {
public:
	Gesture(vector<ofPoint> points, vector<int> labeled);
	virtual ~Gesture();

	vector<ofPoint>& points();
    vector<int>& labels();

    void setPoints(vector<ofPoint>& p);
    void setLabels(vector<int>& l);

    static Gesture * build_from_centroids(vector<ofPoint> points, vector<ofPoint> centroids);
    static void rescale(float box_width, vector<ofPoint> &points);
    static vector<int> label(vector<ofPoint> points, vector<ofPoint> centroids);
    static int labelPoint(vector<ofPoint> & centroids, ofPoint & p);

protected:
	vector<ofPoint> _points;
    vector<int> _sequence;
};

#endif /* GESTURE_H_ */
