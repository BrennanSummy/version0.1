#pragma once
#include <vector>
#include <string>

/* The width and height of the board. The char matrix
    dimensions must be known at compile time. */
//inline constexpr size_t bWidth  = 23;
//inline constexpr size_t bHeight = 21;
inline constexpr int bWidth  = 1020;
inline constexpr int bHeight = 1020;
inline constexpr size_t bArea = bWidth * bHeight;
inline constexpr size_t bBWidth  = 2*bWidth -1;
inline constexpr size_t bBHeight = 2*bHeight-1;

inline constexpr int noUpdateOffset = 2;
inline constexpr int min_updating_i = noUpdateOffset;
inline constexpr int min_updating_j = noUpdateOffset;
inline constexpr int max_updating_i = bWidth  - 1 - noUpdateOffset;
inline constexpr int max_updating_j = bHeight - 1 - noUpdateOffset;

const std::string dir = "/home/b_lin/Documents/Coding/barnard-cobden-labs/ads_sim_proj/version0.1/out/";

const bool DEVICE = 0; // 0 for DESKTOP, 1 for LAPTOP

// Number of pixels to randomly update per step
const int stepSizeInPixelUpdates = 8*7500;
const int stepSizeInCheckerboardUpdates = 9;

// Number of steps to do between boundary length scans / information gathering
const int scanBoundsPeriodInSteps = 1000;

// Number of reads to do between screenshots
const int grabScreenPeriodInSteps = 2000;

// Number of reads to take per file/parameter set
const int readsPerFile = 30001;


// Names of parameters to be looped through for data collection. This vector is referenced in the Textbox class
inline const std::vector<std::string> paramNames = {"iNN","i2NN","cNN","c2NN","cC"};

const double start_kT = 5;
const double end_kT   = 0.1;

const int kT_rampLength = 9000;