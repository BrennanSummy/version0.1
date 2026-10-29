#pragma once
#include <vector>
#include <string>

/* The width and height of the board. The char matrix
    dimensions must be known at compile time. */
inline constexpr size_t bWidth  = 120;
inline constexpr size_t bHeight = 120;
inline constexpr size_t bBWidth  = 2*bWidth -1;
inline constexpr size_t bBHeight = 2*bHeight-1;

inline constexpr int min_updating_i = 2;
inline constexpr int min_updating_j = 2;
inline constexpr int max_updating_i = bWidth  - 3;
inline constexpr int max_updating_j = bHeight - 3;

const std::string dir = "/home/b_lin/Documents/Coding/barnard-cobden-labs/ads_sim_proj/version0.1/out/";

const bool DEVICE = 0; // 0 for DESKTOP, 1 for LAPTOP

// Number of pixels to randomly update per step
const int stepSizeInPixelUpdates = 7500;

// Number of steps to do between boundary length scans / information gathering
const int scanBoundsPeriodInSteps = 1000;

// Number of reads to do between screenshots
const int grabScreenPeriodInSteps = 5000;

// Number of reads to take per file/parameter set
const int readsPerFile = 20001;


// Names of parameters to be looped through for data collection. This vector is referenced in the Textbox class
inline const std::vector<std::string> paramNames = {"iNN","i2NN","cNN","c2NN","cC"};

const double start_kT = 5;
const double end_kT   = 0.1;

const int kT_rampLength = 1000;