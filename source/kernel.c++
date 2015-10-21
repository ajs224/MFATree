#include <cmath>
#include <cstdlib>
#include "Kernel.h"

//#include "mfa_params.h"

// Coagulation kernel definition
double k(double x, double y)
{
    
return kernel->k(x,y);// Calculates each time   
  
//  return (double) 1e0;
//  
//  using namespace mfaAnalytic;
//
//
//	switch (kernelType)
//    {
//    case continuum: // Brownian motion (continuum regine)
//      return (pow(x,1e0/3e0)+pow(y,1e0/3e0))*(pow(x,-1e0/3e0)+pow(y,-1e0/3e0));
//      break;
//    case freemolecular: // Brownian motion (free molecular regine)
//      return pow(pow(x,1e0/3e0)+pow(y,1e0/3e0),2e0)*pow(pow(x,-1e0/3e0)+pow(y,-1e0/3e0),1e0/2e0);
//      break;
//    case kinetic: // Based on kinetic theory
//      return (pow(x,1e0/3e0)+pow(y,1e0/3e0))*pow(x*y,1e0/2e0)*pow(x+y,-3e0/2e0);
//      break;
//    case shearlinear: // Shear (linear velocity profile)
//      return pow(pow(x,1e0/3e0)+pow(y,1e0/3e0),3e0);
//      break;
//    case shearnonlinear: // Shear (nonlinear velocity profile):
//      return pow(pow(x,1e0/3e0)+pow(y,1e0/3e0),7e0/3e0);
//      break;
//    case settling: // Gravitational settling
//      return pow(pow(x,1e0/3e0)+pow(y,1e0/3e0),2e0)*abs(pow(x,1e0/3e0)-pow(y,1e0/3e0));
//      break;
//    case inertiasettling: // Inertia and gravitational settling
//      return pow(pow(x,1e0/3e0)+pow(y,1e0/3e0),2e0)*abs(pow(x,2e0/3e0)-pow(y,2e0/3e0));
//      break;
//    case berry: // Analytic approximation of Berry's kernel
//      return pow(x-y,2e0)*pow(x+y,-1e0);
//      break;
//    case condensation: // Condensation and/or branched-chain polymerisation
//      return (x+2)*(y+2);  // with constant c=2
//      break;
//    case additive:
//      return x+y;
//      break;
//    case multiplicative:
//      return x*y;
//      break;
//    case spmtest:
//      return pow(x*y,1e0/3e0);
//      break;
//    default: // constant kernel
//      return (double) 1e0;
//    }
}


/*
double k(double x, double y)
{
  int a=0;

  return 1e0; //pow(x,a)*pow(y,a);
  
}
*/

