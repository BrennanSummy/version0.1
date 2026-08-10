#pragma once

// chrono library is only used here for calculating the execution time for simulation stepping.
#include <chrono>
#include <unordered_map>
#include <array>
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <random>


const int width = 60;
const int height = 40;
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
        void randomStep();

        // This updates the value of kT to the given value.
        void updateTemp(float value);

        // This updates the value of tensionFactor to the given value.
        void updateTension(float value);

        // This updates the value of nearestNeighborFactor to the given value.
        void updateNearestNeighbor(float value);

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
        double m_tensionFactor=1;

        // Line tension factor for probability calculation
        double m_nearestNeighborFactor=1;

        // Unordered map to associate 'A' with D or E, etc
        const std::unordered_map<char, std::array<char,2>> domainToBoundaryMap = {{'A',{'D','E'}}, {'B', {'D', 'F'}}, {'C', {'E', 'F'}}};

        // Unordered map to associate 'A' with 0, etc
        const std::unordered_map<char, int> domainToIntMap = {{'A',0}, {'B', 1}, {'C', 2}};
        // Backwards version of that
        const char m_X[3] = {'A', 'B', 'C'};

        /* static array that stores the number of A's B's and C's neighboring the current coordinate
            0 <-> A, 1 <-> B, 2 <-> C */
        short m_neighborVals[3];

        // Counter that tracks how many paths have been explored since starting to update an element
        int m_numPathsExplored;

        /* Array that stores the total lengths of the boundaries associated with A,B, or C.
            0 <-> A, 1 <-> B, 2 <-> C */
        int m_prospectiveBoundaryLengths[3]={0,0,0};

        std::vector<std::array<int,2>> m_visitedEdgePositions1;
        // Map which takes in the prospective char for an element's step and gives you a vector of coordinates for the edges visited
        std::unordered_map<char, std::vector<std::array<int,2>>> m_visitedEdgePositions;

        // an accompanying array that stores probabilities of switching to each domain
        double m_probabilities[3];
        // plan b array which stores the energies (at low kT the probabilities can vanish)
        double m_energies[3];

        // random number device and generator and uniform distribution (used with the <random> library)
        std::random_device rand_device;
        std::mt19937 rand_generator;
        std::uniform_real_distribution<double> uniform_real;
        std::uniform_int_distribution<int> uniform_3;
        std::uniform_int_distribution<int> uniform_2;
        std::uniform_int_distribution<int> uniform_int_i;
        std::uniform_int_distribution<int> uniform_int_j;

        // time keeping variables to help with development
        int m_stepCounter = 0;
        double m_stepDurations[1000];

        // (Arbitrary once random updates were added) Number of elements updated per step.
        int m_stepSize = floor(width*height/5);

        // Private methods
        // Initialize the board to some arbitrary state
        void initBoard();
        // For initializing the boundary board by scanning the board
        void updateBoundaryBoardViaScan();
        /* Updates the state of the board element at the given coordinates
           (i is the first index of the board array).*/
        void updateBoardElement(int i,int j);

        void incrementExploredPaths();
        void printExploredPaths();

        // Update the elements of the boundary board relevant to board coords i,j
        void updateBoundaryBoardElements(int boardi, int boardj);
        // Modifies boundaryBoard as if the element at i,j was instead value. Used for prospective boundary length finding
        void pointModifyBoundaryBoard(char prospectiveValue, int i, int j);
        /* Check which type of boundary is at a certain neighbor index, where the index is 0 at the top leftmost neighbor
            with top having priority over leftmost, and goes clockwise from there with a max of 3.
            Note that edgePosition is a length two int array that has j in the first and i in the second element*/
        void getAdjacentEdgePosition(int* edgePosition, int neighborIndex);
        // Recursive function that explores contiguous edges. Used for boundary length finding
        void exploreLine(int* startingEdgePosition, int* position, bool direction, char boundaryType);
        // Function that takes edge position and the neighbor chosen from that position, and outputs a boolean for direction
        bool getEdgeFindingDirection(int* position, int chosenNeighborIndex);
        // Check if an edge position is valid
        bool edgePositionCheck(int* position);
        // Checks the neighboring indices and updates the neighborVals array accordingly
        void updateNeighborVals(int i, int j);
        // Uses the neighborVals array to update the probabilities array
        void updateProbabilities();
        // Uses the boundary board to calculate the prospective lengths of boundaries
        void updateProspectiveBoundaryLengths(char prospectiveValue, int i, int j);
        // Uses the probabilties array and the uniform distribution to pick a domain for the current element
        char roll();
};