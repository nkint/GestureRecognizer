/*
 * OnlineGestureDrawer.cpp
 *
 *  Created on: Feb 25, 2012
 *      Author: alberto
 */

#include "OnlineGestureDrawer.h"

OnlineGestureDrawer::OnlineGestureDrawer(std::string name, hPanel * parent, int dispMode, int xx, int yy, int width, int height)
: DrawerWidget(name, parent, dispMode, xx, yy, width, height) {

	float x,y;
	for (int i = 1; i < 4; ++i) {
		x = i*width/4;
		for (int j = 1; j < 4; ++j) {
			y = j*height/4;
			this->centroids.push_back(ofPoint(x,y));
		}
	}

	this->frame_size = 150;
	this->g = Gesture::build_from_centroids(getPoints(), centroids);

	receiver.setup( PORT );
}

OnlineGestureDrawer::~OnlineGestureDrawer() {
	// TODO Auto-generated destructor stub
}

//--------------------------------------------------------------

void OnlineGestureDrawer::reciveOSC() {
	// check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );

		if (m.getAddress() == "/label/")
		{
			std::cout << "recived label" << std::endl;
			int label = m.getArgAsInt32(0);
			if (colors.count(label)==0) {
				colors[label] = ofColor::fromHsb(ofRandom(255), 255, 255);
			}
			paths[label].push_back(ofPoint(m.getArgAsInt32( 1 ), m.getArgAsInt32( 2 )));
		}
		else if (m.getAddress() == "/label/old") {
			int label = m.getArgAsInt32(0);
			paths.erase (label);

		}
	}

	// and draw
	map<int,vector<ofPoint> >::iterator it;
	for ( it=paths.begin() ; it != paths.end(); it++ ) {
		int label = (*it).first;
		vector<ofPoint> p = (*it).second;
		ofPoint p1, p2;
		if(p.size()>=2) {
			p1 = p[0];

			this->mousePressed(p1.x, p1.y, 0);

			for(int j = 1;j < p.size();j++){
				p2 = p[j];

				this->mouseDragged(p2.x, p2.y, 0);
			}

			this->mouseReleased(p2.x, p2.y, 0);
		}


	}

}

void OnlineGestureDrawer::draw(void)
{
	DrawerWidget::draw();  // only when points repetiion will be deleted from DrawerWidget

//	cout << "OnlineGestureDrawer::draw(void) " << g->points().size() << endl;
//
	this->initDraw();
	ofSetColor(0,154,205, 100);
	this->drawPath(this->g->points());
	reciveOSC();
	this->endDraw();
}

//--------------------------------------------------------------

/* virtual */
void OnlineGestureDrawer::mousePressed(int xx, int yy, int btn)
{
	DrawerWidget::mousePressed(xx, yy, btn);
	this->g = Gesture::build_from_centroids(getPoints(), centroids);
	points = vector<ofPoint>();
}

/* virtual */
void OnlineGestureDrawer::mouseReleased(int xx, int yy, int btn)
{
	DrawerWidget::mouseReleased(xx, yy, btn);
}

/* virtual */
void OnlineGestureDrawer::mouseDragged(int xx, int yy, int btn) {

	int size = points.size();
	int l;
	ofPoint apoint; // = *pool->getPoint();
	vector<int>& rLabels = g->labels();

	apoint.x = xx-this->x;
	apoint.y = yy-this->y;

	if (size >= frame_size) {
		points.erase( this->points.begin() );
		rLabels.erase( rLabels.begin() ); // pop_front snippet TODO complete refactor nedded in data structure
	}
	this->points.push_back(apoint);

	vector<ofPoint> t(this->points);

	Gesture::rescale(w, t); // sadly, AABB change every frame and for scale invariance it should be recalculated
	l = Gesture::labelPoint(centroids, t.back());
	rLabels.push_back(l);

	g->setPoints(t);

	double loglik = 1;
	GestureHMM* best = c->classify(*g, loglik);

	hLabel * text = static_cast<hLabel*>(hEvents::getInstance()->getObject("recognized_online"));
	if(best!=NULL) {
		if(loglik>-200) {
			cout << best->name  << loglik << endl;
			text->setLabel(best->name);
		} else {
			cout << "no best matching " << loglik  << " " << best->name << endl;
			text->setLabel("no best matching");

//			stringstream ss;
//			vector<int> centroids_without_repetition = remove_duplicate(g->labels());
//			ostream_iterator<int> set_string( ss, " " );
//			copy(centroids_without_repetition.begin(), centroids_without_repetition.end(), set_string);
//			ss << endl << endl << "labelled points:  ";
//			ss << to_string(g->labels());
//
//			out << ss.str();

		}
	}

}

//--------------------------------------------------------------

void OnlineGestureDrawer::setClassifier(GestureHMMClassifier * c) {
	this->c = c;
}

