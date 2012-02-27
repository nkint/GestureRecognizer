/*
 * HMMClassifier.h
 *
 *  Created on: Feb 22, 2012
 *      Author: alberto
 */

#ifndef GESTUREHMMCLASSIFIER_H_
#define GESTUREHMMCLASSIFIER_H_

#include "HMMClassifier.h"
#include "GestureHMM.h"

using namespace std;

class GestureHMMClassifier: public HMMClassifier {

public:
	GestureHMMClassifier();
	virtual ~GestureHMMClassifier();

//	void add(GestureHMM* hmm);
//	GestureHMM* get(int i);

	GestureHMM* classify(Gesture& data, double &l, double threashold=-500);
//private:
//	vector<GestureHMM*> hmms;
};

#endif /* GESTUREHMMCLASSIFIER_H_ */
