// Compile with `c++ -std=c++17 -Isignalgp-lite/include native.cpp`

#include <iostream>

#include "ConfigSetup.h"
#include "World.h"

/**
 * Sets up configuration manager.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param config Pointer to configuration manager.
 */
void SetUpConfig(int argc, char *argv[], MyConfigType* config) {
  // check if config settings file was read successfully
  config->Read("MySettings.cfg");
  bool success = config->Read("MySettings.cfg");
  if(!success) config->Write("MySettings.cfg");

  // if relevant, override config settings with command line arguments
  auto args = emp::cl::ArgManager(argc, argv);
  if (args.ProcessConfigOptions(*config, std::cout, "MySettings.cfg") == false) {
    std::cerr << "There was a problem in processing the options file." << std::endl;
    exit(1);
  }
  if (args.TestUnknown() == false) {
    std::cerr << "Leftover args no good." << std::endl;
    exit(1);
  }
}

// This is the main function for the NATIVE version of this project.
int main(int argc, char *argv[]) {

  // sets up config
  MyConfigType config;
  SetUpConfig(argc, argv, &config);

  // sets up main random instance for the world and the SignalGP-Lite emp::Random instance
  emp::Random random(config.SEED());
  OrgWorld world(random);
  sgpl::tlrand.Get().ResetSeed(config.SEED());

  // sets up world
  world.SetPopStruct_Grid(20, 20);
  world.Resize(20 ,20);
  world.SetupOrgFile("seed_" + std::to_string(config.SEED()) + "_num_start_"  + std::to_string(config.NUM_START()) + ".data").SetTimingRepeat(100);


  // creates 5 new organisms and randomly places them in the world
  for (int i = 0; i< config.NUM_START(); i++) {
    Organism* new_org = new Organism(&world, world.MakeCellID());
    world.Inject(*new_org);
  }

  // runs updates to see organisms evolve
  for (int update = 0; update < 50000; update++) {
    world.Update();
  }
}