#ifndef FAUX_ENBPHYLOG_H
#define FAUX_ENBPHYLOG_H

#include<stdio.h>

#define Debug(fmt, ...)   printf("[DEBUG]: %s, " fmt "\n", __func__, ##__VA_ARGS__)
#define Error(fmt, ...)   printf("[ERROR]: %s, " fmt "\n", __func__, ##__VA_ARGS__)
#define Warning(fmt, ...) printf("[WARN]:  %s, " fmt "\n", __func__, ##__VA_ARGS__)
#define Info(fmt, ...)    printf("[INFO]:  %s, " fmt "\n", __func__, ##__VA_ARGS__)

#endif
