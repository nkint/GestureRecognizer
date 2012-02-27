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
	this->pool = PointPool::getInstance();
}

OnlineGestureDrawer::~OnlineGestureDrawer() {
	// TODO Auto-generated destructor stub
}

//--------------------------------------------------------------

void OnlineGestureDrawer::draw(void)
{
	DrawerWidget::draw();  // only when points repetiion will be deleted from DrawerWidget

//	cout << "OnlineGestureDrawer::draw(void) " << g->getPoints().size() << endl;
//
	this->initDraw();
	ofSetColor(0,154,205, 100);
	this->drawPath(this->g->getPoints());
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
	vector<int>& rLabels = g->getLabeledPoints();

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
			cout << "no best matching" << endl;
			text->setLabel("no best matching");
		}
	}


//	stringstream ss;
//	vector<int> centroids_without_repetition = remove_duplicate(g->getLabeledPoints());
//	ostream_iterator<int> set_string( ss, " " );
//	copy(centroids_without_repetition.begin(), centroids_without_repetition.end(), set_string);
//	cout << ss.str() << endl;
}

//--------------------------------------------------------------

void OnlineGestureDrawer::setClassifier(GestureHMMClassifier * c) {
	this->c = c;
}

