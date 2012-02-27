/*
 * HMMClassifier.cpp
 *
 *  Created on: Feb 27, 2012
 *      Author: alberto
 */

#include "HMMClassifier.h"

HMMClassifier::HMMClassifier() {
}

HMMClassifier::~HMMClassifier() {
}


//-------------------------------------------------------------- getter && setter
																 // TODO:
																 // collection interface
																 // []operator
void HMMClassifier::add(HMM* hmm) {
	this->hmms.push_back(hmm);
}
HMM* HMMClassifier::get(int i) {
	return this->hmms.at(i);
}

HMM *HMMClassifier::classify(rowvec & data, double & l, double threashold)
{
	field<rowvec> data1(data.n_rows,1);
	data1(0,0) = data;

	map<double, HMM*> loglik2hmm;
	double loglik;
	double maxlik = -std::numeric_limits<double>::max();
	int hmm_size = hmms.size();
	for (int i = 0; i < hmm_size; ++i) {
		loglik = hmms.at(i)->evaluate(data1);
		if (loglik>maxlik)
			maxlik = loglik;
		HMM *pHMM = hmms[i];
		loglik2hmm[loglik] = pHMM;
	}

	l = maxlik;
	if(maxlik>threashold)
		return loglik2hmm[maxlik];
	else
		return NULL;
}


