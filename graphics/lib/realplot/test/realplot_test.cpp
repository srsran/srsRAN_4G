/**
 * \file lib/generic/modulation/Crc_test.cpp
 * \version 1.0
 *
 * \section COPYRIGHT
 *
 * Copyright 2012-2013 The Iris Project Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution
 * and at http://www.softwareradiosystems.com/iris/copyright.html.
 *
 * \section LICENSE
 *
 * This file is part of the Iris Project.
 *
 * Iris is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Iris is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 * \section DESCRIPTION
 *
 * Main test file for Realplot class.
 */

#define BOOST_TEST_MODULE Realplot_Test

#include "Realplot.h"

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <qapplication.h>
#include <cstdlib>
#include <complex>
#include <vector>

typedef std::vector<float> FloatVec;

template<class T>
void getPoints(T* data, int numPoints) {
	for (int i = 0; i < numPoints; i++) {
		data[i] = 10 * ((T) rand() / RAND_MAX);
	}
}

template<class Iterator>
void getPoints(Iterator begin, Iterator end) {
	for (; begin != end; begin++) {
		*begin = 10 * ((double) rand() / RAND_MAX);
	}
}

void *threadMain1(void *arg) {
	Realplot plot;

	float data[1024];

	for (int i = 0; i < 100; i++) {
		getPoints(data, 504);
		plot.setNewData(data, 504);
		boost::this_thread::sleep(boost::posix_time::milliseconds(5));
	}
	return NULL;
}

void *threadMain2(void *arg) {
	Realplot plot;
	double data[1024];

	for (int i = 0; i < 100; i++) {
		getPoints(data, 504);
		plot.setNewData(data, 504);
		usleep(5000);
	}
	return NULL;
}

void *threadMain3(void *arg) {
	Realplot plot;
	FloatVec v(1024);

	for (int i = 0; i < 100; i++) {
		getPoints(v.begin(), v.end());
		plot.setNewData(v.begin(), v.end());
		usleep(5000);
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	int argc2 = 1;
	char* argv2[] = { const_cast<char *>("Realplot_Basic_Test"), NULL };
	QApplication a(argc2, argv2);
	pthread_t threads[3];
	int i;

	if (pthread_create(&threads[0], NULL, threadMain1, NULL)) {
		perror("pthread_create");
		exit(-1);
	}
	if (pthread_create(&threads[1], NULL, threadMain2, NULL)) {
		perror("pthread_create");
		exit(-1);
	}
	if (pthread_create(&threads[2], NULL, threadMain3, NULL)) {
		perror("pthread_create");
		exit(-1);
	}

	qApp->exec();

	for (i=0;i<3;i++) {
		pthread_join(threads[i], NULL);
	}
	exit(0);
}
