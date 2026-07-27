#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <random>

const int width = 4;
const int height = 7;
struct ScreenPosition
{
    const double x;
    const double y;
};

class Simulation
{
    public:
        Simulation();
        //~Simulation();
        void printBoard();
        void step();
        /* char matrix used for 'A' vs 'B' vs 'C' domains. chars use 1 byte, which is the minimum
            possible per element. */
        char m_board[height][width];

        //std::vector<ScreenPosition> aVertices;
        //std::vector<ScreenPosition> bVertices;
        //std::vector<ScreenPosition> cVertices;

        /* The width and height should match the board dimensions. The char matrix
            dimensions must be known at compile time, which is why I have it set up like this. */
        int m_width=width;
        int m_height=height;


    private:
        // Temperature for probability calculation
        double m_kT=1;
        /* A buffer is used to hold the new element values to avoid influencing dynamics
            with arbitrary sweeping of element updates */
        char m_board_buffer[height][width];

        /* static array that stores the number of A's B's and C's neighboring the current coordinate
            0 <-> A, 1 <-> B, 2 <-> C */
        short m_neighborVals[3];
        // an accompanying array that stores probabilities of switching to each domain
        double m_probabilities[3];

        // random number device and generator and uniform distribution (used with the <random> library)
        std::random_device rand_device;
        std::mt19937 rand_generator;
        std::uniform_real_distribution<double> uniform;

        // Private methods
        // Initialize the board to some arbitrary state
        void initBoard();
        /* Updates the state of the board element buffer at the given coordinates
           (i is the first index of the board array).*/
        void updateBufferElement(int i,int j);
        // Writes the buffer to the state (done at the end of an update step)
        void updateBoardAndVertsWithBuffer();
        // Checks the neighboring indices and updates the neighborVals array accordingly
        void updateNeighborVals(int i, int j);
        // Uses the neighborVals array to update the probabilities array
        void updateProbabilities();
        // Uses the probabilties array and the uniform distribution to pick a domain for the current element
        char roll();
        //// Update the correct vector of positions
        //void updateVerticeVectors(int i, int j, char value);

        //double iToScreenCoord(double i, double j);
        //double jToScreenCoord(double j);
};