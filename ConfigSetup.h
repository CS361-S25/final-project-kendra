#include "emp/config/ArgManager.hpp"

/**
 * Configuration manager class used for the random seed and the number of starting organisms.
 */
EMP_BUILD_CONFIG(MyConfigType, 
VALUE(SEED, int, 1, "What value should the random seed be?"), 
VALUE(NUM_START, int, 5, "how many to start?")
)