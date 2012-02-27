/*
 * GestureHMM.h
 *
 *  Created on: Feb 20, 2012
 *      Author: alberto
 */

#ifndef GESTUREHMM_H_
#define GESTUREHMM_H_

#include "ofxXmlSettings.h"
#include "ofPoint.h"

#include "../utils.h"
#include "../hmm/HMM.h"
#include "Gesture.h"

class GestureHMM: public HMM {
public:
	GestureHMM(int hiddenStates, int alphabet);
	GestureHMM(int hiddenStates, int alphabet, mat prior, mat transmat, mat obsmat);
	virtual ~GestureHMM();

	vector<double> train(vector< Gesture* > train_set, int max_iter);

	bool save(string filename);
	static GestureHMM* fromXML(string filename);

	vector< Gesture* > get_used_train_set();

protected:
	vector< Gesture* > used_train_set;

};



#endif /* GESTUREHMM_H_ */
