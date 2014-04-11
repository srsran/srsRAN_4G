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
 * Main test file for scatterplot class.
 */

#include "Scatterplot.h"

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <qapplication.h>
#include <cstdlib>
#include <complex>
#include <vector>

typedef std::complex<float> Cplx;
typedef std::vector<Cplx> CplxVec;

template<class T>
void getPoints(std::complex<T>* data, int numPoints) {
	for (int i = 0; i < numPoints; i++) {
		data[i] = std::complex<T>(2 * ((T) rand() / RAND_MAX) - 1,
				2 * ((T) rand() / RAND_MAX) - 1);
	}
}

template<class Iterator>
void getPoints(Iterator begin, Iterator end) {
	for (; begin != end; begin++) {
		float r = 2 * ((double) rand() / RAND_MAX) - 1;
		float i = 2 * ((double) rand() / RAND_MAX) - 1;
		*begin = Cplx(r, i);
	}
}

void *threadMain1(void *arg) {
	Scatterplot plot;
	std::complex<float> data[1024];

	for (int i = 0; i < 10; i++) {
		getPoints(data, 1024);
		plot.setNewData(data, 1024);
		usleep(100000);
	}
	return NULL;
}

void *threadMain2(void *arg) {
	Scatterplot plot;
	std::complex<double> data[1024];

	for (int i = 0; i < 10; i++) {
		getPoints(data, 1024);
		plot.setNewData(data, 1024);
		usleep(100000);
	}
	return NULL;
}

void *threadMain3(void *arg) {
	Scatterplot plot;
	CplxVec v(1024);

	for (int i = 0; i < 10; i++) {
		getPoints(v.begin(), v.end());
		plot.setNewData(v.begin(), v.end());
		usleep(100000);
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	int argc2 = 1;
	char* argv2[] = { const_cast<char *>("Scatterplot_Basic_Test"), NULL };
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
