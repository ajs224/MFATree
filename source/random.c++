#include<iostream>
#include<fstream>
#include<random> // Need to compile with g++ -std=gnu++0x
#include<ostream>
#include"random.h"

void saveState(std::mt19937 & generator)
{
  std::cout << "Saving state "<< std::endl;
  std::ofstream state("state.dat", std::ios::out);
  state << generator;
  state.close();
}

/*
std::mt19937 loadState()
{
  std::cout << "Loading state "<< std::endl;
  std::ifstream state("state.dat", std::ios::in);
  std::mt19937 generator;
  state >> generator;
  state.close();
  return generator;
}
*/

void loadState(std::mt19937 & generator)
{
  std::cout << "Loading state "<< std::endl;
  std::ifstream state("state.dat", std::ios::in);
  std::mt19937 restoredGen;
  state >> restoredGen;
  state.close();
  generator = restoredGen;
}
