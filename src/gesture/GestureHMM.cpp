#include "GestureHMM.h"

//--------------------------------------------------------------
GestureHMM::GestureHMM(int hiddenStates, int alphabet) : HMM(hiddenStates, alphabet) {
}

GestureHMM::GestureHMM(int hiddenStates, int alphabet, mat prior, mat transmat, mat obsmat) :
		HMM(hiddenStates, alphabet, prior, transmat, obsmat) {
}

GestureHMM::~GestureHMM() {
	// TODO Auto-generated constructor stub
}

//--------------------------------------------------------------

vector<double> GestureHMM::train(vector< Gesture* > train_set, int max_iter) {

	this->used_train_set = train_set;

	field<rowvec> data(train_set.size(),1);
	vector< Gesture* >::const_iterator iter;
	int r = 0;
	for ( iter = train_set.begin(); iter != train_set.end(); ++iter ) {
		Gesture * g = *iter;
		vector<int> x = g->getLabeledPoints();
		data(r++,0) = rowvec(g->getLabeledPointsString(" "));
	}

	return HMM::train(data, max_iter);
}

//--------------------------------------------------------------

// TODO : move to an external class (kind of serialization factory)
bool GestureHMM::save(string filename) {
	Gesture *g;
	stringstream ss;
	fix_stream(ss);
	ofxXmlSettings XML;

	XML.clear();

	XML.addValue("name", name);

	XML.addValue("alphabet", alphabet);
	XML.addValue("hiddenStates", hiddenStates);

	XML.addValue( "prior", matrix_to_string(prior) );
	XML.addValue( "transmat", matrix_to_string(transmat) );
	XML.addValue( "obsmat", matrix_to_string(obsmat) );

	XML.addTag("gestures");
	XML.pushTag("gestures");

	for (int i = 0; i < used_train_set.size(); ++i) {
		g = used_train_set[i];

		XML.addTag("train_gestures");
		XML.addAttribute("train_gestures","num",i, i);

		XML.pushTag("train_gestures", i);
		{
			XML.addValue("labeled", g->getLabeledPointsString());
			XML.addValue("raw", vector_to_string( g->getPoints()) );
		}
		XML.popTag();
	}
	XML.popTag();

	XML.saveFile(filename);
}

GestureHMM* GestureHMM::fromXML(string filename){

	ofxXmlSettings XML;
	XML.loadFile(filename);

	string name = XML.getValue("name", "");
	int alphabet = XML.getValue("alphabet", 0);
	int hiddenStates = XML.getValue("hiddenStates", 0);


	mat prior;
	mat transmat;
	mat obsmat;

	prior = matrix_from_string<mat>( XML.getValue("prior", "") );
	transmat = matrix_from_string<mat>( XML.getValue("transmat", "") );
	obsmat = matrix_from_string<mat>( XML.getValue("obsmat", "") );

	GestureHMM * g = new GestureHMM(alphabet, hiddenStates, prior, transmat, obsmat);
	g->name = name;

	XML.pushTag("gestures");
	int num_gesture = XML.getNumTags("train_gestures");
	//cout << "fromXML num_gesture: " << num_gesture << endl;
	if(num_gesture>0) {
		vector<Gesture*> gestures(num_gesture);
		for(int i = 0; i < num_gesture; i++) {
			string labels_string = XML.getValue("train_gestures;labeled", "", i);
			vector<int> labels = intvector_from_string(labels_string);
			vector<ofPoint> raws = ofPointvector_from_string(XML.getValue("train_gestures:raw", "", i));
			gestures[i] = new Gesture(raws, labels);
		}
		//cout << "GestureHMM::fromXML trainset size: " << gestures.size() << endl;
		g->used_train_set = gestures;
	}
	XML.popTag();


	return g;
}

//--------------------------------------------------------------

vector< Gesture* > GestureHMM::get_used_train_set() {
	return this->used_train_set;
}

