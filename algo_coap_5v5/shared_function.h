#pragma once
#include <omnetpp.h>  // include the OMNeT++ header file
#include <stdio.h>
#include <string.h> // needed for string
#include <cstdlib> // needed to generate random number
#include <iostream>
#include <cmath>
#include <time.h>
#include <random>

double randomDouble(double lowerLimit, double upperLimit);
int getDownloadSpeed(double rtt, int size);
double calculateRTO(double rtt, bool isSpurious);
