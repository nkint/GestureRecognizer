#include "GestureHMMClassifier.h"

//--------------------------------------------------------------
GestureHMMClassifier::GestureHMMClassifier() : HMMClassifier() {
}

GestureHMMClassifier::~GestureHMMClassifier() {
}


//--------------------------------------------------------------

GestureHMM* GestureHMMClassifier::classify(Gesture& data, double& l, double threashold) {
	if(! (data.labels().size()>=2)) return 0;
	//assert(data.labels().size()>=2);
	rowvec data1 = conv_to< rowvec >::from(data.labels());
	return static_cast<GestureHMM*>( HMMClassifier::classify(data1, l, threashold));
}

//--------------------------------------------------------------

