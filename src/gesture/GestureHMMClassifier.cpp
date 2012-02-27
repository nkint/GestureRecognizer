#include "GestureHMMClassifier.h"

//--------------------------------------------------------------
GestureHMMClassifier::GestureHMMClassifier() : HMMClassifier() {
}

GestureHMMClassifier::~GestureHMMClassifier() {
}


//--------------------------------------------------------------

GestureHMM* GestureHMMClassifier::classify(Gesture& data, double& l, double threashold) {
	rowvec data1 = conv_to< rowvec >::from(data.labels());
	return static_cast<GestureHMM*>( HMMClassifier::classify(data1, l, threashold));
}

//--------------------------------------------------------------

