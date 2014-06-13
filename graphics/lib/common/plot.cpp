/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include "plot.h"
#include <stdio.h>
#include <QApplication>
#include <QMainWindow>
#include <unistd.h>

pthread_t thread;
static int plot_initiated=0;

void *qt_thread(void *arg)
{
	int argc = 1;
	char* argv[] = { const_cast<char *>("libLTE Visualizer"), NULL };
    QApplication app(argc, argv);
	app.exec();
	pthread_exit(NULL);
}

int plot_init() {
	if (!plot_initiated) {
		/** FIXME: Set attributes to detachable */
		if (pthread_create(&thread, NULL, qt_thread, NULL)) {
			perror("phtread_create");
			return -1;
		}
		usleep(100000);
		plot_initiated=1;
	}
	return 0;
}

void plot_exit() {
	if (plot_initiated) {
		pthread_cancel(thread);
	}
  plot_initiated=0;
}

