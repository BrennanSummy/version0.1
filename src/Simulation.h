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
#include <algorithm>
#include "BoundaryCounter.h"
#include "constants.h"
#include "structs.h"


class Simulation
{
   public:
        // Constructor
        Simulation(double kT, double tensionFactor, double intraLayerNearestNeighbor, double crossLayerNearestNeighbor, double crossLayerBoundaryFactor);
        // Prints the board to the console.
        void printBoard();
        // Prints the board with the boundary to the console.
        void printBoardWithBoundary();
        // The step function calculates the board state for the next timestep.
        void randomStep();

        //////////////////// Update Functions ////////////////////
        // This updates the value of kT to the given value.
        void updateTemp(float value);

        // This updates the value of tensionFactor to the given value.
        void updateTension(float value);

        // This updates the value of nearestNeighborFactor to the given value.
        void updateNearestNeighbor(float value);

        // This updates the value of m_crossLayerNearestNeighborFactor to the given value.
        void updateCrossLayerNearestNeighbor(float value);

        // This updates the value of m_crossLayerBoundaryFactor to the given value.
        void updateCrossLayerBoundaryInterference(float value);
        //////////////////// Update Functions ////////////////////

        /* char matrix used for 'A' vs 'B' vs 'C' domains. chars use 1 byte, which is the minimum
            possible per element. */
        char m_board[2][height][width];

        /* char matrix used for keeping track of boundaries*/
        char m_boundaryBoard[2][bBHeight][bBWidth];

        /* char matrix used for debugging boundaries*/
        char m_debugBoundaryBoard[2][bBHeight][bBWidth];

        int m_width   = (int)width;
        int m_height  = (int)height;
        int m_bBWidth = (int)bBWidth;
        int m_bBHeight= (int)bBHeight;
    private:

        // Temperature for probability calculation
        double m_kT;

        // Line tension factor for probability calculation
        double m_tensionFactor;

        // Intra-layer nearest neighbor factor for probability calculation
        double m_nearestNeighborFactor;
        
        // Inter-layer nearest neighbor factor for probability calculation
        double m_crossLayerNearestNeighborFactor;

        // Inter-layer boundary interference factor for probability calculation
        double m_crossLayerBoundaryFactor;

        // Unordered map to associate 'A' with D or E, etc
        const std::unordered_map<char, std::array<char,2>> domainToBoundaryMap = {{'A',{'D','E'}}, {'B', {'D', 'F'}}, {'C', {'E', 'F'}}};

        // Unordered map to associate 'A' with 0, etc
        const std::unordered_map<char, int> domainToIntMap = {{'A',0}, {'B', 1}, {'C', 2}};
        // Backwards version of that
        const char m_X[3] = {'A', 'B', 'C'};

        /* static array that stores the number of A's B's and C's neighboring the current coordinate
            0 <-> A, 1 <-> B, 2 <-> C */
        short m_inLayerNeighborCounts[3];

        /* static array that stores the number of A's B's and C's neighboring the current coordinate's complement (on the other side of the board)
            0 <-> A, 1 <-> B, 2 <-> C */
        short m_crossLayerNeighborCounts[3];

        // A boolean that stores whether or not the proposed element value matches the complement or not
        bool m_matchingComplement[3];


        /* static array that stores the positions of A's B's and C's neighboring the current coordinate
            0 <-> A, 1 <-> B, 2 <-> C , index 0 means upper left neighbor, indices go cw to 5 at the left. */
        short m_neighborVals[6];

        // Counter that tracks how many paths have been explored since starting to update an element
        int m_numPathsExplored;

        /* Array that stores the total lengths of the boundaries associated with A,B, or C.
            0 <-> A, 1 <-> B, 2 <-> C */
        int m_prospectiveBoundaryLengths[3]={0,0,0};

        // Array that, for each prospective boundary, keeps track of the number of overlaps with boundaries on the other side
        int m_prospectiveBoundaryOverlapCounts[3]={0,0,0};

        std::vector<std::array<int,2>> m_visitedEdgePositions1;
        // Map which takes in the prospective char for an element's step and gives you a vector of coordinates for the edges visited
        std::unordered_map<char, std::vector<std::array<int,2>>> m_visitedEdgePositions;

        std::vector<std::array<int,3>> m_remainingInitEdges;

        std::array<bool,6> m_startingBoundaries;

        // an accompanying array that stores probabilities of switching to each domain
        double m_probabilities[3];
        // Energy terms for A B and C
        double m_nearestEnergy[3];
        double m_tensionEnergy[3];
        double m_cLNearestEnergy[3];
        double m_cLBoundaryInterferenceEnergy[3];
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
        const int m_stepSize = 2000; //floor(width*height/10); // about 500 updates per frame seems to be realistic for 60fps

        // This keeps track of how much tension vs NN is determining outcomes
        double tensionEnergyTypeCounter;
        double nNEnergyTypeCounter;

        // Boundary Counter object
        BoundaryCounter m_counter;

        // Private methods
        // Initialize the board to some arbitrary state
        void initBoard();
        // For initializing the boundary board by scanning the board
        void updateBoundaryBoardViaScan();
        /* Updates the state of the board element at the given coordinates
           (i is the first index of the board array).*/
        char updateBoardElement(int i,int j, bool top);

        void setDebugBoardToBoundaryBoard();
        void setDebugBoardElement(int i, int j, bool top, char value);
        void printDebugBoard();
        // Return the number of edges on the bottom or top board
        int  countEdges(bool top);
        // Return the number of dissimilar neighbors across the specified board (count dissimilar neighbors for each element and divide by 2)
        int  countDissimNeighbors(bool top);
        // Return the energy of the current state
        int  getConfigurationEnergy();

        // Update the elements of the boundary board relevant to board coords i,j
        void updateBoundaryBoardElements(int boardi, int boardj, bool top);
        // Modifies boundaryBoard as if the element at i,j was instead value. Used for prospective boundary length finding
        void pointModifyBoundaryBoard(char prospectiveValue, int i, int j, bool top);
        // Gets the correct neighboring edge coordinates given current board coords and neighbor index and side
        void getNeighborEdgeCoordsFromIndex(int* i, int* j, int neighborIndex, bool top);
        /* Check which type of boundary is at a certain neighbor index, where the index is 0 at the top leftmost neighbor
            with top having priority over leftmost, and goes clockwise from there with a max of 3.
            Note that edgePosition is a length two int array that has j in the first and i in the second element*/
        void getAdjacentEdgePosition(int* edgePosition, int neighborIndex, bool top);
        // Recursive function that explores contiguous edges. Used for boundary length finding
        void exploreLine(std::vector<std::array<int,3>>& remainingInitEdges, int* startingEdgePosition, int* position, bool direction,
                         char boundaryType, bool* looptr);

        // Same idea as above, but this one returns a vector of adjacent board elements with the given value.
        void exploreLineWithElementTracking(int* length, std::vector<std::array<int,2>>& passedElements, std::vector<std::array<int,3>>& remainingInitEdges,
                                            int* startingEdgePosition, int* position, bool direction, char prospectiveValue, bool* looptr);

        void exploreLineOLD(int* startingEdgePosition, int* position, bool direction, char boundaryType, bool* looptr);
        // Function that takes edge position and the neighbor chosen from that position, and outputs a boolean for direction. 0 means first two neighbors, 1 means last two
        bool getEdgeFindingDirection(int* position, int chosenNeighborIndex);
        // For a given edge position, returns the two adjacent board element positions
        std::vector<std::array<int,2>> getAdjElementPositionsFromEdgePos(int* edgePosition);
        // For a given element position and a bool copy of the element board, checks off visited elements and returns
        // the boundary length associated with the element.
        int getBoundaryLengthAtElement(bool (*remainingElements)[height][width], int i, int j, bool top);
        // Gets a vector containing the length of every boundary on the board
        std::vector<int> getBoundaryLengths();
        // Get a boundary board-shaped char array detailing the number of non-contiguous boundaries
        // Check if an edge position is valid. Assumes position has j as element 0, i as element 1
        bool edgePositionCheck(int* position);
        // Check if an edge position is valid
        bool elementPositionCheck(int i,int j);
        // Checks the neighboring indices and updates the neighborVals array accordingly
        void updateNeighborVals(int i, int j, bool top);
        // Clears m_inLayerNeighborCounts
        void clearNeighborVals();
        // Checks the complement of the given value and updates m_matchingComplement accordingly (e.g. true in element 0 means that the complement matches A)
        void updateMatchingComplement(int i, int j, bool top);
        // Uses the neighborVals array to update the probabilities array
        void updateProbabilities();
        // Uses the boundary board to calculate the prospective lengths of boundaries
        void updateProspectiveBoundaryLengthsOLD(char prospectiveValue, int i, int j, bool top);
        void updateProspectiveBoundaryLengths(char prospectiveValue, elementCoords element, bool top);
        // Uses the probabilties array and the uniform distribution to pick a domain for the current element
        char roll();
};