#pragma once

// chrono library is only used here for calculating the execution time for simulation stepping.
#include <chrono>
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <random>

const int width = 10;
const int height = 10;
struct ScreenPosition
{
    const double x;
    const double y;
};

class Simulation
{
    public:
        // Constructor
        Simulation();
        // Prints the board to the console.
        void printBoard();
        // Prints the board with the boundary to the console.
        void printBoardWithBoundary();
        // The step function calculates the board state for the next timestep.
        void step();

        // This updates the value of kT to the given value.
        void updateTemp(float value);

        /* char matrix used for 'A' vs 'B' vs 'C' domains. chars use 1 byte, which is the minimum
            possible per element. */
        char m_board[height][width];

        /* char matrix used for keeping track of boundaries*/
        char m_boundaryBoard[2*height-1][2*width-1];
        
        /* The width and height of the board. The char matrix
            dimensions must be known at compile time. */
        int m_width=width;
        int m_height=height;

    private:
        // Temperature for probability calculation
        double m_kT=1;

        // Line tension factor for probability calculation
        double m_tensionFactor;

        /* A buffer is used to hold the new element values to avoid influencing dynamics
            with arbitrary sweeping of element updates */
        char m_boardBuffer[height][width];

        /* char matrix used for keeping track of boundaries*/
        char m_boundaryBoardBuffer[2*height-1][2*width-1];

        /* static array that stores the number of A's B's and C's neighboring the current coordinate
            0 <-> A, 1 <-> B, 2 <-> C */
        short m_neighborVals[3];

        /* Array that stores the total lengths of the boundaries associated with A,B, or C.
            0 <-> A, 1 <-> B, 2 <-> C */
        short m_prospectiveBoundaryLengths[3];

        // an accompanying array that stores probabilities of switching to each domain
        double m_probabilities[3];

        // random number device and generator and uniform distribution (used with the <random> library)
        std::random_device rand_device;
        std::mt19937 rand_generator;
        std::uniform_real_distribution<double> uniform;

        // time keeping variables to help with development
        int m_stepCounter = 0;
        double m_stepDurations[1000];

        // Private methods
        // Initialize the board to some arbitrary state
        void initBoard();
        // Initialize the boundary board by scanning the entire m_board.
        void updateBoundaryBoardBufferViaScan();
        /* Updates the state of the board element buffer at the given coordinates
           (i is the first index of the board array).*/
        void updateBufferElement(int i,int j);
        // Update the elements of the boundary board relevant to board coords i,j
        void updateBoundaryBoardBufferElements(int boardi, int boardj);
        // Writes the buffer to the state (done at the end of an update step)
        void updateBoardWithBuffer();
        // Writes the boundary board buffer to the boundary board state
        void updateBoundaryBoardWithBuffer();
        // Checks the neighboring indices and updates the neighborVals array accordingly
        void updateNeighborVals(int i, int j);
        // Uses the neighborVals array to update the probabilities array
        void updateProbabilities();
        // Uses the boundary board to calculate the prospective lengths of boundaries
        void updateProspectiveBoundaryLengths(char prospectiveValue, int i, int j);
        // Uses the probabilties array and the uniform distribution to pick a domain for the current element
        char roll();
};