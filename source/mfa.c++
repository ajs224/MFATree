// ----------------------------------------------
// MFA C++
// A. J. Smith (ajs224@cam.ac.uk)
//
//-----------------------------------------------

/*
  This is hardcoded pure multiplicative coagulation (for speed)
   */


#
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <limits> // provides std::numeric_limits<double>::infinity();
#include <iterator>

#include<random> // Need to compile with g++ -std=gnu++0x
#include<time.h>  

#include "random.h"
#include "mfa_functions.h"
#include "mfa_params.h"
#include "Particle.h"

#include "Kernel.h"

   
double k(double x, double y)
{ 
  return 5.0e-1*x*y; // hardcoded multiplicative kernel
    //using namespace mfaAnalytic;
    //return kernel->k(x,y);// Calculates each time 
}

// This is the multiplicative part of the kernel used in the binary tree representation
// For a K(x,y)=Kxy, the distribution appearing in the mass flow algorithm is K(x,y)/y = Kx
// so phi(x) = Kx. In general the kernel will break up into multiplicative parts and we'll have a separate "phi"
// for each part
double phi(double x) 
{ 
  //return exp(-x*x/2e0);
  return 5.0e-1*x;
}

void printMasses(std::vector<Particle> & mfaParticle)
{
  using namespace std;
  cout << "Particle masses:" << endl;
  for (auto pIt = mfaParticle.begin(); pIt != mfaParticle.end(); ++pIt)
	cout << pIt-mfaParticle.begin() << ": " << pIt->getMass() << endl;
  cin.sync();
  cin.get();
}



// updadte tree after an event which changes some particles
void updateTree(std::vector<double> & tree, int height, unsigned long particleIndex, double deltaMass)
{
  // Particles are not stored int the tree, so get a tree index by adding tree height
  unsigned long treeIndex = particleIndex + tree.size();
  
  // All particles contribute to tree[0], so update this
  tree[0] += deltaMass;
   
  for (int level=0; level < height; ++level)
	{
	  if (treeIndex%2 == 0)
		{
		  // If the current node is to the left of its parent, we need to update the parent node
		  treeIndex /=2;
		  tree[treeIndex] += deltaMass;
		  //std::cout << "Updating tree[" << treeIndex << "] = " << tree[treeIndex] << std::endl;
		}
	  else
		{
		  // Don't need to do anything. Walk up the tree until we hit the root
		  treeIndex--;
		  treeIndex /=2;
		  //std::cout << "Skipping treeIndex " << treeIndex << std::endl;
		}
	  
	}
  
}


void printTree(std::vector<double> & tree, int height)
{
  using namespace std;

  cout << "t[0] = " << tree[0] << endl;
  
  
  for (int level=0; level < height; ++level)
	{
	  for(int node = 0; node < pow(2,level); ++node) // Can be efficiently computed with bit shifts?
		{
		  cout <<"t["<< (int) pow(2,level) + node << "] = " << tree[pow(2,level) + node] << endl;
		  
		}
	}

  std::cin.sync();
  std::cin.get();
}


// Grow a new tree from scratch (e.g., at the beginning or when round-off errors have accumulated)
void growTree(std::vector<double> & tree, int height, std::vector<Particle> & Particles)
{
  /*
	tree[0] contains the sum of all children = \sum_{i=0}^{2^N-1}\phi(x_i)
	tree[1] is the root node which contains the sum of all the left children = \sum_{i=0}^{2^{N-1}-1}\phi(x_i)
	The nodes at level m will be stored in tree[2^m-1:2^(m+1)-2]
	e.g., for a tree of height 3
	tree[0] = phi(x_0)+phi(x_1)+phi(x_2)+phi(x_3)+phi(x_4)+phi(x_5)+phi(x_6)+phi(x_7)
	The root is at tree[1] = phi(x_0)+phi(x_1)+phi(x_2)+phi(x_3)
	tree[2], tree[3] are the two nodes at depth 1
	tree[4], tree[5], tree[6], tree[7] are the nodes at depth 2
  */
  
  /*
	% Taking a tree of height 3 as an example, so our distribution has 8
	% elements: x[0],...,x[7]
	% The tree has the following stucture:
	% level: nodes
	% 0: f(x[0])+f(x[1])+f(x[2])+f(x[3])+f(x[4])+f(x[5])+f(x[6])+f(x[7]) // sum of all elements
	% 1: f(x[0])+f(x[1])+f(x[2])+f(x[3]) // root node
	% 2: f(x[0])+f(x[1]), f(x[4])+f(x[5]) // left and right children of root
	% 3: f(x[0]), f(x[2]), f(x[4]), f(x[6]) // left and right children of left and right children of root...
	% 4: x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7] //  not stored
	% To generate an index distribution we generate u~U[0,1] and calculate y = u*sum(f(x[k]) (note this is tree[0])
	% If this is less than the current node we go left.
	% Else we let y = y - current Node and go right
	% Repeat until we hit a leaf
	
  */

    
  double branchSum;
  // Initialise our tree
  for (int level=0; level < height; ++level)
	{
	  for(int node = 0; node < pow(2,level); ++node) // Can be efficiently computed with bit shifts?
		{
		  branchSum = 0;
		  for (int index = pow(2,height-level)*node; index < pow(2,height-level-1)*(2*node+1); ++index)
			{
			  branchSum += phi(Particles[index].getMass());
			}
		  //tree[pow(2,level) - 1 + node] = branchSum;
		  tree[pow(2,level) + node] = branchSum; // will now store sum of all elements in tree[0]
		}
	}


  // Set tree[0] to contain sum of all children
  // i.e., tree[1] + 
  branchSum = 0;
  for (int index = pow(2,height-1); index < pow(2,height); ++index)
	{
	  branchSum += phi(Particles[index].getMass());
	}
  tree[0] = tree[1] + branchSum;
  
  
  
  /*
  std::cout << "Tree = " << std::endl;
  for (auto treeIt = tree.begin(); treeIt != tree.end(); ++treeIt)
	std::cout << *treeIt << std::endl;
  
  std::cout << "particles = " << std::endl;
  for (auto partIt = Particles.begin(); partIt != Particles.end(); ++partIt)
	std::cout << partIt->getMass() << std::endl;
  */
	
}




// Currently this supports a single distribution
int generateIndex(std::vector<double> & tree, int height, double y)
{
  // Select particle index from binary tree
  double value,  targetValue = y;

  int branch = 1;
  // Descend the tree
  for (int level = 1; level<=height; ++level)
	{
	  value = targetValue - tree[branch];

	  if(value > 0 )
		{
		  targetValue = value;
		  branch = 2*branch + 1;
		}
	  else
		branch *=2;

	}

  int randIndex = branch - tree.size(); // index will be from 2^N to 2^(N+1) - 1, so need to subtract 2^N to get a particle index

  // Perhaps need to check for accumulation of rounding errors and if necessary regrow the tree
  //if (randIndex == 0)
  //	growTree(tree, height, mfaParticle);
  

  
  return randIndex;
  
}

/* // original implementation
int generateIndex(std::vector<double> & tree, int height, double y)
{
  // Select particle index from binary tree
  int branch = 1;
  
  // Descend the tree
  for (int level = 0; level<height-1; ++level)
	{
	  if (y < tree[branch])
		branch *= 2; // go left - do this with bit shifts?
	  else
		{
		  y -= tree[branch];
		  branch *=2; branch++; // go right - do this with bit shifts?
		}
	}
  //std::cout << "particle index  = " <<  branch  << std::endl;
  return branch;
  
}
*/

int main(int argc, char *argv[])
{
    
  using namespace std;
  using namespace mfaAnalytic;
  //using namespace ajsRandom;
  
  //bool loadRandState=false; // Generate a new set of seeds
  //bool saveRandState=true; // Write the state so we can rewread later
  
  // Declare a Mersenne Twister random number generator
  //MTRand mtrand;
  
  clock_t startTime, stopTime;
  unsigned long int elapsedClicks;
  double elapsedTime;
  
  bool saveRNGState = true;
  bool loadRNGState = false;

  std::uniform_real_distribution<double> uniformRealDist(0,1.0); 
  
  std::mt19937 generator;
  
  // Non deterministic random numbers (using /dev/random)
  // Low performance, very high quality random numbers
  std::random_device rdev{}; // use /dev/random
  generator.seed(rdev());
  
  if(saveRNGState)
    saveState(generator);
  if(loadRNGState)
    loadState(generator);
  
  // Fixed seed
  //unsigned seed = 1198451325; //
  //generator.seed(seed);
  
  
  double M; // mass density
  double collProb;
  //double* xDiam = NULL;   // Pointer to double, initialize to nothing.
  int N = 1024;           // Number of particles
  int L = 1;// Default number of runs
  
  unsigned int noProcesses = 6; 
  double lambda,lambda0;
  int alpha;
  double t, tStop=1e0; // Current time and stopping time (default 12 seconds)
  double tau; // Waiting time
  double deltaMass;

  
  int eventsTotal;
  
  double collRate, inRate, outRate, rateTotal;
  double minProperty;
  
  //double inFactor=2e0,outFactor=inFactor;
  double inFactor;
  double outFactor;

  double momentScaleFactor; // In MFA number of particles is constant, so m0 = 1 always.  This scale factor (which is proportional to outFactor/inFactor) corrects the calculated moments to give moments for a system with variable particle number  

  bool momentScaling = false; //false;
  bool coagOn=true;
  
  bool firstChosen, secondChosen;
  int firstParticle, secondParticle, particleIndex;


  
  // Hardcode everything for now
  string kernelName;

  
  // Set up output file streams
  ofstream outputFile;
  ofstream momentsFile;
  ofstream momentsDumpFile;
  ofstream diamsFile;

  string ext=".txt";
  
  string desc;
  stringstream out;
  
  // Set output precision
  cout.precision(10);
  cout.width(20);
  cout.setf(ios::scientific);

  momentsFile.precision(10);
  momentsFile.width(20);
  momentsFile.setf(ios::scientific);
  
  momentsDumpFile.precision(10);
  momentsDumpFile.width(20);
  momentsDumpFile.setf(ios::scientific);
  outputFile.precision(8);
  diamsFile.precision(8);

  // Print program header blurb
  cout << endl;
  cout << "Stochastic PBE Solver - A. J. Smith (ajs224@cam.ac.uk)" << endl;
  cout << endl;
  cout << "This code solves the continuous Smoluchowski equation with in/outflow" << endl;
  cout << "and coagulation described by constant, additive and multiplicative kernels " << endl;
  cout << "(admitting analytic solutions) in addition to a range of more physically " << endl;
  cout << "realistic kernels (run with --help for additional information)." << endl;
  cout << endl;

  
  // Parse command line arguments
  if(parseArgs(argc, argv, inFactor, outFactor, tStop, N, L, kernelName, coagOn))
    {
      // Didn't enter any arguments
      return 0;
    }

  // Over-ride most of the command line arguments
  kernelName= "multiplicative";
  //inFactor=0e0;
  //outFactor=inFactor;
  tStop=1e0;
  //L = 10;
  //N = atol(argv[1]); //16;//1024;//32768;//1024;//32768;//8;
  bool debug = false;
  

  
  //if (!coagOn)
  //  kernelName = "nocoag";
  
  // Setup output file names
  string outputFileName=dataDir+kernelName+"_data";
  string momentsFileName=dataDir+kernelName+"_moments";
  string diamsFileName=dataDir+kernelName+"_diameters";

  string momentsDumpFileName=dataDir+kernelName+"_moments_dump";

  
  out << "_alpha" << inFactor << "_beta" << outFactor << "_N" << N << "_L" << L;
  desc=out.str()+"";
  
  outputFileName += desc + ext;
  momentsFileName += desc + ext;
  diamsFileName += desc + ext;
  
  momentsDumpFileName += desc + ext;
  // Open file handles
  outputFile.open(outputFileName.c_str(), ios::out);
  momentsFile.open(momentsFileName.c_str(), ios::out);
  diamsFile.open(diamsFileName.c_str(), ios::out);

  momentsDumpFile.open(momentsDumpFileName.c_str(), ios::out);
  
  // Declare a Mersenne Twister random number generator
  //MTRand mtrand;
  //mtrand=myRand(loadRandState);
  
  // This is a uniform distribution on 0,N-1, so generates a particle index 0 <= i < N 
  std::uniform_int_distribution<int> uniformIntDist(0,N-1); 
  
  // 3D array containing moments at all timesteps for all runs
  vector<vector<vector<double>>> momentsAllRuns(L, vector<vector<double> >(noMoments+1));
  vector<vector<vector<double>>> momentsAllRunsInterp(L, vector<vector<double> >(noMoments)); // don't need to store time here because they're all the same
  vector<double> interpTimes;
  
  vector<double> moments(noMoments,0e0); // vector moments at current timestep of current run  
  vector<Particle> mfaParticle(N,Particle()); // vector of stochastic particles
  
  vector<double> rate(noProcesses,0e0); // vector of rates 
  //vector<double> rate; // vector of rates 
  vector<int> events(noProcesses,0);  // vector to keep track of number of each event/process which have occurred

  enum{coagulation, inflow, outflow}; // Indices of particle processes
    
  //kernelName = kernel->Name();
  cout << "Running stochastic simulation with " << N << " stochastic particles for " << tStop << " seconds." << endl;
  kernelName[0]=toupper(kernelName[0]); // capitalise
  if(coagOn)
    cout << kernelName << " kernel selected." << endl;
  else
    cout << "Solving Cauchy problem (in/outflow only)" << endl;
  
  
  //cout << "Inflow rate (1/alpha):" << 1e0/inFactor << endl;
  //cout << "Outflow rate (1/beta):" << 1e0/outFactor << endl;
    
  // Output moments file header
  momentsFile << "t \t m0 \t m0_err\t m1 \t m1_err\t m2 \t m2_err\t m3 \t m3_err\t m4 \t m4_err" << endl;

  momentsDumpFile << "run \t t \t m0 \t m0_err\t m1 \t m1_err\t m2 \t m2_err\t m3 \t m3_err\t m4 \t m4_err" << endl;

  
  // Start the simulation
  // This can easily be parallelised over threads/cores
  for(int run = 0; run < L; run++)
    {
	  // Reseed the random number generator
      //mtrand.seed(); // This used /dev/urandom
      
      t=0e0;
      
      lambda0=1e0/N;
      
      alpha=0;
      
      // Initialise PSD to a delta delta(x-1), i.e., mono-dispersed (equivalent to setting m(x,0)=delta(x-1))
      for (vector<Particle>::iterator iterPart = mfaParticle.begin(); iterPart != mfaParticle.end(); ++iterPart)
		{
		  iterPart->setIndex(iterPart-mfaParticle.begin());
		  iterPart->setMass((double) 1e0); // Monodispersed initial conditions
		  //iterPart->setMass((double) 2e0); 
		}

		  
	  // Here we will create and initialise our binary tree
	  int height = log2(N); // N leaves to hold the particles, connected N/2 branches above to hold sum of two particles, which are connected to N/4 nranches aboove that to hold sums of 4 particles, etc., up to the root node which holds the sum of all particles (normalisation factor) 
	  cout << "Height = " << height << endl;
	  //int height = log2(tree.size()+1);

	  vector<double> tree(pow(2,height)); // root node is now tree[1] and child sum is stored in tree[0]
	  cout << "Tree size = " << tree.size() << endl;
	  growTree(tree, height, mfaParticle);




/*

	  int size = 1024;
	  int p = 10;

	  cout << "p = " << p << endl;
	  cout << "size = " << size << endl;

	  int divMe = 8;
	  size = divMe>>1; // This divides divMe by 2
	  cout << "size = " << size << endl;
	  size = size<<1; // This multiplies size by 2
	  cout << "size = " << size << endl;
	   
	  cin.sync();
	  cin.get();
*/
  
	  /*
	  size=1<<1; // size = 2 
	  cout << "size = " << size << endl;
	  size=1<<2; // size = 4
	  cout << "size = " << size << endl;
	  size=1<<3; // size = 8
	  cout << "size = " << size << endl;
	  
	  size=1<<(p+1); // size = 2^(p+1)
	  cout << "size = " << size << endl;
	  */

	  
	  
	  

	  
	  /*
	  double phiSum = std::accumulate(mfaParticle.begin(),mfaParticle.end(),0.0,[](double s, const Particle & p){return s + phi(p.getMass());});
	  cout << "phiSum  = " <<  phiSum  << endl;
	  cout << "tree[0]  = " <<  tree[0]  << endl;
	  */
	  //ofstream indicesDump("indices", ios::out);
	  
	  
      // Initialise events array
      for(vector<int>::iterator iterEvent = events.begin(); iterEvent != events.end(); ++iterEvent)
		*iterEvent = 0;
      
      cout << "Performing run " << run + 1 << " of " << L << endl;
      
      // Output header
      cout << "t";
      //cout << "\t\t\tscale";
      cout << "\t\t\tm0\t\t\tm1\t\t\tm2\t\t\tm3" << endl;
      
      // Iterate whilst t less than the stopping time
      while (t<tStop)
		{
		  lambda=lambda0*pow(double( N )/(N-1), alpha);
		  
		  M=lambda*N;
		  
		  // Populate momentsAllRuns array with current timestep
		  momentsAllRuns[run][0].push_back(t);
	  
		  // Let's compute the moments of the distribution
		  for(vector<double>::iterator iterMom=moments.begin();iterMom != moments.end();++iterMom)
			{	       
			  *iterMom=0e0;
			  int moment = iterMom - moments.begin();
			  
			  for(vector<Particle>::iterator iterPart = mfaParticle.begin(); iterPart != mfaParticle.end(); ++iterPart)
				{
				  *iterMom+=pow(iterPart->getMass(), moment);
				}
	      
			  *iterMom /= N;

			  // Scale the moments?
			  if(momentScaling)
				{
				  // m_k = 1/N*\sum{i=1}^n x_i^k
				  // where N is the number of computational particles (fixed)
				  // and n is the number of physical particles (can increase or decrease due to in/outflows)
				  // alpha = inflow-outflow, so n = N+alpha
				  momentScaleFactor = 1e0 + alpha/(double) N; 
	
				  *iterMom *= momentScaleFactor;
				}
		  
			  // Populate momentsAllRuns array with moments for current timestep and current runs
			  momentsAllRuns[run][moment+1].push_back(*iterMom);
			  
			}
		  

          
		  // Output data to screen and file
		  cout << t << "\t";
		  //cout << momentScaleFactor << "\t";
		  for(vector<double>::iterator iterMom=moments.begin();iterMom != moments.end();++iterMom)
			cout << *iterMom << "\t";
	  
		  cout << endl;
	  
		  outputFile << run <<  "\t" << tau
					 << "\t" << t << "\t"
					 << M << "\t" << lambda << "\t"
					 << alpha << "\t" << events[0]
					 << "\t" << events[1] << "\t"
					 << events[2] << "\t" << eventsTotal << endl;
		  

		  //Compute rates (could shove this in a subroutine)
		  //Compute collision rate
		  
		  
		  /* // Without using binary tree
			 collRate=0.0;
			 minProperty=1/0;
			 double inf=1.0/0.0;
			 // Want to do this more efficiently using a binary tree
			 for(vector<Particle>::iterator iterPart1 = mfaParticle.begin(); iterPart1 != mfaParticle.end(); ++iterPart1)
			 {
			 minProperty=min(minProperty, iterPart1->getMass());
			 for(vector<Particle>::iterator iterPart2 = mfaParticle.begin(); iterPart2 != mfaParticle.end(); ++iterPart2)
			 {
			 collRate+=k(iterPart1->getMass(),iterPart2->getMass())/iterPart2->getMass();
			 }
			 }

			 collRate*=lambda;
		  */

		  // Total rate is stored in the first element of the tree
		  collRate = tree[0];
	  
  
		  if(!coagOn)
			collRate=0e0; // Switch off coagulation for testing purposes

		  // Inflow rate
		  inRate = N / inFactor; // m_in~U[0,1] // this should always be the rate!
		  //inRate=N/2e0; // m_in~U[0,1] // this should always be the rate!
		  //inRate = 1e0/2e0;  // m_in_delta_{i1}
	  		  
	  	  // Outflow rate
		  outRate = std::accumulate(mfaParticle.begin(),mfaParticle.end(),0.0, // use = in lambda so can access outer scoped vars
									[=](double s, const Particle & p){return s + 1e0/theta(p.getMass(),inFactor);}); 
		  

		  /*
		   // non-STL
		  outRate=0e0;
		  for(vector<Particle>::iterator iterPart = mfaParticle.begin(); iterPart != mfaParticle.end(); ++iterPart)
			{
			  //outRate += 1e0/theta(iterPart->getMass(),1e0/outFactor);
			  outRate += 1e0/theta(iterPart->getMass(),outFactor);
			}
		  */  

		  /*
		  rate.push_back(collRate);
		  rate.push_back(inRate);
		  rate.push_back(outRate);
	  	  */
		  
		  rate[coagulation] = collRate;
		  rate[inflow] = inRate;
		  rate[outflow] = outRate;
		  
		  
		  // Dump rates
		  cout << "Rates (coag, inflow, outflow): ";
		  std::copy(rate.begin(), rate.end(), std::ostream_iterator<double>(std::cout, "\t"));
		  cout << endl;
		  

		
		
		// Total rate
		//rateTotal=rate[0]+rate[1]+rate[2];
		rateTotal=std::accumulate(rate.begin(), rate.end(), 0.0);

		// Compute exponentially distributed waiting time
		tau=-log(uniformRealDist(generator))/(rateTotal);

		double d = uniformRealDist(generator) * rateTotal - collRate;

		if (d < 0)
		  {
			//cout << "Coagulation event." << endl;

			// Coagulation occurs
			// Choose a collision pair

			firstParticle = generateIndex(tree, height, collRate*uniformRealDist(generator));
			secondParticle = firstParticle;
			while(secondParticle==firstParticle)
			  secondParticle = uniformIntDist(generator);

			// Coagulate particles and calculate contribution to binary tree
			deltaMass = -phi(mfaParticle[firstParticle].getMass());
			mfaParticle[firstParticle] += mfaParticle[secondParticle]; // use overloaded += operator
			deltaMass +=phi(mfaParticle[firstParticle].getMass());
		  
			// Update the tree
			updateTree(tree, height, firstParticle, deltaMass);
			//growTree(tree, height, mfaParticle);

		  		  
			/* // Acceptance-rejection method (non-binary tree version)
			   {
			   
			   firstChosen=false;
			   secondChosen=false;
			   
			   while(firstChosen==false)
			   {
			   while(secondChosen==false)
			   {
			   //firstParticle=mtrand.randInt( N-1 ) ;
			   //secondParticle=mtrand.randInt( N-1 );
			   firstParticle=uniformIntDist(generator);
			   secondParticle=uniformIntDist(generator);
			   
			   collProb=k(mfaParticle[firstParticle].getMass(),mfaParticle[secondParticle].getMass())/
			   (rateTotal*mfaParticle[secondParticle].getMass());
			   
			 if( (uniformRealDist(generator)< collProb) && (firstParticle != secondParticle))
			 {
			 firstChosen=true;
			 secondChosen=true; 
			 }
			 }		    
			 }
			*/
			
			events[coagulation]++;	  
	      
		  }  // Coagulation process complete 
		else
		  {
			d -= inRate;
			if( d < 0)
			  {
				// Particle inflow
				//cout << "Inflow event." << endl;

				// Implement this using our binary tree
				particleIndex=uniformIntDist(generator);
				deltaMass = -phi(mfaParticle[particleIndex].getMass());
				mfaParticle[particleIndex].setMass(mIn(1)); // m_in is delta(x-a), a=1 
				deltaMass +=phi(mfaParticle[particleIndex].getMass());
				
				if(deltaMass > 0)
				  {
					//cout << "Mass changed - Updating tree" << endl;
					// Update the tree
					updateTree(tree, height, particleIndex, deltaMass);
				  }
				
				
				//mfaParticle[particleIndex].setMass(mtrand()); // m_in~U[0,1]
				//mfaParticle[particleIndex].setMass(mIn()); // m_in_delta_{i1}
				//mfaParticle[particleIndex].setMass(1e0); // m_in is delta(x-a), a=1 
				//mfaParticle[particleIndex].setMass(mtrand.randInt( 1024 )); //  m_in~U[0,1] 
		  
				alpha++;
				events[inflow]++;
				
			  } // Inflow event complete 
			else
			  {
				// d -= other rates...
				//if (d < 0) ...
				//cout << "Outflow event." << endl;

				// Particle leaves the system
				firstChosen=false;
				secondChosen=false;

				// Should select according to outflow distribution
				// Using binary tree - for now, just do a uniform
				while(firstChosen == false)
				  {
					firstParticle=uniformIntDist(generator); 
					firstChosen=true;
				  }
				while(secondChosen == false)
				  {
					secondParticle=uniformIntDist(generator);;
					if(firstParticle != secondParticle)
					  secondChosen=true;
				  }


				deltaMass = -phi(mfaParticle[firstParticle].getMass());
				
				// Replace particle i with a copy of particle j (that is, particle i leaves the system)
				mfaParticle[firstParticle] = mfaParticle[secondParticle]; // This ought to do a shallow copy, copying all the member data
				deltaMass +=phi(mfaParticle[firstParticle].getMass());

				//cout << "Outflow deltaMass = " << deltaMass << endl;


				if(deltaMass > 0)
				  {
					//cout << "Mass changed - Updating tree" << endl;
					// Update the tree
					updateTree(tree, height, firstParticle, deltaMass);
					//cin.sync();
					//cin.get();
				  }
				
					
				alpha--;
				events[outflow]++;

			  } // Outflow event complete 

		  } // All particle processes complete 
		
			



	
		
		
			
		
	  

		


	  


	 
	  // Update stochastic time
	  t+=tau;
	  
	  //eventsTotal=events[0]+events[1]+events[2];
	  eventsTotal = std::accumulate(events.begin(), events.end(), 0.0);
	   
		}// End of current run	   
      
      
      
      
      /*
		cout << endl << "PSD:" << endl;
		for(vector<Particle>::iterator iterPart = mfaParticle.begin(); iterPart != mfaParticle.end(); ++iterPart)
		{
		cout  << "Particle "<< iterPart- mfaParticle.begin() 
		<< " has index " <<  iterPart->getIndex()<< " has mass " << iterPart->getMass() << endl;
		
		}
      */
	  
      // Print summary of events for current run
      cout << "Events summary (collisions, inflows, outflows):" << endl << endl;
      cout << "\t" << events[0] << "\t" << events[1] << "\t" << events[2] << endl << endl;
      cout << "Run " << run + 1 << " of "<< L << " complete!" << endl;
      
    } // End of L runs
  
  
  
  
  
  // Post-process momentsAllRuns array here
  // In order to average over all the runs we need to interpolate the points (different runs have different #s of timesteps)
  // Strategy is to assume a linear time spacing with a number of steps equal to the # of steps in the largest run
  
  cout << "Post-processing data..." << endl;
  
  int maxNoSteps = 0;
  for(vector<vector<vector<double> > >::iterator iterRun = momentsAllRuns.begin(); iterRun != momentsAllRuns.end(); ++iterRun)
    {
      if ((*iterRun)[0].size() > maxNoSteps)
		maxNoSteps = (*iterRun)[0].size();
    }
  //cout << "Max number of steps is " << maxNoSteps << endl;
  
  t = 0;
  
  int lStep, rStep;
  double t1, t2;
  double interpedMom;
  
  /* - Test interpolation routine
     cout << "interpMon(1.5, 1, 2, 2, 4) = " << interpMon(1.5, 1, 2, 2, 4) << endl;
     cout << "interpMon(1, 1, 2, 2, 4) = " << interpMon(1, 1, 2, 2, 4) << endl;
     cout << "interpMon(2, 1, 2, 2, 4) = " << interpMon(2, 1, 2, 2, 4) << endl;
     cout << "interpMon(1.75, 1, 2, 2, 4) = " << interpMon(1.75, 1, 2, 2, 4) << endl;
     return 0 ;
  */
  
  cout << "Interpolating data... ";
  
  for (int timeStep = 0; timeStep < maxNoSteps; timeStep++)
    {
      interpTimes.push_back(t);
      t += (double) tStop/maxNoSteps;
      //cout << "interpolation t = " << t << endl;
      
      // Need to loop through all runs and find which steps t is between
      
      for (int run = 0; run < L; run++)
		{
		  lStep = 0;
		  rStep = 0;
		  for (int runStep = 0; runStep < momentsAllRuns[run][0].size() - 1; runStep++)
			{
			  // We're trying to find the two steps in the current run in which t lies
			  if( (t>momentsAllRuns[run][0][runStep]) && (t<momentsAllRuns[run][0][runStep+1]) )
				{
				  lStep = runStep;
				  rStep = runStep+1;
				}
			  else if(t > momentsAllRuns[run][0][runStep+1])
				{
				  lStep = runStep+1;
				  rStep = runStep+1;
				}
			}
		  //cout << "run " << run << " first t = " << momentsAllRuns[run][0][1];
		  //cout << " # timesteps = " << momentsAllRuns[run][0].size() << endl;
	  
		  if (lStep == 0 && rStep == 0)
			{
			  cout << "Interpolation failed!" << endl;
			  cout << "lStep" << lStep << endl;
			  cout << "rStep" << rStep << endl;
			  //cout << "" << << endl;
	      
			  return 0;
			}
	  
		  t1 = momentsAllRuns[run][0][lStep];
		  t2 = momentsAllRuns[run][0][rStep];
		  
		  
		  //momentsAllRunsInterp[run][0][timeStep + 1] = t;
		  //momentsAllRunsInterp[run][0].push_back(t);
	  
		  for (int mom = 1; mom <= noMoments; mom++)
			{
			  if (lStep != rStep)
				interpedMom = interpMon(t,t1, t2, momentsAllRuns[run][mom][lStep],momentsAllRuns[run][mom][rStep]);
			  else
				interpedMom = momentsAllRuns[run][mom][rStep];
			  
			  //momentsAllRunsInterp[run][mom][timeStep + 1] = interpedMom;
			  momentsAllRunsInterp[run][mom-1].push_back(interpedMom); // -1 because we don't store time now
			}
	  
		  //cout << "t = " << t << " lies between " << momentsAllRuns[run][0][lStep] << " and " << momentsAllRuns[run][0][rStep]
		  //     << "steps " << lStep << " and " << rStep << endl;
	  
		}    
    }
  
  cout << "Done!" << endl;
  
  /*
    for(vector<double>::iterator iterT= interpTimes.begin(); iterT != interpTimes.end(); ++iterT)
	{
	cout << " interpolated time = " << *iterT << endl;
    
	}
  */
  
  cout << "Averaging moment data...";
  
  // t = 0 are the ICs, so no need to interpolate here, just output values from the first run
  // and zero variances (since ICs are identical for all runs)
  momentsFile << "0.0000000000e+00\t";
  for(int mom = 0; mom < noMoments; mom++)
    momentsFile << momentsAllRuns[0][mom+1][0] << "\t0.0000000000e+00\t";
  
  momentsFile << endl;  
  
  double momentCum;
  double momentSqrdCum;
  double momMean, momVar;
  
  
  // We now have an array momentsAllRunsInterp of interpolated moments at the *same* time, so we can loop
  // through and calculate confidence intervals
  
  for (int timeStep = 0; timeStep < maxNoSteps; timeStep++)
    {
      momentsFile << interpTimes[timeStep] << "\t";
      for (int mom = 0; mom < noMoments; mom++)
		{
		  momentCum = 0e0;
		  momentSqrdCum = 0e0;
		  
		  for (int run = 0; run < L; run++)
			{
			  momentCum += momentsAllRunsInterp[run][mom][timeStep];
			  momentSqrdCum += momentsAllRunsInterp[run][mom][timeStep]*momentsAllRunsInterp[run][mom][timeStep];
			}
		  momMean = momentCum/L;
		  momVar = momentSqrdCum/L - momMean*momMean;
		  momentsFile << momMean << "\t" << momVar << "\t";
		}
      momentsFile << endl;  
      
    }
  cout << "Done!" << endl;
  
  // Let's save the state of the random number generator
  if(saveRNGState)
    {
      //saveState(mtrand);
	  saveState(generator);
    }
  
  // Close files
  diamsFile.close();
  momentsFile.close();   
  outputFile.close(); 
  momentsDumpFile.close();
  
  cout << "Simulation complete!" << endl << endl;
  
  return 0;
  
}



  
  
  /*
  cout << "Dumping interpolated moments"<< endl;
  
  // Let's dump the interpolated moments from the first run - works
  for (int run = 0; run < L; run++)
    {
      // output initial condition
      for(int elem = 0; elem < noMoments+1; elem++)
	cout << momentsAllRuns[run][elem][0]<< "\t";
      cout << endl;
      
      
      for (int step = 0; step < momentsAllRunsInterp[run][0].size();step++) // count the # time steps in this run
	{
	  for(int elem = 0; elem < noMoments+1; elem++)
	    {
	      cout << momentsAllRunsInterp[run][elem][step] << "\t";
	    }
	  cout << endl;
	}
    }
  
  */
  



  

  /*
for(vector<vector<vector<double> > >::iterator iterRun = momentsAllRuns.begin(); iterRun != momentsAllRuns.end(); ++iterRun)
    {
      cout << "iterRun = "  << endl;
      for(vector<double>::iterator iterStep=(*iterRun)[0].begin(); iterStep != (*iterRun)[0].end(); ++iterStep)
	{
	  cout << "timeStep  = " << *timeStep;

	}
    }
  */

  
  /*
  // Let's dump the moments from the first run - works
  for (int run = 0; run < 1; run++)
    {
      for (int step = 0; step < momentsAllRuns[run][0].size();step++) // count the # time steps in this run
	{
	  for(int elem = 0; elem < noMoments+1; elem++)
	    {
	      momentsFile << momentsAllRuns[run][elem][step] << "\t";
	    }
	  momentsFile << endl;
	}
    }
  */
      

  

  /*
  vector<vector<vector<double> > >::iterator iterRun = momentsAllRuns.begin();
  cout << "noElems = " << iterRun->size() << endl;
  cout << "steps in first elem = " << (*iterRun)[0].size() << endl;

  for(int elem = 0; elem < iterRun->size(); elem++)
    {
      for (int step = 0; step < (*iterRun)[elem].size(); step++)
	{
	  cout << (*iterRun)[elem][step] << "\t" <<endl;
	}
      cout << endl;
      
      //
      
      
    }

  */


  /*
  // Let's dump the moments from the first run
  vector<vector<vector<double> > >::iterator iterRun = momentsAllRuns.begin();
  for(vector<vector<double> >::iterator iterElem = iterRun->begin(); iterElem != iterRun->end(); ++iterElem)
    {
      for(vector<double>::iterator iterLine = iterElem->begin(); iterLine != iterElem->end(); ++iterLine)
	{
	  momentsFile << *iterLine << "\t";
	}
      momentsFile << endl;
    }
  */

  /*
  // Dump all info - to screen
  for(vector<vector<vector<double> > >::iterator iterRun = momentsAllRuns.begin(); iterRun != momentsAllRuns.end(); ++iterRun)
    {
      cout << "Run: " << iterRun - momentsAllRuns.begin() + 1 << endl;
      for(vector<vector<double> >::iterator iterElem = iterRun->begin(); iterElem != iterRun->end(); ++iterElem)
	{
	  cout << "Element: " << iterElem -  iterRun->begin() << "\t";
	  for(vector<double>::iterator iterLine = iterElem->begin(); iterLine != iterElem->end(); ++iterLine)
	    {
	      cout << "Timestep = " << iterLine -  iterElem->begin() << "value = " << *iterLine << "\t";
	    }
	  cout << endl;
	}

    }

  */



