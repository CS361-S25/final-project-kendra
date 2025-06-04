// Compile with `c++ -std=c++17 -Isignalgp-lite/include native.cpp`

#include <iostream>

#include "World.h"

// This is the main function for the NATIVE version of this project.
int main(int argc, char *argv[]) {

  // sets up main random instance for the world and the SignalGP-Lite emp::Random instance
  emp::Random random(2);
  OrgWorld world(random);
  sgpl::tlrand.Get().ResetSeed(2);

  // resizes world and population
  world.Resize(20 ,20);
  world.SetPopStruct_Grid(20, 20);

  // creates 5 new organisms and randomly places them in the world
  for (int i = 0; i< 5; i++) {
    Organism* new_org = new Organism(&world, world.MakeCellID());
    world.Inject(*new_org);
  }

  // runs updates to see organisms evolve
  for (int update = 0; update < 10000000; update++) {
    world.Update();
  }
}