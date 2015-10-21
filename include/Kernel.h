/* 
 * File:   kernel.h
 * Author: ajs224
 *
 * Created on 08 September 2014, 20:36
 */

#ifndef KERNEL_H
#define	KERNEL_H



#include <string>
#include <cmath>
#include <cstdlib>

class Kernel
{
 protected:
  double A;
  std::string kernelName;
 public:
 Kernel(double val, std::string name) : A(val), kernelName(name) {}
  virtual double k(double x, double y) =0;
  void setA(double val){ A = val;}
  std::string Name() {return kernelName;}
};

class Constant: public Kernel
{
 public:
 Constant(double val) : Kernel(val, "constant") {}
  double k(double x, double y) { return A;}
};

class Additive: public Kernel
{
 public:
 Additive(double val) : Kernel(val, "additive") {}
  double k(double x, double y) { return A*(x+y);}
};

class Multiplicative: public Kernel
{
 public:
 Multiplicative(double val) : Kernel(val, "multiplicative") {}
  double k(double x, double y) { return A*x*y;}
};


// Brownian motion (continuum regime)
class Continuum: public Kernel
{
 public:
 Continuum(double val) : Kernel(val, "continuum") {}
  double k(double x, double y) { return A*(pow(x,1e0/3e0)+pow(y,1e0/3e0))*(pow(x,-1e0/3e0)+pow(y,-1e0/3e0));}
};

// Brownian motion (freemolecular regime)
class Freemolecular: public Kernel
{
 public:
 Freemolecular(double val) : Kernel(val, "freemolecular") {}
  double k(double x, double y) { return A*pow(pow(x,1e0/3e0)+pow(y,1e0/3e0),2e0)*sqrt((1e0/x)+(1e0/y));}
};
// Transition kernel
class Transition: public Kernel
{
 public:
 Transition(double val) : Kernel(val, "transition") {}
  double k(double x, double y){ return A*(pow(x,1e0/3e0)+pow(y,1e0/3e0))*(pow(x,-1e0/3e0)+pow(y,-1e0/3e0))*1e0/(1e0+(pow(x,1e0/3e0)+pow(y,1e0/3e0))*(pow(x,-1e0/3e0)+pow(y,-1e0/3e0))/(pow(pow(x,1e0/3e0)+pow(y,1e0/3e0),2e0)*sqrt((1e0/x)+(1e0/y))));};
};

// Based on kinetic theory
class Kinetic: public Kernel
{
 public:
 Kinetic(double val) : Kernel(val, "kinetic") {}
  double k(double x, double y) { return A*(pow(x,1e0/3e0)+pow(y,1e0/3e0))*pow(x*y,1e0/2e0)*pow(x+y,-3e0/2e0);}
};

// Shear (linear velocity profile)
class Shearlinear: public Kernel
{
 public:
 Shearlinear(double val) : Kernel(val, "shearlinear") {}
  double k(double x, double y) { return A*pow(pow(x,1e0/3e0)+pow(y,1e0/3e0),3e0);}
};

// Shear (nonlinear velocity profile):
class Shearnonlinear: public Kernel
{
 public:
  Shearnonlinear(double val) : Kernel(val, "shearnonlinear") {}
  double k(double x, double y) { return A*pow(pow(x,1e0/3e0)+pow(y,1e0/3e0),7e0/3e0);}
};

// Gravitational settling
class Settling: public Kernel
{
 public:
 Settling(double val) : Kernel(val, "settling") {}
  double k(double x, double y) { return A*pow(pow(x,1e0/3e0)+pow(y,1e0/3e0),2e0)*abs(pow(x,1e0/3e0)-pow(y,1e0/3e0));}
};


// Inertia and gravitational settling
class Inertiasettling: public Kernel
{
 public:
  Inertiasettling(double val) : Kernel(val, "inertiasettling") {}
  double k(double x, double y) { return A*pow(pow(x,1e0/3e0)+pow(y,1e0/3e0),2e0)*abs(pow(x,2e0/3e0)-pow(y,2e0/3e0));}
};

// Analytic approximation of Berry's kernel
class Berry: public Kernel
{
 public:
 Berry(double val) : Kernel(val, "berry") {}
  double k(double x, double y) { return A*pow(x-y,2e0)*pow(x+y,-1e0);}
};


// Condensation and/or branched-chain polymerisation
// with constant c=2 
class Condensation: public Kernel
{
 public:
 Condensation(double val) : Kernel(val, "condensation") {}
  double k(double x, double y) { return A*(x+2)*(y+2);}
};

//SPM test kernel
class SPMtest: public Kernel
{
 public:
 SPMtest(double val) : Kernel(val, "spmtest") {}
  double k(double x, double y) { return A*pow(x*y,1e0/3e0);}
};


/*
class : public Kernel
{
 public:
 (double val) : Kernel(val, "") {}
  double k(double x, double y) { return A*;}
};
*/















#endif	/* KERNEL_H */

