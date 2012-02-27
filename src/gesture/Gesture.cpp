#include "Gesture.h"

//-------------------------------------------------------------- constructor and deconstructor

// TODO: centroids passed per reference is more efficent?
Gesture::Gesture(vector<ofPoint> points, vector<int> labeled) {
	this->_points = points;
	this->_sequence = labeled;
}

Gesture::~Gesture() {
	// TODO Auto-generated destructor stub
}

//-------------------------------------------------------------- factory

Gesture * Gesture::build_from_centroids(vector<ofPoint> points, vector<ofPoint> centroids) {
	vector<int> labeled = label(points, centroids);
	return new Gesture(points, labeled);
}

//-------------------------------------------------------------- getter && setter

vector<int>& Gesture::labels() {
	return this->_sequence;
}
vector<ofPoint>& Gesture::points() {
	return this->_points;
}

void Gesture::setPoints(vector<ofPoint> & p)
{
	this->_points = p;
}

void Gesture::setLabels(vector<int> & l)
{
	this->_sequence = l;
}

//-------------------------------------------------------------- utils

bool _compare_second(pair<int, float> a, pair<int, float> b) { return a.second < b.second; }
bool _compare_min_x(ofPoint const &p1, ofPoint const &p2) { return p1.x < p2.x; }
bool _compare_min_y(ofPoint const &p1, ofPoint const &p2) { return p1.y < p2.y; }

//-------------------------------------------------------------- statics

int Gesture::labelPoint(vector<ofPoint> & centroids, ofPoint & p)
{
    int K = centroids.size();
    map<int,float> id2distance;
    for(int j = 0;j < K;++j){
        id2distance[j] = p.distanceSquared(centroids[j]);
    }
    pair<int,float> min = *min_element(id2distance.begin(), id2distance.end(), _compare_second);

    return min.first +1; // +1 because labels must begin from 1 and not from 0
}

vector<int> Gesture::label(vector<ofPoint> points, vector<ofPoint> centroids)
{
	vector<int> labeled_points;
    // label data ( computational complexity O(centroids*data) )
    int N = points.size();

    for(int i = 0;i < N;++i){
    	ofPoint& p = points[i];
    	int l = labelPoint(centroids, p);
        labeled_points.push_back(l);
    }
    return labeled_points;
}

void Gesture::rescale(float box_width, vector<ofPoint> &points) {

	// find AABB
	float min_x = 0, min_y = 0, max_x = 0, max_y = 0;
	if(points.size() > 1){
		min_x = (*std::min_element(points.begin(), points.end(), &_compare_min_x)).x;
		min_y = (*std::min_element(points.begin(), points.end(), &_compare_min_y)).y;
		max_x = (*std::max_element(points.begin(), points.end(), &_compare_min_x)).x;
		max_y = (*std::max_element(points.begin(), points.end(), &_compare_min_y)).y;
	}

	float w = max_x - min_x;
	float h = max_y - min_y;

	float target_w, target_h, ratio;
	if (w>h) {
		target_w = box_width;
		ratio = target_w/w;
		target_h = h*ratio;
	} else {
		target_h = box_width;
		ratio = target_h/h;
		target_w = w*ratio;
	}

	// scale the values
	int size = points.size();
	for(int i = 0;i < size;++i){
		points[i].x = ofMap(points[i].x, min_x, max_x, 0, target_w);
		points[i].y = ofMap(points[i].y, min_y, max_y, 0, target_h);
	}
}


