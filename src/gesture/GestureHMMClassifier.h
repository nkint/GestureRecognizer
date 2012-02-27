/*
 * HMMClassifier.h
 *
 *  Created on: Feb 22, 2012
 *      Author: alberto
 */

#ifndef HMMCLASSIFIER_H_
#define HMMCLASSIFIER_H_

#include <map>
#include <limits>

#include "GestureHMM.h"

using namespace std;

class GestureHMMClassifier {
public:
	GestureHMMClassifier();
	virtual ~GestureHMMClassifier();

	void add(GestureHMM* hmm);
	GestureHMM* get(int i);

	GestureHMM* classify(Gesture& data, double &l, double threashold=-500);
private:
	vector<GestureHMM*> hmms;
};

#endif /* HMMCLASSIFIER_H_ */
