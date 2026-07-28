#include "Simulation.h"

Simulation::Simulation()
:rand_device(), rand_generator(rand_device()), uniform(0,1)
{
    initBoard();
    //std::cout << "board initialized" << std::endl;
}

void Simulation::initBoard()
{
    // Easy ~half A quarter B quarter C default initialization
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            if(j >= (int) (m_height / 2))
            {
                if(i>=(int) (m_width / 2))
                {
                m_board[j][i] = 'C';
                }
                else
                {
                m_board[j][i] = 'B';
                }
            }
            else
            {
                m_board[j][i] = 'A';
            }
        }
    } 
    
}

void Simulation::printBoard()
{
    std::string space = " ";
    space.append(" ");
    for (int j = m_height-1; j >= 0; j--)
    {
        std::string space = "";
        for (int spaces = j; spaces >= 0; spaces--)
        {
            space.append(" ");
        }
        std::cout << space;
        for (int i = 0; i < m_width; i++)
        {
            std::cout <<m_board[j][i] <<  " ";
        }
        std::cout << "\n";
    } 
    
}

void Simulation::updateBufferElement(int i, int j)
{
    updateNeighborVals(i,j);
    updateProbabilities();
    m_board_buffer[j][i] = roll();
}

void Simulation::updateNeighborVals(int i, int j)
{
    // NOTE THAT A RIGHT-TILTED PARALLELOGRAM IS ASSUMED. 'i' INDEXES THE HORIZONTAL, 'j' THE VERTICAL
    // List the relevant neighbor indices, starting with the upper left and going clockwise
    int indices[6][2] = {   {j+1,i-1},
                            {j+1,i},
                            {j,i+1},
                            {j-1,i+1},
                            {j-1,i},
                            {j,i-1}};
    
    /* Loop through all of the neighbors:
        - if(coordinate is valid)
            - check char value of board[coordinate], return A, B, or C
            - add 1 to the count of A, B, or C respectively
        - else pass
    */
    for (int neighbor = 0; neighbor < 6; neighbor++)
    {
        // Test the validity of the neighbor indices (this deals with edges and corners)
        bool coordIsValid = (indices[neighbor][0] >= 0)        && (indices[neighbor][1] >= 0)         &&
                            (indices[neighbor][0] < m_height)  && (indices[neighbor][1] < m_width);
        if(coordIsValid)
        {
            // Check the state of the board from the previous timestep to get the value of this element
            char value = m_board[indices[neighbor][0]][indices[neighbor][1]];
            // Increment the appropriate counter in m_neighborVals
            switch (value)
            {
                case 'A':
                    m_neighborVals[0]++;
                    break;
                case 'B':
                    m_neighborVals[1]++;
                    break;
                case 'C':
                    m_neighborVals[2]++;
                    break;
            }
            
        }

    }

}

void Simulation::updateBoardWithBuffer()
{
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            char val        = m_board_buffer[j][i];
            m_board[j][i]   = val;
        }
    }
    //std::cout << "Board has been updated" << std::endl;
}


void Simulation::updateProbabilities()
{
    // Naive energy function: Energy of X is total number of neighbors - number of X neighbors
    // e.g. E(A) = A+B+C - A = B+C where A is the number of neighbors with value 'A'
    double A = m_neighborVals[0];
    double B = m_neighborVals[1];
    double C = m_neighborVals[2];


    double boltzmann_A = std::exp(-(B+C)/m_kT);
    double boltzmann_B = std::exp(-(A+C)/m_kT);
    double boltzmann_C = std::exp(-(A+B)/m_kT);

    double Z = boltzmann_A + boltzmann_B + boltzmann_C;

    m_probabilities[0] = boltzmann_A / Z;
    m_probabilities[1] = boltzmann_B / Z;
    m_probabilities[2] = boltzmann_C / Z;
    //std::cout << "P_A " << m_probabilities[0] << std::endl;

    // Reset neighborVals
    for (int i=0;i<3;i++){m_neighborVals[i]=0;}
}

char Simulation::roll()
{
    // roll a number between 0 and 1
    double roll = uniform(rand_generator);
    //std::cout << "roll: " << roll << std::endl;
    char val;
    // if that number is between 0 and P(A), return 'A'
    if (roll<m_probabilities[0])
    {
        val = 'A';
    }
    else if ((m_probabilities[0] <= roll) && (roll < m_probabilities[0] + m_probabilities[1]))
    {
        val = 'B';
    }
    else if (m_probabilities[0] + m_probabilities[1] <= roll)
    {
        val = 'C';
    }
    return val;
}

void Simulation::step()
{
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            updateBufferElement(i,j);
        }
    } 
    //std::cout << "Buffer fully updated" << std::endl;
    updateBoardWithBuffer();
    //printBoard();
}

void Simulation::updateTemp(float value)
{
    m_kT = value;
}