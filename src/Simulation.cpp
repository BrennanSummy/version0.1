#include "Simulation.h"

Simulation::Simulation()
:rand_device(), rand_generator(rand_device()), uniform(0,1)
{
    initBoard();
    updateBoundaryBoardBufferViaScan();
    updateBoundaryBoardWithBuffer();
    printBoardWithBoundary();
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

void Simulation::updateBoundaryBoardBufferViaScan()
{
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            // Check the state of the board
            char ijValue   = m_board[j][i];

            // Look to the upper left, upper right, and right for different domains
            int indices[3][2] = {   {j+1,i-1},
                                    {j+1,i},
                                    {j,i+1},
            };

            for (int neighbor = 0; neighbor < 3; neighbor++)
            {
                // Test the validity of the neighbor indices (this deals with edges and corners)
                bool coordIsValid = (indices[neighbor][0] >= 0)        && (indices[neighbor][1] >= 0)         &&
                                    (indices[neighbor][0] < m_height)  && (indices[neighbor][1] < m_width);
                if(coordIsValid)
                {
                    // Check the state of the relevant neighbors
                    int neighbor_j = indices[neighbor][0];
                    int neighbor_i = indices[neighbor][1];
                    char neighborValue = m_board[neighbor_j][neighbor_i];

                    char boundaryType = '.';
                    if( (neighborValue=='A' && ijValue=='B') || (neighborValue=='B' && ijValue=='A') )
                    {
                        boundaryType = 'D';
                    }
                    else if( (neighborValue=='A' && ijValue=='C') || (neighborValue=='C' && ijValue=='A') )
                    {
                        boundaryType = 'E';
                    }
                    else if( (neighborValue=='B' && ijValue=='C') || (neighborValue=='C' && ijValue=='B') )
                    {
                        boundaryType = 'F';
                    }
                    /* Update the boundary array using the correct indices as judged by the
                        neighbor loop index*/
                    int boundi=2*i;
                    int boundj=2*j;
                    switch(neighbor)
                    {
                        case 0:
                        boundi += -1;
                        boundj +=  1;
                        break;
                        case 1:
                        boundj +=  1;
                        break;
                        case 2:
                        boundi +=  1;
                        break;
                    }
                    m_boundaryBoardBuffer[boundj][boundi] = boundaryType;
                    
                }

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

void Simulation::printBoardWithBoundary()
{
    std::string space = " ";
    space.append(" ");
    for (int doublej = 2*m_height-2; doublej >= 0; doublej--)
    {
        std::string space = "";
        for (int spaces = doublej; spaces >= 0; spaces--)
        {
            space.append(" ");
        }

        std::cout << space;

        if(doublej%2==0)
        {
            for (int i = 0; i < m_width; i++)
            {
                std::cout <<m_board[doublej/2][i] <<  " ";

                if(i<(m_width - 1))
                {
                    std::cout << m_boundaryBoard[doublej][2*i+1] <<  " ";
                }
            }
            std::cout << "\n";
        }
        else
        {
            for (int doublei = 0; doublei < 2*m_width - 1; doublei++)
            {
                std::cout << m_boundaryBoard[doublej][doublei] <<  " ";
            }
            std::cout << "\n";
        }
    } 
}

void Simulation::updateBufferElement(int i, int j)
{
    updateNeighborVals(i,j);
    updateProbabilities();
    m_boardBuffer[j][i] = roll();
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

void Simulation::updateProspectiveBoundaryLengths(char prospectiveValue, int i, int j)
{}

void Simulation::updateBoardWithBuffer()
{
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            char val        = m_boardBuffer[j][i];
            m_board[j][i]   = val;
        }
    }
}

void Simulation::updateBoundaryBoardWithBuffer()
{
    for (int i = 0; i < 2*m_width-1; i++)
    {
        for (int j = 0; j < 2*m_height-1; j++)
        {
            char val        = m_boundaryBoardBuffer[j][i];
            m_boundaryBoard[j][i]   = val;
        }
    }
}

void Simulation::updateProbabilities()
{
    // Naive energy function: Energy of X is total number of neighbors - number of X neighbors
    // e.g. E(A) = A+B+C - A = B+C where A is the number of neighbors with value 'A'
    double A = m_neighborVals[0];
    double B = m_neighborVals[1];
    double C = m_neighborVals[2];

    double tension_A = m_prospectiveBoundaryLengths[0];
    double tension_B = m_prospectiveBoundaryLengths[1];
    double tension_C = m_prospectiveBoundaryLengths[2];

    double boltzmann_A = std::exp(-(B+C + tension_A)/m_kT);
    double boltzmann_B = std::exp(-(A+C + tension_B)/m_kT);
    double boltzmann_C = std::exp(-(A+B + tension_C)/m_kT);

    double Z = boltzmann_A + boltzmann_B + boltzmann_C;

    m_probabilities[0] = boltzmann_A / Z;
    m_probabilities[1] = boltzmann_B / Z;
    m_probabilities[2] = boltzmann_C / Z;

    // Reset neighborVals
    for (int i=0;i<3;i++){m_neighborVals[i]=0;}
}

void Simulation::updateBoundaryBoardBufferElements(int boardi, int boardj)
{
    int indices[3][2] = {{boardj-1,boardi+1},  
                         {boardj-1,boardi},    
                         {boardj,boardi-1}     
    };

    char ijValue = m_boardBuffer[boardj][boardi];

    for (int neighbor = 0; neighbor < 3; neighbor++)
    {
        // UPDATE BOUNDARY BOARD BUFFER
        /* This relies on the boardBuffer, which is in the process of being updated.
        But all of the elements below and to the left have already been updated, so this should be safe
        (avoiding scanning artifacts) */

        // Note that this neighbor indexing is boardNeighborIndex-3
        // Test the validity of the neighbor indices (this deals with edges and corners)
        bool coordIsValid = (indices[neighbor][0] >= 0)        && (indices[neighbor][1] >= 0)         &&
                            (indices[neighbor][0] < m_height)  && (indices[neighbor][1] < m_width);
        if(coordIsValid)
        {
            // Check the state of the board from this timestep to see what the boundary should be
            char value = m_boardBuffer[indices[neighbor][0]][indices[neighbor][1]];
            char boundaryType = '.';
            if( (value=='A' && ijValue=='B') || (value=='B' && ijValue=='A') )
            {
                boundaryType = 'D';
            }
            else if( (value=='A' && ijValue=='C') || (value=='C' && ijValue=='A') )
            {
                boundaryType = 'E';
            }
            else if( (value=='B' && ijValue=='C') || (value=='C' && ijValue=='B') )
            {
                boundaryType = 'F';
            }
            int boundi = 2*boardi;
            int boundj = 2*boardj;
            switch(neighbor)
            {
                case 0:
                boundi += +1;
                boundj += -1;
                break;
                case 1:
                boundj += -1;
                break;
                case 2:
                boundi += -1;
                break;
            }
            m_boundaryBoardBuffer[boundj][boundi] = boundaryType;
        } 
    }
}

char Simulation::roll()
{
    // roll a number between 0 and 1
    double roll = uniform(rand_generator);
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
    // for time keeping
    m_stepCounter ++;
    auto tBegin = std::chrono::high_resolution_clock::now();
    // Carry out the update element by element, going left to right, bottom to top.
    for (int j = 0; j < m_height; j++)
    {
        for (int i = 0; i < m_width; i++)
        {
            updateBufferElement(i,j);
            updateBoundaryBoardBufferElements(i,j);
        }
    } 
    updateBoardWithBuffer();
    updateBoundaryBoardWithBuffer();
    // Record time taken for the step, and print average every 1000 steps
    auto tDone = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double,std::milli> tDiff =  tDone-tBegin;
    double tDiffDouble = tDiff.count();
    m_stepDurations[m_stepCounter] = tDiffDouble;

    if ( m_stepCounter % 1000 == 0 )
    {
        // get average calculation time
        double avgStepTime;
        for (int i = 0; i < m_stepCounter; i++)
        {
            avgStepTime += m_stepDurations[i];
        }
        avgStepTime = avgStepTime / m_stepCounter;
        std::cout << "Calculated average over " << m_stepCounter << " steps:" << std::endl;

        std::cout << "Average step took " << avgStepTime << " milliseconds to calculate." << std::endl;

        std::cout << "Average update time per triplet/pixel: " << avgStepTime/(m_width*m_height) << " ms" << std::endl;
        m_stepCounter = 0;
    }
    
    //printBoardWithBoundary();
}

void Simulation::updateTemp(float value)
{
    m_kT = value;
}