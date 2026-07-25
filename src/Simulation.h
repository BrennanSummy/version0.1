#include <iostream>
#include <memory>
#include <random>

class Simulation
{
    public:
        Simulation();
        ~Simulation();
        void step();

    private:
        /* The width and height should match the board dimensions. The char matrix
            dimensions must be known at compile time, which is why I have it set up like this. */
        int m_width=10;
        int m_height=15;
        /* char matrix used for 'A' vs 'B' vs 'C' domains. chars use 1 byte, which is the minimum
            possible per element. */
        char board[10][15];

        /* static array that stores the number of A's B's and C's neighboring the current coordinate
            0 <-> A, 1 <-> B, 2 <-> C */
        short neighborVals[3];
        // an accompanying array that stores probabilities of switching to each domain
        float probabilities[3];

        // random number device and generator and uniform distribution (used with the <random> library)
        std::random_device rand_device;
        std::mt19937 rand_generator;
        std::uniform_real_distribution<float> uniform;

        // Private methods
        // Initialize the board to some arbitrary state
        void initBoard();
        /* Updates the state (domain) of the board element at the given coordinates
           (i is the first index of the board array)*/
        void updateElement(int i,int j);
        // Checks the neighboring indices and updates the neighborVals array accordingly
        void updateNeighborVals(int i, int j);
        // Uses the neighborVals array to update the probabilities array
        void updateProbabilities();
        // Uses the probabilties array and the uniform distribution to pick a domain for the current element
        char roll();
};