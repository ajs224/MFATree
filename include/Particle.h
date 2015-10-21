// Define a new particle class
class Particle
{
public:
  // accessor member functions
  unsigned int getIndex(void) const;
  void setIndex(unsigned int);

  double getDiam(void) const;
  void setDiam(double);

  double getMass(void) const;
  void setMass(double);

  double getVol(void) const;
  
  // Particle default constructor
  Particle();
  // Particle constructor
  Particle(unsigned int initIndex, double initDiam, double initMass);
  // Particle destructor
  //~Particle();
  // Copy constructor
  Particle(const Particle &);

  // Inter-particle coagulation function
  Particle coagulate(const Particle &);

  // Overload the + operator for inter-particle particle coagulation
  Particle operator+(const Particle &);
  
  // Overload the += operator for inter-particle particle coagulation
  Particle operator+=(const Particle &);

  // Particle Functor so we can use std::accumulate
  //double operator() (double result, const Particle & p);
   
private: // change this to protected if we want derived classes to have access
  unsigned int m_Index;
  double m_Diam;
  double m_Mass;
  //double m_Vol;
};


