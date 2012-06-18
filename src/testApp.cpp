
#include "testApp.h"
#include "gui.h"


//--------------------------------------------------------------
void testApp::setup(){

	fix_stream(cout);

	ofBackground(127,127,127);
	ofSetWindowTitle("gesture recognition - Armadillo");
	ofEnableAlphaBlending ();

	this->setupGui();

	gesture_number = 0;
	hmm_number = 0;
	selected_hmm = -1;

	add_hmm( GestureHMM::fromXML("o.xml") );
	add_hmm( GestureHMM::fromXML("i.xml") );

	test();

	OnlineGestureDrawer * drawer = static_cast<OnlineGestureDrawer*> (hEvents::getInstance()->getObject("recognizer_online"));
	drawer->setClassifier(&c);
}


//--------------------------------------------------------------
void testApp::update(){
	
}

//--------------------------------------------------------------
void testApp::draw(){
	//gesture_draw.draw(gui->margin2, gui->margin2);
}

//-------------------------------------------------------------- gui event handling

void testApp::_event_new_gesture(hEventArgs& args) {

	cout << "testApp::_event_new_gesture" << endl;

	GestureThrower * drawer = static_cast<GestureThrower*>(hEvents::getInstance()->getObject("drawer"));
	visualize_gesture(drawer->getGesture());

}
void testApp::_event_add_gesture(hEventArgs& args) {
    GestureThrower * drawer = static_cast<GestureThrower*>(hEvents::getInstance()->getObject("drawer"));
    hListBox * lb = static_cast<hListBox*>(hEvents::getInstance()->getObject("listBox"));

	// take the path from Drawer and add in the map
	Gesture * g = drawer->getGesture();
	if(g != NULL && g->points().size()>1) {
		train_set.push_back(g);
		gesture_number++;
		string s = to_string(gesture_number);

		// add the element in the list box
		if(gesture_number <= TRAINSET_LENGTH)
			lb->setElementLabel(gesture_number, s);
		else {
			lb->addData(s);

			// update the scroll
			lb->scrollItems(gesture_number-TRAINSET_LENGTH);
			lb->getScrollBar()->setPosition(gesture_number-TRAINSET_LENGTH);
		}

		lb->selectElement(gesture_number);
	}
}

void testApp::_event_visualize_gesture(hEventArgs& args) {
	int selected_gesture = args.values[0]-1;
	cout << "visualize_gesture " << selected_gesture << endl;
	if(selected_gesture>=0 && selected_gesture<=gesture_number) {
		Gesture * g = train_set[selected_gesture];
		visualize_gesture(g);
	}
}

void testApp::_event_train(hEventArgs& args) {
	cout << "testApp::train" << endl;

	// create hmm from train_set
	if(train_set.size()==0) return;
	GestureHMM *hmm = new GestureHMM(12, 10);
	hmm->name = "an hidden markov model";
	hmm->train(train_set, 15);
    add_hmm(hmm);

    hTabBox *mainTabBox = static_cast<hTabBox*>(hEvents::getInstance()->getObject("mainTabBox"));
	mainTabBox->selectItem(2);
	clear_trainset();
}

void testApp::_event_clear_gestures(hEventArgs & args)
{
	clear_trainset();
}

void testApp::_event_visualize_hmm(hEventArgs& args) {
	selected_hmm = args.values[0]-1;
	if(selected_hmm>=0 && selected_hmm<hmm_number) {
		GestureHMM *pGHMM = static_cast<GestureHMM*>(c.get(selected_hmm));
		visualize_hmm(pGHMM);
	}

}

void testApp::_event_hmm_name(hEventArgs& args) {
	//cout << "_event_hmm_name" << endl;

	hTextBox * textBox_hmm_name = static_cast<hTextBox*>(hEvents::getInstance()->getObject("hmm_name"));
	hListBox * lb = static_cast<hListBox*>(hEvents::getInstance()->getObject("listBox_hmm"));

	c.get(selected_hmm)->name = textBox_hmm_name->getLabel();
	lb->setElementLabel(selected_hmm+1, textBox_hmm_name->getLabel());
}

void  testApp::_event_classify(hEventArgs& args) {
	cout << "testApp::_event_classify" << endl;

	double l = 0;
	GestureThrower * drawer = static_cast<GestureThrower*>(hEvents::getInstance()->getObject("recognizer"));
	hTextArea * text = static_cast<hTextArea*>(hEvents::getInstance()->getObject("classify_info"));

	Gesture * g = drawer->getGesture();
	if(g->labels().size() < 2) return;

	GestureHMM* best = c.classify(*g, l);

	stringstream ss;

	ss << "sequence: ";
	vector <int> v = remove_duplicate( g->labels() );
	ostream_iterator<int> set_string( ss, " " );
	copy(v.begin(), v.end(), set_string);
	ss << endl << endl;

	if(best != NULL) {
		ss << "liklihood: " << l;
		ss << endl << endl;

		ss << "gesture:" << best->name;
		ss << endl << endl;
	} else {
		ss << "gesture not recognized!";
		ss << endl << endl;
	}


	text->setText(ss.str());

}

void testApp::_event_save(hEventArgs& args) {

	cout << "testApp::_event_save" << endl;

	// TODO : selected_hmm should be a getter to be sync with listbox
	if(selected_hmm>=0) {
		// TODO check the file does not already exist.
		GestureHMM *g = static_cast<GestureHMM*>(c.get(selected_hmm));
		g->save(g->name+".xml");
	}

}

void testApp::_event_load(hEventArgs& args) {
	cout << "testApp::_event_load" << endl;

	ofFileDialogResult result = ofSystemLoadDialog("open xml", false);
	string filename = result.getName();

	cout << "testApp::_event_load > " << filename << endl;

	GestureHMM * hmm = GestureHMM::fromXML(filename);

    add_hmm(hmm);
}


//-------------------------------------------------------------- routines

void testApp::visualize_gesture(Gesture * g) {
	cout << "testApp::visualize_gesture" << endl;

	GestureThrower * drawer = static_cast<GestureThrower*>(hEvents::getInstance()->getObject("drawer"));
	hTextArea * sequence = static_cast<hTextArea*>(hEvents::getInstance()->getObject("sequence"));

	vector<int> labeled = g->labels();
	stringstream ss;

	if(g!=NULL) {
		ss << "size: " << labeled.size() << endl << endl;

		ss << "centroids: ";

		//set into stringstream
		vector<int> centroids_without_repetition = remove_duplicate(labeled);
		ostream_iterator<int> set_string( ss, " " );
		copy(centroids_without_repetition.begin(), centroids_without_repetition.end(), set_string);

		ss << endl << endl << "labelled points:  ";
		ss << to_string(g->labels());

		drawer->setGesture(g);
		sequence->setText(ss.str());
	}
}

void testApp::clear_trainset() {
	hListBox * lb = static_cast<hListBox*>(hEvents::getInstance()->getObject("listBox"));
	lb->clear();
	// also message is been cleared
	lb->addItems(TRAINSET_LENGTH, "");
	lb->setMessage("visualize_gesture");

	gesture_number = 0;
	train_set.clear();
}

void testApp::visualize_hmm(GestureHMM * hmm) {
	hTextBox * textBox_hmm_name = static_cast<hTextBox*>(hEvents::getInstance()->getObject("hmm_name"));
	MultiGestureDrawer * drawer_hmm = static_cast<MultiGestureDrawer*>(hEvents::getInstance()->getObject("drawer_hmm"));
	hTextArea * textAreaHMMinfo = static_cast<hTextArea*>(hEvents::getInstance()->getObject("hmm_info"));
	hListBox * lb = static_cast<hListBox*>(hEvents::getInstance()->getObject("listBox_hmm"));

	textBox_hmm_name->setLabel(hmm->name);

	// draw of all training gesture
	vector<Gesture* > gs = hmm->trainset();
	int num_gesture = gs.size();
	for (int i = 0; i < num_gesture; ++i) {
		vector<ofPoint>& pts = gs.at(i)->points();
		Gesture::rescale( drawer_hmm->getWidth(), pts );
	}
	//cout << "testApp::visualize_hmm _trainset size: " << gs.size() << endl;
	drawer_hmm->setGestures(gs);

	stringstream ss;
	fix_stream(ss);

	ss << "log likelihoods:" << endl;
	vector<double> LL= hmm->getLL();
	int LLsize = LL.size();
	for(int i=0; i < LLsize; ++i) {
		ss << "iteration " << i << ", loglik = " << LL[i] << endl;
	}
	ss << endl << endl;

	ss << "prior:" << endl;
	hmm->getPrior().raw_print(ss);
	ss << endl << endl;

	ss << "transmat:" << endl;
	hmm->getTransmat().raw_print(ss);
	ss << endl << endl;

	ss << "obsmat:" << endl;
	hmm->getObsmat().raw_print(ss);

	textAreaHMMinfo->setText(ss.str());
	lb->selectItem(selected_hmm+1);
}


void testApp::add_hmm(GestureHMM *hmm) {
    // add hmm in hmm list
    c.add(hmm);
    selected_hmm = hmm_number;
    hmm_number++;
    hListBox *lb = static_cast<hListBox*>(hEvents::getInstance()->getObject("listBox_hmm"));
    if(hmm_number <= HMMS_LENGTH)
		lb->setElementLabel(hmm_number, hmm->name);
	else {
		lb->addData(hmm->name);

		// update the scroll
		lb->scrollItems(gesture_number-HMMS_LENGTH);
		lb->getScrollBar()->setPosition(gesture_number-HMMS_LENGTH);
	}
    visualize_hmm(hmm);

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	//cout << key << endl;

	hEventArgs args;
    hTabBox *mainTabBox = static_cast<hTabBox*>(hEvents::getInstance()->getObject("mainTabBox"));

    cout << mainTabBox->getSelectedItem() << endl;

	if(mainTabBox->getSelectedItem()==1) {
		hTextBox * textBox_hmm_name = static_cast<hTextBox*>(hEvents::getInstance()->getObject("hmm_name"));

		switch (key) {
			case 32: // space
				_event_add_gesture(args);
				break;
			case 13: // enter
				if(!textBox_hmm_name->isSelected())
					_event_train(args);
				break;
			default:
				break;
		}
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
