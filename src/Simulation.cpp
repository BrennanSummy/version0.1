#include "Simulation.h"

Simulation::Simulation()
:rand_device(), rand_generator(rand_device()), uniform(0,1)
{
    initBoard();
}

void Simulation::updateElement(int i, int j)
{
    updateNeighborVals(i,j);
    updateProbabilities();
    board[i][j] = roll();
}

void Simulation::updateNeighborVals(int i, int j)
{
    // NOTE THAT A RIGHT-TILTED PARALLELOGRAM IS ASSUMED. 'i' INDEXES THE HORIZONTAL, 'j' THE VERTICAL
    // List the relevant neighbor indices, starting with the upper left and going clockwise
    int indices[6][2] = {   {i-1,j+1},
                            {i,j+1},
                            {i+1,j},
                            {i+1,j-1},
                            {i,j-1},
                            {i-1,j}
                        };
    
    /* Loop through all of the neighbors:
        - if(coordinate is valid)
            - check char value of board[coordinate], return A, B, or C
            - add 1 to the count of A, B, or C respectively
        - else pass
    */

}

void Simulation::updateProbabilities()
{

}

char Simulation::roll()
{

}

void Simulation::step()
{
    for (int i = 0; i << m_width; i++)
    {
        for (int j = 0; j << m_height; j++)
        {
            updateElement(i,j);
        }
    } 
}