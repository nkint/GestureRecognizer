/*
 * HMMClassifier.h
 *
 *  Created on: Feb 27, 2012
 *      Author: alberto
 */

#ifndef HMMCLASSIFIER_H_
#define HMMCLASSIFIER_H_

#include <map>
#include <limits>

#include "HMM.h"

class HMMClassifier {
public:
	HMMClassifier();
	virtual ~HMMClassifier();

	void add(HMM* hmm);
	HMM* get(int i);

	HMM* classify(rowvec& data, double &l, double threashold=-500);

protected:
	vector<HMM*> hmms;
};

#endif /* HMMCLASSIFIER_H_ */
