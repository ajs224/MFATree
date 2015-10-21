#include <cmath>
#include "Particle.h"
#include "mfa_params.h"

////////////////////////////////////////
// Particle member function defintions
////////////////////////////////////////

// Accessor members
unsigned int Particle::getIndex(void) const
{
  return m_Index;
}

void Particle::setIndex(unsigned int index)
  {
    m_Index=index;
  }

double Particle::getDiam(void) const
{
  return m_Diam;
}

void Particle::setDiam(double diam)
{
  m_Diam=diam;
}

double Particle::getMass(void) const
{
  return m_Mass;
}

void Particle::setMass(double mass)
{
  m_Mass=mass;
}

double Particle::getVol(void) const
{
  // By default these particles are assumed to be spherical
  return MFA::ONE_THIRD*MFA::PI*pow(this->getDiam()/2e0,3e0);
}

// Particle default constructor
Particle::Particle()
{
  m_Index=0;
  m_Diam=0.0;
  m_Mass=0.0;
}

// Particle constructor
Particle::Particle(unsigned int initIndex, double initDiam, double initMass)
{
  m_Index=initIndex;
  m_Diam=initDiam;
  m_Mass=initMass;
}

// Particle destructor
//Particle::~Particle()
//{
//  // Does nothing as of yet
//	//std::cout<<"Deallocatinf!\n";
//}
// Particle copy constructor (currently just shallow copy)
Particle::Particle(const Particle & rhs)
{
  m_Index=rhs.getIndex();
  m_Diam=rhs.getDiam();
  m_Mass=rhs.getMass();
}

// Inter-particle coagulation function
// returns Particle Object
Particle Particle::coagulate(const Particle & coagParticle)
{
  // Just add the masses for now 
  //firstParticle.setMass(firstParticle.getMass()+secondParticle.getMass());

  return Particle(m_Index,m_Diam,m_Mass+coagParticle.getMass());
  
}

// Overload the + operator for inter-particle particle coagulation
Particle Particle::operator+(const Particle & coagParticle)
{
  // Keeping the coaglate member function makes what this is doing more transparent
  return Particle(this->coagulate(coagParticle));
}

Particle Particle::operator+=(const Particle & coagParticle)
{
  // Keeping the coagulate member function makes what this is doing more transparent
  return *this=this->coagulate(coagParticle);
}

/*
double Particle::operator() (double result, const Particle & p)
{
  return result + p.getMass();
}
*/
