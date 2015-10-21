#ifndef MFA_FUNCS_H
#define MFA_FUNCS_H

#include <string>

// Function headers
double k(double x, double y); // Collision kernel
double kt(double x, double y); // Majorant kernel
double* computeMoments(double* diamArray, int noMoments);
double theta(double x, double rate); // Outflow rate
double mIn(int i=1); // Inflowing particle distribution


int parseArgs(int argc, char *argv[], double & inFactor, double & outFactor, double & tStop, int & N, int & L, std::string & kernelName, bool & coagOn);

double interpMon(double evalTime, double t1, double t2, double p1, double p2)
{
  double m = (p2-p1)/(t2-t1);
  return m*(evalTime-t1)+p1;
};


  

#endif
