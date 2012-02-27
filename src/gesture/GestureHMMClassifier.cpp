#include "GestureHMMClassifier.h"

//--------------------------------------------------------------
GestureHMMClassifier::GestureHMMClassifier() {
	// TODO Auto-generated constructor stub

}

GestureHMMClassifier::~GestureHMMClassifier() {
	// TODO delete vector<GestureHMM*> hmms and all its content
}

//--------------------------------------------------------------
void GestureHMMClassifier::add(GestureHMM* hmm) {
	this->hmms.push_back(hmm);
}
GestureHMM* GestureHMMClassifier::get(int i) {
	return this->hmms.at(i);
}


//--------------------------------------------------------------

GestureHMM* GestureHMMClassifier::classify(Gesture& data, double& l, double threashold)
{
	rowvec _data1 = conv_to< rowvec >::from(data.getLabeledPoints());
	field<rowvec> data1(_data1.n_rows,1);
	data1(0,0) = _data1;

	map<double, GestureHMM*> loglik2hmm;
	double loglik;
	double maxlik = -std::numeric_limits<double>::max();
	int hmm_size = hmms.size();
	for (int i = 0; i < hmm_size; ++i) {
		loglik = hmms.at(i)->evaluate(data1);
		if (loglik>maxlik)
			maxlik = loglik;
		loglik2hmm[loglik] = hmms[i];
	}

	l = maxlik;
	if(maxlik>threashold)
		return loglik2hmm[maxlik];
	else
		return NULL;
}

//--------------------------------------------------------------

