/*
 * test.h
 *
 *  Created on: Feb 24, 2012
 *      Author: alberto
 */

#ifndef TEST_H_
#define TEST_H_

#include "utils.h"

static
void test_remove_duplicate() {
	vector<int> v;
	v.push_back(1); //123131
	v.push_back(1); //123131
	v.push_back(2);
	v.push_back(2);
	v.push_back(3);
	v.push_back(1);
	v.push_back(3);
	v.push_back(3);
	v.push_back(1);
	v.push_back(1);

	vector<int> v1;
	v1 = remove_duplicate(v);
	for(int i=0; i<v1.size(); i++) {
		cout << v1[i];
	} cout << endl;
}


static
void test() {
	test_remove_duplicate();
}


#endif /* TEST_H_ */
