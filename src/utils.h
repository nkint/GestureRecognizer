/*
 * utils.h
 *
 *  Created on: Feb 20, 2012
 *      Author: alberto
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>

#include <armadillo>

#include "ofPoint.h"

static
vector<string> split(const string &s, char delim) {

	// http://stackoverflow.com/a/236803/433685
    vector<string> elems;
	stringstream ss(s);
	string item;
	while(getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

static
double double_from_string( const std::string& s ) {
	// http://stackoverflow.com/a/393027/433685
	std::istringstream i(s);
	double x;
	if (!(i >> x))
		return 0;
	return x;
}

template<typename Stream>
void fix_stream(Stream& stream){
    stream.precision(4);
    stream.setf(ios::fixed);
}

template <class T>
inline string to_string (const T& t)
{
stringstream ss;
ss << t;
return ss.str();
}

template<typename Item>
string to_string(vector<Item>& v, const string& delimiter=" - ") {
	stringstream ss;
	fix_stream(ss);
	int vsize = v.size();
	for(int p=0; p<vsize; p++) {
		ss << v[p] << delimiter;
	}
	return ss.str();
}

template<typename Matrix>
string matrix_to_string(Matrix& m) {
	stringstream ss;
	fix_stream(ss);
	m.save(ss, arma::arma_ascii);
	return ss.str();
}

template<class Matrix>
Matrix matrix_from_string(string s) {
	stringstream ss;
	ss << s;
	Matrix m;
	m.load(ss, arma::arma_ascii);
	return m;
}

static
vector<int> intvector_from_string(string s) {
	/* does not support negative number (TODO) */

	vector<string> tokenized;
	tokenized = split(s, ',');
	vector<int> result;
	int length = tokenized.size();
	int thevalue;

	result.resize(length);
	for(int i=0; i<length; i++) {
		istringstream ss(tokenized[i]);
		ss >> thevalue;
		result[i] = thevalue;
	}
	return result;
}

static
vector<ofPoint> ofPointvector_from_string(string s) {

	vector<string> tokenized = split(s, '-');
	vector<ofPoint> result;
	int length = tokenized.size();

	result.resize(length);
	for (int i = 0; i < length; ++i) {
		vector<string> numbers = split(tokenized[i], ',');
		double x = double_from_string(numbers[0]);
		double y = double_from_string(numbers[1]);
		double z = double_from_string(numbers[2]);
		ofPoint p(x,y,z);

		result[i] = p;
	}

	return result;
}

template<class T>
vector<T> remove_duplicate(vector<T>& vec) {
	int length = vec.size();
	vector<T> result(length);
	result[0] = vec[0];
	int last = 0;
	for(int i=0; i<length; i++)
		if(vec[i] != result[last]){
			last++;
			result[last] = vec[i];
		}
	result.resize(last+1);
	return result;
}

static
bool _compare_second(pair<int, float> a, pair<int, float> b) { return a.second < b.second; }
static
bool _compare_min_x(ofPoint const &p1, ofPoint const &p2) { return p1.x < p2.x; }
static
bool _compare_min_y(ofPoint const &p1, ofPoint const &p2) { return p1.y < p2.y; }


#endif /* UTILS_H_ */

