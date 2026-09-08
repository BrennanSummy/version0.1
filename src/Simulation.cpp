#include "Simulation.h"

Simulation::Simulation(double kT, double tensionFactor, double intraLayerNearestNeighbor, double crossLayerNearestNeighbor, double crossLayerBoundaryFactor)
:rand_device(), rand_generator(rand_device()), uniform_real(0,1), uniform_3(0,2), uniform_2(0,1), uniform_int_i(0,m_width-1),uniform_int_j(0,m_height-1),
m_kT(kT), m_tensionFactor(tensionFactor), m_nearestNeighborFactor(intraLayerNearestNeighbor), m_crossLayerNearestNeighborFactor(crossLayerNearestNeighbor),
m_crossLayerBoundaryFactor(crossLayerBoundaryFactor),m_counter(BoundaryCounter(m_board,m_boundaryBoard,&width,&height,&bBWidth,&bBWidth))

{
    initBoard();
    updateBoundaryBoardViaScan();
    setDebugBoardToBoundaryBoard();
    printBoardWithBoundary();
    clearNeighborVals();
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
                m_board[0][j][i] = 'C';
                m_board[1][j][i] = 'A';
                }
                else
                {
                m_board[0][j][i] = 'B';
                m_board[1][j][i] = 'B';
                }
            }
            else
            {
                m_board[0][j][i] = 'A';
                m_board[1][j][i] = 'C';
            }
        }
    } 
    
}

void Simulation::updateBoundaryBoardViaScan()
{
    for (int top = 1; top>-1; top--)
    {
        for (int i = 0; i < m_width; i++)
        {
            for (int j = 0; j < m_height; j++)
            {
                // Check the state of the board
                char ijValue   = m_board[top][j][i];

                // Look to the upper left, upper right, and right for different domains
                int indices[3][2];
                if(top)
                {
                    indices[0][0] = j+1; indices[0][1] = i-1;
                    indices[1][0] = j+1; indices[1][1] = i  ;
                    indices[2][0] = j  ; indices[2][1] = i+1;
                }
                else
                {
                    indices[0][0] = j+1; indices[0][1] = i  ;
                    indices[1][0] = j+1; indices[1][1] = i-1;
                    indices[2][0] = j  ; indices[2][1] = i-1;
                }

                for (int neighbor = 0; neighbor < 3; neighbor++)
                {
                    // Test the validity of the neighbor indices (this deals with edges and corners)
                    bool coordIsValid = elementPositionCheck(indices[neighbor][1],indices[neighbor][0]);
                    if(coordIsValid)
                    {
                        // Check the state of the relevant neighbors
                        int neighbor_j = indices[neighbor][0];
                        int neighbor_i = indices[neighbor][1];
                        char neighborValue = m_board[top][neighbor_j][neighbor_i];

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
                        int neighbori = i;
                        int neighborj = j;
                        getNeighborEdgeCoordsFromIndex(&neighbori,&neighborj,neighbor,top);
                        //std::cout << "i,j: " << neighbori << "," << neighborj << std::endl;
                        m_boundaryBoard[top][neighborj][neighbori] = boundaryType;
                    }

                }
            }
        }
    }
}

void Simulation::printBoard()
{
    // TOP
    std::string space = " ";
    //space.append(" ");
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
            std::cout <<m_board[1][j][i] <<  " ";
        }
        std::cout << "\n";
    } 
    
    // BOTTOM
    //space.append(" ");
    for (int j = m_height-1; j >= 0; j--)
    {
        space = "";
        for (int spaces = (m_height-1-j); spaces > 0; spaces--)
        {
            space.append(" ");
        }
        std::cout << space;
        for (int i = m_width-1; i >= 0; i--)
        {
            std::cout <<m_board[0][j][i] <<  " ";
        }
        std::cout << "\n";
    } 
}

void Simulation::printBoardWithBoundary()
{
    // TOP
    std::string space = " ";
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
                std::cout <<m_board[1][doublej/2][i] <<  " ";

                if(i<(m_width - 1))
                {
                    std::cout << m_boundaryBoard[1][doublej][2*i+1] <<  " ";
                }
            }
            std::cout << "\n";
        }
        else
        {
            for (int doublei = 0; doublei < 2*m_width - 1; doublei++)
            {
                std::cout << m_boundaryBoard[1][doublej][doublei] <<  " ";
            }
            std::cout << "\n";
        }
    } 
    std::cout << "\n";

    // BOTTOM
    for (int doublej = 2*m_height-2; doublej >= 0; doublej--)
    {
        std::string space = "";
        for (int spaces = 2*m_height - 2 - doublej; spaces > 0; spaces--)
        {
            space.append(" ");
        }

        std::cout << space;

        if(doublej%2==0) // Sparse row
        {
            for (int i = m_width - 1; i >= 0; i--)
            {
                if(i<(m_width - 1))
                {
                    std::cout << m_boundaryBoard[0][doublej][2*i+1] <<  " ";
                }
                std::cout <<m_board[0][doublej/2][i] <<  " ";

            }
            std::cout << "\n";
        }
        else
        {
            for (int doublei = 2*m_width - 2; doublei >=0; doublei--)
            {
                std::cout << m_boundaryBoard[0][doublej][doublei] <<  " ";
            }
            std::cout << "\n";
        }
    } 
    std::cout << "\n";
}

int Simulation::countDissimNeighbors(bool top)
{
    int count = 0;
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            // Count neighbors, then see how many are not similar
            updateNeighborVals(i, j, top);
            char value = m_board[top][j][i];

            for (int valIndex = 0; valIndex < 3; valIndex++)
            {
                if (valIndex != domainToIntMap.at(value))
                {
                    count += m_inLayerNeighborCounts[valIndex];
                }
            }
            clearNeighborVals();
        }

    }

    return count/2;
}

int Simulation::countEdges(bool top)
{
    int count = 0;
    for (int boundi = 0; boundi < m_bBWidth; boundi++)
    {
        for (int boundj = 0; boundj < m_bBHeight; boundj++)
        {
            char val = m_boundaryBoard[top][boundj][boundi];
            if (val == 'D' || val == 'E' || val == 'F')
            {
                //DEBUGstd::cout << "Edge: " << m_boundaryBoard[top][boundj][boundi]  << std::endl;
                count++;
            }
        }
    }
    return count;
}

int Simulation::getConfigurationEnergy()
{
    int energy = 0;
    // ONLY TOP LAYER FOR NOW (top=1)
    for (int top = 1; top <2; top++)
    {
        energy += m_tensionFactor*countEdges(top);
        energy += m_nearestNeighborFactor*countDissimNeighbors(top);
    }
    return energy;
}


char Simulation::updateBoardElement(int i, int j, bool top)
{
    elementCoords element = elementCoords(i,j);
    updateNeighborVals(i,j, top);
    updateMatchingComplement(i,j,top);
    for (int x = 0; x<3;x++)
    {
        updateProspectiveBoundaryLengths(m_X[x],element, top);
    }
    updateProbabilities();
    char result = roll();
    m_board[top][j][i] = result;
    return result;
}

void Simulation::updateNeighborVals(int i, int j, bool top)
{
    // NOTE THAT A RIGHT-TILTED PARALLELOGRAM IS ASSUMED. 'i' INDEXES THE HORIZONTAL, 'j' THE VERTICAL
    // List the relevant neighbor indices, starting with the upper left and going clockwise
    int indices[6][2];
    if(top)
    {
        indices[0][0] = j+1; indices[0][1] = i-1;
        indices[1][0] = j+1; indices[1][1] = i  ;
        indices[2][0] = j  ; indices[2][1] = i+1;
        indices[3][0] = j-1; indices[3][1] = i+1;
        indices[4][0] = j-1; indices[4][1] = i  ;
        indices[5][0] = j  ; indices[5][1] = i-1;

    }
    else
    {
        indices[0][0] = j+1; indices[0][1] = i  ;
        indices[1][0] = j+1; indices[1][1] = i-1;
        indices[2][0] = j  ; indices[2][1] = i-1;
        indices[3][0] = j-1; indices[3][1] = i  ;
        indices[4][0] = j-1; indices[4][1] = i+1;
        indices[5][0] = j  ; indices[5][1] = i+1;
    }
    
    /* Loop through all of the neighbors:
        - if(coordinate is valid)
            - check char value of board[coordinate], return A, B, or C
            - add 1 to the count of A, B, or C respectively
        - else pass
    */
    for (int neighbor = 0; neighbor < 6; neighbor++)
    {
        // Test the validity of the neighbor indices (this deals with edges and corners)
        bool coordIsValid = elementPositionCheck(indices[neighbor][1],indices[neighbor][0]);
        if(coordIsValid)
        {
            // Check the state of the board to get the value of this element
            char inLayerValue = m_board[top][indices[neighbor][0]][indices[neighbor][1]];
            //char crossLayerValue = m_board[!top][indices[neighbor][0]][indices[neighbor][1]];
            // Increment the appropriate counter in m_inLayerNeighborCounts
            switch (inLayerValue)
            {
                case 'A':
                    m_inLayerNeighborCounts[0]++;
                    //m_neighborVals[neighbor] = 0;
                    break;
                case 'B':
                    m_inLayerNeighborCounts[1]++;
                    //m_neighborVals[neighbor] = 1;
                    break;
                case 'C':
                    m_inLayerNeighborCounts[2]++;
                    //m_neighborVals[neighbor] = 2;
                    break;
            }
            /*
            switch (crossLayerValue)
            {
                case 'A':
                    m_crossLayerNeighborCounts[0]++;
                    break;
                case 'B':
                    m_crossLayerNeighborCounts[1]++;
                    break;
                case 'C':
                    m_crossLayerNeighborCounts[2]++;
                    break;
            }*/
        }

    }

}

void Simulation::clearNeighborVals()
{
    for (int i=0;i<3;i++)
    {
        m_inLayerNeighborCounts[i]=0;
    }
}

void Simulation::updateMatchingComplement(int i, int j, bool top)
{
    char complement = m_board[!top][j][i];
    for(int prospValIndex = 0; prospValIndex < 3; prospValIndex++)
    {
        m_matchingComplement[prospValIndex] = (complement==m_X[prospValIndex]);
    }
}

bool Simulation::elementPositionCheck(int i, int j)
{
    return (j >= 0) && (i >= 0) && (j < m_height) && (i < m_width);
}

void Simulation::updateProspectiveBoundaryLengthsOLD(char prospectiveValue, int i, int j, bool top)
{
    //DEBUGstd::cout << "Element (i,j) = (" << i << "," << j << ")" << std::endl;
    //DEBUGstd::cout << "Top? " << top << std::endl;
    //DEBUGstd::cout << "Prospective Value: " << prospectiveValue << std::endl;
    char currentValue = m_board[top][j][i];
    pointModifyBoundaryBoard(prospectiveValue,i,j, top);
    //printBoardWithBoundary();

    int boundi=2*i;
    int boundj=2*j;
    int indices[6][2] = {   {boundj+1,boundi-1},
                            {boundj+1,boundi  },
                            {boundj  ,boundi+1},
                            {boundj-1,boundi+1},
                            {boundj-1,boundi  },
                            {boundj  ,boundi-1}
    };

    // Keep track of (up to 6) initial edge positions
    m_startingBoundaries = {0,0,0,0,0,0};
    bool allBounds = true;
    for (int neighborIndex=0;neighborIndex<6;neighborIndex++)
    {
        int loopi = i;
        int loopj = j;
        // Get the indices of the edges surrounding the board element ij
        getNeighborEdgeCoordsFromIndex(&loopi, &loopj, neighborIndex, top);
        if(!edgePositionCheck((int[2]) {loopj, loopi})){allBounds=false; continue;}
        char boundaryType = m_boundaryBoard[top][loopj][loopi];
        if ( boundaryType == domainToBoundaryMap.at(prospectiveValue)[0] || boundaryType == domainToBoundaryMap.at(prospectiveValue)[1] )
        {
            // Remember this edge's position
            m_remainingInitEdges.push_back({loopj, loopi, neighborIndex});
        }
        else{allBounds=false;}

    }
    // Save some processing time by abandoning when every edge is a compatible boundary
    if(allBounds)
    {
        //DEBUGstd::cout << "Setting boundary length to 6, as the element is surrounded by boundary" << std::endl;
        m_prospectiveBoundaryLengths[domainToIntMap.at(prospectiveValue)]=6;
        m_remainingInitEdges.clear();
        // Revert point modification
        pointModifyBoundaryBoard(currentValue,i,j,top);
        return;
    }
    // Look through every compatible boundary around the element one at a time
    while (m_remainingInitEdges.size()>0)
    {
        // Keep track of whether or not the traced boundary makes a loop
        bool loop = false;

        // Find the next starting edge neighbor index (this element will be removed once exploreLine is called)
        int initialEdgeIndex = m_remainingInitEdges[0][2];


        // These indices are turned into the appropriate starting edge coordinates by the function below
        int edgei = i;
        int edgej = j;

        // Get the indices of the edges surrounding the board element ij
        getNeighborEdgeCoordsFromIndex(&edgei, &edgej, initialEdgeIndex, top);
        // If this edge overlaps the one on the other side of the board, count that
        if (m_boundaryBoard[!top][edgej][edgei] != '.')
        {
            m_prospectiveBoundaryOverlapCounts[domainToIntMap.at(prospectiveValue)]++;
            //std::cout << "Overlap: " << m_prospectiveBoundaryOverlapCounts[domainToIntMap.at(prospectiveValue)] << std::endl;
        }
        

        // Count this starting edge in the length
        m_prospectiveBoundaryLengths[domainToIntMap.at(prospectiveValue)]++;

        // For each starting edge, travel down the line first in the 0 direction then in the 1 direction if not a loop
        for (int startingDirection=0;startingDirection<2;startingDirection++)
        {   
            //DEBUGstd::cout<<"direction: " << startingDirection<<std::endl;
            // These indices are turned into the appropriate starting edge coordinates by the function below
            int loopi = i;
            int loopj = j;

            // Get the indices of the edges surrounding the board element ij
            getNeighborEdgeCoordsFromIndex(&loopi, &loopj, initialEdgeIndex, top);

            // Set start position to this edge and make a position buffer
            int initPosition[3]  = {loopj          , loopi          , top};
            int position[3]      = {initPosition[0], initPosition[1], top};

            //DEBUGsetDebugBoardToBoundaryBoard();
            //DEBUGsetDebugBoardElement(2*i, 2*j, top, '+');
            //DEBUGsetDebugBoardElement(loopi, loopj, top, '!');

            // Look down the line of contiguous edges and break if a loop is found
            exploreLineOLD(initPosition, position, startingDirection, prospectiveValue, &loop);
            //DEBUGprintDebugBoard();

            if(loop){break;}
        }   
    }
    
    // Revert point modification
    pointModifyBoundaryBoard(currentValue,i,j, top);
}

void Simulation::updateProspectiveBoundaryLengths(char prospectiveValue, elementCoords element, bool top)
{
    //DEBUGstd::cout << "Element (i,j) = (" << i << "," << j << ")" << std::endl;
    //DEBUGstd::cout << "Top? " << top << std::endl;
    //DEBUGstd::cout << "Prospective Value: " << prospectiveValue << std::endl;

    char currentValue = m_board[top][element.j][element.i];

    pointModifyBoundaryBoard(prospectiveValue,element.i,element.j, top);
    // DEBUG
    //setDebugBoardToBoundaryBoard();
    //setDebugBoardElement(2*element.i, 2*element.j, top, '+');
    //printDebugBoard();
    // DEBUG

    int boundaryOverlaps;

    int length = m_counter.countProspectiveBoundaryLength(prospectiveValue,element,top,&boundaryOverlaps);
    
    m_prospectiveBoundaryLengths[domainToIntMap.at(prospectiveValue)] = length;

    m_prospectiveBoundaryOverlapCounts[domainToIntMap.at(prospectiveValue)] = boundaryOverlaps;

    // Revert point modification
    pointModifyBoundaryBoard(currentValue,element.i,element.j, top);
}

void Simulation::exploreLine(std::vector<std::array<int,3>>& remainingInitEdges, int* startingEdgePosition, int* position, bool direction, char prospectiveValue, bool* looptr)
{
    // Test to see if this edge is one of the remaining starting edges
    for (int edgeIndex = 0; edgeIndex<remainingInitEdges.size(); edgeIndex++)
    {
        if ( position[0]==remainingInitEdges[edgeIndex][0] && position[1]==remainingInitEdges[edgeIndex][1])
        {
            remainingInitEdges.erase(remainingInitEdges.begin() + edgeIndex);
        }
    }

    // Use the direction to select the two possible edge neighbor indices to look through
    int startIndex = direction * 2;
    int endIndex = startIndex + 2;
    int positionBuffer[2];
    for (int edgeNeighborIndex = startIndex; edgeNeighborIndex<endIndex; edgeNeighborIndex++)
    {
        // Set position buffer
        positionBuffer[0] = position[0]; positionBuffer[1] = position[1];

        // Load the position of the neighboring edge into the position buffer
        getAdjacentEdgePosition(positionBuffer, edgeNeighborIndex, position[2]);

        /* Make sure that position is valid (not out of bounds)
            if it is, skip the rest of this iteration to look at the next edge.*/
        if(!edgePositionCheck(positionBuffer)){continue;}

        char val = m_boundaryBoard[position[2]][positionBuffer[0]][positionBuffer[1]];

        // If that edge has a compatible boundary type, recurse then break
        if( val == domainToBoundaryMap.at(prospectiveValue)[0] ||  val == domainToBoundaryMap.at(prospectiveValue)[1])
        {
            // Increment the correct prospective boundary length
            m_prospectiveBoundaryLengths[domainToIntMap.at(prospectiveValue)]++;
            
            // Make sure that this isn't a loop by checking equivalence with starting position
            if ( startingEdgePosition[0]==positionBuffer[0] &&
                    startingEdgePosition[1]==positionBuffer[1])
                    {
                        //std::cout << "loop" << std::endl;
                        *looptr = true;
                        break;
                    }

            // Get the direction for the next step
            bool newDirection = getEdgeFindingDirection(position, edgeNeighborIndex);

            // Use the buffer to update position
            position[0] = positionBuffer[0]; position[1] = positionBuffer[1];

            // Recurse
            exploreLine(remainingInitEdges, startingEdgePosition, position, newDirection, prospectiveValue, looptr);
            
            break;
        }
    }

}

/*
std::vector<int> Simulation::getBoundaryLengths()
{
    // Make a boolean array with the shape of the board. This acts as a checklist (which elements have had their boundaries inspected)
    bool remainingElements[2][m_height][m_width];
    for (int top = 1; top >-1; top--)
    {
        for (int j = 0; j < m_height; j++)
        {
            for (int i = 0; i < m_width; i++)
            {
                remainingElements[top][j][i] = false;
            }
        }
    }

    // Declare the vector of lengths
    std::vector<int> boundaryLengths;
    // Go through the remaining elements one by one, marking an element as true if it has been directly checked or if it is adjacent and compatible
    // with a boundary connected to a checked element
    for (int top = 1; top >-1; top--)
    {
        for (int j = 0; j < m_height; j++)
        {
            for (int i = 0; i < m_width; i++)
            {
                if(remainingElements[top][j][i] == false)
                {
                    int length = getBoundaryLengthAtElement(remainingElements, i, j, top);
                    boundaryLengths.push_back(length);
                }
            }
        }
    }

}
    


void Simulation::exploreLineWithElementTracking(int* length, std::vector<std::array<int,2>>& passedElements, std::vector<std::array<int,3>>& remainingInitEdges,
                                                int* startingEdgePosition, int* position, bool direction, char value, bool* looptr)
{
    // Test to see if this edge is one of the remaining starting edges
    for (int edgeIndex = 0; edgeIndex<remainingInitEdges.size(); edgeIndex++)
    {
        if ( position[0]==remainingInitEdges[edgeIndex][0] && position[1]==remainingInitEdges[edgeIndex][1])
        {
            remainingInitEdges.erase(remainingInitEdges.begin() + edgeIndex);
        }
    }

    // Look on both sides of this edge to identify the coordinates of the element with the given value (we want to count this edge twice in the grand scheme)
    std::vector<std::array<int,2>> adjElementCoords = getAdjElementPositionsFromEdgePos(position);
    for (int n=0;n<2;n++)
    {
        std::array<int,2> coords = adjElementCoords[n];
        if (m_board[position[2]][coords[0]][coords[1]] == value)
        {
            passedElements.push_back(coords);
            // Later these coordinates will be removed from the vector of remaining coordinates to investigate
        }
    }


    // Use the direction to select the two possible edge neighbor indices to look through
    int startIndex = direction * 2;
    int endIndex = startIndex + 2;
    int positionBuffer[2];
    for (int edgeNeighborIndex = startIndex; edgeNeighborIndex<endIndex; edgeNeighborIndex++)
    {
        // Set position buffer
        positionBuffer[0] = position[0]; positionBuffer[1] = position[1];

        // Load the position of the neighboring edge into the position buffer
        getAdjacentEdgePosition(positionBuffer, edgeNeighborIndex, position[2]);

        if(!edgePositionCheck(positionBuffer)){continue;}

        char val = m_boundaryBoard[position[2]][positionBuffer[0]][positionBuffer[1]];

        // If that edge has a compatible boundary type, recurse then break
        if( val == domainToBoundaryMap.at(value)[0] ||  val == domainToBoundaryMap.at(value)[1])
        {
            // Increment the length
            //m_prospectiveBoundaryLengths[domainToIntMap.at(value)]++;
            *length ++;
            
            // Make sure that this isn't a loop by checking equivalence with starting position
            if ( startingEdgePosition[0]==positionBuffer[0] &&
                    startingEdgePosition[1]==positionBuffer[1])
                    {
                        //std::cout << "loop" << std::endl;
                        *looptr = true;
                        break;
                    }

            // Get the direction for the next step
            bool newDirection = getEdgeFindingDirection(position, edgeNeighborIndex);

            // Use the buffer to update position
            position[0] = positionBuffer[0]; position[1] = positionBuffer[1];

            // Recurse
            exploreLineWithElementTracking(length, passedElements, remainingInitEdges, startingEdgePosition,
                                           position, newDirection, value, looptr);
            
            break;
        }
    }

}
*/
void Simulation::exploreLineOLD(int* startingEdgePosition, int* position, bool direction, char prospectiveValue, bool* looptr)
{

    // Test to see if this edge is one of the remaining starting edges
    for (int edgeIndex = 0; edgeIndex<m_remainingInitEdges.size(); edgeIndex++)
    {
        if ( position[0]==m_remainingInitEdges[edgeIndex][0] && position[1]==m_remainingInitEdges[edgeIndex][1])
        {
            m_remainingInitEdges.erase(m_remainingInitEdges.begin() + edgeIndex);
        }
    }



    // Use the direction to select the two possible edge neighbor indices to look through
    int startIndex = direction * 2;
    int endIndex = startIndex + 2;
    int positionBuffer[2];
    for (int edgeNeighborIndex = startIndex; edgeNeighborIndex<endIndex; edgeNeighborIndex++)
    {
        // Set position buffer
        positionBuffer[0] = position[0]; positionBuffer[1] = position[1];
        //DEBUGstd::cout << "initial pos " << positionBuffer[1] << "i, " << position[0] << "j" << std::endl;
        //DEBUGstd::cout << "neighbor index " << edgeNeighborIndex<< std::endl;
        // Load the position of the neighboring edge into the position buffer
        getAdjacentEdgePosition(positionBuffer, edgeNeighborIndex, position[2]);
        //DEBUGstd::cout << "candidate pos " << positionBuffer[1] << "i, " << positionBuffer[0] << "j" << std::endl;
        /* Make sure that position is valid (not out of bounds)
            if it is, skip the rest of this iteration to look at the next edge.*/
        if(!edgePositionCheck(positionBuffer)){continue;}
        //DEBUGstd::cout << "passed pos check" << std::endl;
        char val = m_boundaryBoard[position[2]][positionBuffer[0]][positionBuffer[1]];
        //DEBUGstd::cout << "looking for " << prospectiveValue << " compatible boundary at " << positionBuffer[1]<< "i, "<<positionBuffer[0]<<"j."<< std::endl;
        // If that edge has a compatible boundary type, recurse then break
        if( val == domainToBoundaryMap.at(prospectiveValue)[0] ||  val == domainToBoundaryMap.at(prospectiveValue)[1])
        {
            // Increment the correct prospective boundary length
            m_prospectiveBoundaryLengths[domainToIntMap.at(prospectiveValue)]++;
            
            //DEBUG
            /* DEBUG 
            try{

            //std::cout<<"val: " << prospectiveValue<<" pos: " << positionBuffer[1]<< "i, "<<positionBuffer[0]<<"j. length: " << 
            //m_prospectiveBoundaryLengths[domainToIntMap.at(prospectiveValue)] << std::endl;
            std::cout<<"length: " << 
            m_prospectiveBoundaryLengths[domainToIntMap.at(prospectiveValue)] << std::endl;
            std::cout << "//////////////////////////////////////////element done//////////////////////////////////////////"<< std::endl;
            */

            setDebugBoardElement(positionBuffer[1],positionBuffer[0],position[2],'@');

            /*
            if(m_prospectiveBoundaryLengths[domainToIntMap.at(prospectiveValue)] > m_width*m_height)
            {
                throw std::runtime_error("Length of boundary exceeded reasonable length");
            }
            }
            catch(const std::exception& e){std::cerr<<e.what()<<std::endl;}
            //std::cout<<"starting pos: " << startingEdgePosition[1]<< "i, "<<startingEdgePosition[0]<<"j." << std::endl;
            //std::cout<<"Top: "<< position[2] <<std::endl;
            */
            
            // Make sure that this isn't a loop by checking equivalence with starting position
            if ( startingEdgePosition[0]==positionBuffer[0] &&
                    startingEdgePosition[1]==positionBuffer[1])
                    {
                        //std::cout << "loop" << std::endl;
                        *looptr = true;
                        break;
                    }

            // Get the direction for the next step
            bool newDirection = getEdgeFindingDirection(position, edgeNeighborIndex);
            // Use the buffer to update position
            position[0] = positionBuffer[0]; position[1] = positionBuffer[1];

            // Increment the overlap count if appropriate
            if (m_boundaryBoard[!position[2]][position[0]][position[1]] != '.')
            {
                //m_prospectiveBoundaryOverlapCounts[domainToIntMap.at(prospectiveValue)]++;
                //std::cout << "Overlap: " << m_prospectiveBoundaryOverlapCounts[domainToIntMap.at(prospectiveValue)] << std::endl;
            }
            // Recurse
            exploreLineOLD(startingEdgePosition, position, newDirection, prospectiveValue, looptr);
            
            break;
        }
    }

}

bool Simulation::edgePositionCheck(int* position)
{
    bool positionIsValid = (position[0] >= 0)        && (position[1] >= 0)         &&
                        (position[0] < m_bBHeight)  && (position[1] < m_bBWidth);
    return positionIsValid;
}

bool Simulation::getEdgeFindingDirection(int* position, int chosenNeighborIndex)
{
    // I THINK THIS IS BLOATED
    bool direction;
    if(position[2]) // TOP
    {
        if (position[0] % 2 ==0) // sparse row
        {
            switch(chosenNeighborIndex)
            {
                case 0:
                    direction=1;
                    break;
                case 1:
                    direction=0;
                    break;
                case 2:
                    direction=0;
                    break;
                case 3:
                    direction=1;
                    break;
            }
        }
        else // dense row
        {
            if(position[1]%2==1) // Right-Up pointing line
            {
                switch(chosenNeighborIndex)
                {
                    case 0:
                        direction=0;
                        break;
                    case 1:
                        direction=0;
                        break;
                    case 2:
                        direction=1;
                        break;
                    case 3:
                        direction=1;
                        break;
                }
            }
            if(position[1]%2==0) // Left-Up pointing line
            {
                switch(chosenNeighborIndex)
                {
                    case 0:
                        direction=0;
                        break;
                    case 1:
                        direction=1;
                        break;
                    case 2:
                        direction=1;
                        break;
                    case 3:
                        direction=0;
                        break;
                }
            }
        }
    } 
    else // BOTTOM
    {
        if (position[0] % 2 ==0) // sparse row
        {
            switch(chosenNeighborIndex)
            {
                case 0:
                    direction=1;
                    break;
                case 1:
                    direction=0;
                    break;
                case 2:
                    direction=0;
                    break;
                case 3:
                    direction=1;
                    break;
            }
        }
        else // dense row
        {
            if(position[1]%2==0) // Right-Up pointing line
            {
                switch(chosenNeighborIndex)
                {
                    case 0:
                        direction=0;
                        break;
                    case 1:
                        direction=0;
                        break;
                    case 2:
                        direction=1;
                        break;
                    case 3:
                        direction=1;
                        break;
                }
            }
            if(position[1]%2==1) // Left-Up pointing line
            {
                switch(chosenNeighborIndex)
                {
                    case 0:
                        direction=0;
                        break;
                    case 1:
                        direction=1;
                        break;
                    case 2:
                        direction=1;
                        break;
                    case 3:
                        direction=0;
                        break;
                }
            }
        }
    }
    
    return direction;
}

void Simulation::getAdjacentEdgePosition(int* edgePosition, int neighborIndex, bool top)
{
    // I THINK THIS IS BLOATED
    if(top) // top board
    {
        //std::cout << "looking for adjacent edges in top board" << std::endl;
        if (edgePosition[0] % 2 ==0) // sparse row
        {
            switch(neighborIndex)
            {
                case 0:
                    edgePosition[1]--;
                    edgePosition[0]++;
                    break;
                case 1:
                    edgePosition[0]++;
                    break;
                case 2:
                    edgePosition[1]++;
                    edgePosition[0]--;
                    break;
                case 3:
                    edgePosition[0]--;
                    break;
            }
        }
        else // dense row
        {
            if(edgePosition[1]%2==1) // Right-Up pointing line
            {
                switch(neighborIndex)
                {
                    case 0:
                        edgePosition[0]++;
                        break;
                    case 1:
                        edgePosition[1]++;
                        break;
                    case 2:
                        edgePosition[0]--;
                        break;
                    case 3:
                        edgePosition[1]--;
                        break;
                }
            }
            else if(edgePosition[1]%2==0) // Left-Up pointing line
            {
                switch(neighborIndex)
                {
                    case 0:
                        edgePosition[1]++;
                        break;
                    case 1:
                        edgePosition[1]++;
                        edgePosition[0]--;
                        break;
                    case 2:
                        edgePosition[1]--;
                        break;
                    case 3:
                        edgePosition[0]++;
                        edgePosition[1]--;
                        break;
                }
            }
        }
    }
    else // bottom board (note that directions assume you are looking down at a board with the bottom facing you)
    {
        //std::cout << "looking for adjacent edges in bottom board" << std::endl;
        if (edgePosition[0] % 2 ==0) // sparse row
        {
            switch(neighborIndex)
            {
                case 0: // Upper Left Neighbor from birds eye WRT bottom
                    edgePosition[0]++;
                    break;
                case 1:
                    edgePosition[1]--;
                    edgePosition[0]++;
                    break;
                case 2:
                    edgePosition[0]--;
                    break;
                case 3: // Lower Left Neighbor
                    edgePosition[1]++;
                    edgePosition[0]--;
                    break;
            }
        }
        else // dense row
        {
            if(edgePosition[1]%2==0) // Right-Up pointing line
            {
                switch(neighborIndex)
                {
                    case 0: // Upper neighbor (vertical line)
                        edgePosition[1]--;
                        edgePosition[0]++;
                        break;
                    case 1:
                        edgePosition[1]--;
                        break;
                    case 2:
                        edgePosition[1]++;
                        edgePosition[0]--;
                        break;
                    case 3: // Left Neighbor
                        edgePosition[1]++;
                        break;
                }
            }
            else if(edgePosition[1]%2==1) // Left-Up pointing line
            {
                switch(neighborIndex)
                {
                    case 0:
                        edgePosition[1]--;
                        break;
                    case 1:
                        edgePosition[0]--;
                        break;
                    case 2:
                        edgePosition[1]++;
                        break;
                    case 3:
                        edgePosition[0]++;
                        break;
                }
            }
        }
    }
}

void Simulation::pointModifyBoundaryBoard(char prospectiveValue, int i, int j, bool top)
{
    int indices[6][2];
    if(top)
    {
        indices[0][0] = j+1; indices[0][1] = i-1;
        indices[1][0] = j+1; indices[1][1] = i  ;
        indices[2][0] = j  ; indices[2][1] = i+1;
        indices[3][0] = j-1; indices[3][1] = i+1;
        indices[4][0] = j-1; indices[4][1] = i  ;
        indices[5][0] = j  ; indices[5][1] = i-1;

    }
    else
    {
        indices[0][0] = j+1; indices[0][1] = i  ;
        indices[1][0] = j+1; indices[1][1] = i-1;
        indices[2][0] = j  ; indices[2][1] = i-1;
        indices[3][0] = j-1; indices[3][1] = i  ;
        indices[4][0] = j-1; indices[4][1] = i+1;
        indices[5][0] = j  ; indices[5][1] = i+1;
    }
    
    for (int neighbor = 0; neighbor < 6; neighbor++)
    {
        // Test the validity of the neighbor indices (this deals with edges and corners)
        bool coordIsValid = elementPositionCheck(indices[neighbor][1],indices[neighbor][0]);
        if(coordIsValid)
        {
            // Check the state of the board for the neighbor
            char neighborValue = m_board[top][indices[neighbor][0]][indices[neighbor][1]];

            char boundaryType = '.';
            if( (neighborValue=='A' && prospectiveValue=='B') || (neighborValue=='B' && prospectiveValue=='A') )
            {
                boundaryType = 'D';
            }
            else if( (neighborValue=='A' && prospectiveValue=='C') || (neighborValue=='C' && prospectiveValue=='A') )
            {
                boundaryType = 'E';
            }
            else if( (neighborValue=='B' && prospectiveValue=='C') || (neighborValue=='C' && prospectiveValue=='B') )
            {
                boundaryType = 'F';
            }
            /* Update the boundary array using the correct indices as judged by the
                neighbor loop index*/

            int neighbori = i;
            int neighborj = j;
            getNeighborEdgeCoordsFromIndex(&neighbori,&neighborj,neighbor,top);
            /*
            if(neighbori==-1 || neighborj ==-1)
            {
                std::cout << top << std::endl;
                std::cout << "element neighbor: " << indices[neighbor][1] << "," << indices[neighbor][0] <<std::endl;
                std::cout << "i,j: " << i << "," << j << "\nni,nj: " << neighbori << "," << neighborj << std::endl;
            }
            */

            m_boundaryBoard[top][neighborj][neighbori] = boundaryType;
        }

    }

}

void Simulation::getNeighborEdgeCoordsFromIndex(int* iptr, int* jptr, int neighborIndex, bool top)
{
    *iptr = 2* (*iptr);
    *jptr = 2* (*jptr);
    if(top)
    {
        switch(neighborIndex)
        {
            case 0:
            *iptr += -1;
            *jptr +=  1;
            break;
            case 1:
            *jptr +=  1;
            break;
            case 2:
            *iptr +=  1;
            break;
            case 3:
            *iptr += +1;
            *jptr += -1;
            break;
            case 4:
            *jptr += -1;
            break;
            case 5:
            *iptr += -1;
            break;
        }

    }
    else // Bottom layer
    {
        switch(neighborIndex)
        {
            case 0:
            *jptr +=  1;
            break;
            case 1:
            *iptr += -1;
            *jptr +=  1;
            break;
            case 2:
            *iptr += -1;
            break;
            case 3:
            *jptr += -1;
            break;
            case 4:
            *jptr += -1;
            *iptr += +1;
            break;
            case 5:
            *iptr += +1;
            break;
        }
    }
}

void Simulation::updateProbabilities()
{
    // Naive energy function: Energy of X is total number of neighbors - number of X neighbors
    // e.g. E(A) = A+B+C - A = B+C where A is the number of neighbors with value 'A'
    //////////////////////////////// Nearest Neighbor ////////////////////////////////
    ////double A = m_inLayerNeighborCounts[0];
    ////double B = m_inLayerNeighborCounts[1];
    ////double C = m_inLayerNeighborCounts[2];

    ////double nearest_A = m_nearestNeighborFactor*(B+C);
    ////double nearest_B = m_nearestNeighborFactor*(A+C);
    ////double nearest_C = m_nearestNeighborFactor*(B+A);


    int ip1;
    int ip2;
    for (int i = 0; i<3; i++)
    {
        ip1 = (i + 1) % 3;
        ip2 = (i + 2) % 3;
        //////////////////////////////// Nearest Neighbor ////////////////////////////////
        m_nearestEnergy[i] = m_nearestNeighborFactor * (m_inLayerNeighborCounts[ip1] + m_inLayerNeighborCounts[ip2]);
        //////////////////////////////// Nearest Neighbor ////////////////////////////////

        ////////////////////////////////     Tension      ////////////////////////////////
        m_tensionEnergy[i] = m_tensionFactor*m_prospectiveBoundaryLengths[i];
        ////////////////////////////////     Tension      ////////////////////////////////

        //////////////////////////////// Cross Layer Nearest Neighbor ////////////////////////////////
        m_cLNearestEnergy[i] = m_crossLayerNearestNeighborFactor * m_matchingComplement[i];
        //////////////////////////////// Cross Layer Nearest Neighbor ////////////////////////////////

        //////////////////////////////// Cross Layer Boundary Interference ////////////////////////////////
        m_cLBoundaryInterferenceEnergy[i] = 100*m_crossLayerBoundaryFactor*m_prospectiveBoundaryOverlapCounts[i];
        //////////////////////////////// Cross Layer Boundary Interference ////////////////////////////////


        // Total Energy
        m_energies[i] = (m_nearestEnergy[i] + m_tensionEnergy[i] + m_cLNearestEnergy[i] + m_cLBoundaryInterferenceEnergy[i]);
    }
    //////////////////////////////// Nearest Neighbor ////////////////////////////////

    ////////////////////////////////     Tension      ////////////////////////////////
    //double tension_A = m_tensionFactor*m_prospectiveBoundaryLengths[0];
    //double tension_B = m_tensionFactor*m_prospectiveBoundaryLengths[1];
    //double tension_C = m_tensionFactor*m_prospectiveBoundaryLengths[2];
    ////////////////////////////////     Tension      ////////////////////////////////

    //////////////////////////////// Cross Layer Nearest Neighbor ////////////////////////////////
    // THESE WOULD ALLOW TRACKING OF THE NEIGHBORS OF THE COMPLEMENT
    // IRRELEVANT double A_CL = m_crossLayerNeighborCounts[0];
    // IRRELEVANT double B_CL = m_crossLayerNeighborCounts[1];
    // IRRELEVANT double C_CL = m_crossLayerNeighborCounts[2];


    // A positive CL neighbor factor -> matching complement adds energy
    ////double cLNearest_A = m_crossLayerNearestNeighborFactor*(m_matchingComplement[0]);// + (B_CL + C_CL));
    ////double cLNearest_B = m_crossLayerNearestNeighborFactor*(m_matchingComplement[1]);// + (A_CL + C_CL));
    ////double cLNearest_C = m_crossLayerNearestNeighborFactor*(m_matchingComplement[2]);// + (A_CL + B_CL));

    //////////////////////////////// Cross Layer Nearest Neighbor ////////////////////////////////

    //////////////////////////////// Cross Layer Boundary Interference ////////////////////////////////
    ////int A_overlaps = m_prospectiveBoundaryOverlapCounts[0];
    ////int B_overlaps = m_prospectiveBoundaryOverlapCounts[1];
    ////int C_overlaps = m_prospectiveBoundaryOverlapCounts[2];

    ////double cLBoundaryInterface_A = 100*m_crossLayerBoundaryFactor*A_overlaps;
    ////double cLBoundaryInterface_B = 100*m_crossLayerBoundaryFactor*B_overlaps;
    ////double cLBoundaryInterface_C = 100*m_crossLayerBoundaryFactor*C_overlaps;

    //////////////////////////////// Cross Layer Boundary Interference ////////////////////////////////

    //m_energies[0] = (nearest_A + tension_A + cLNearest_A + cLBoundaryInterface_A);
    //m_energies[1] = (nearest_B + tension_B + cLNearest_B + cLBoundaryInterface_B);
    //m_energies[2] = (nearest_C + tension_C + cLNearest_C + cLBoundaryInterface_C);

    double boltzmann_A = std::exp(-(m_energies[0])/m_kT);
    double boltzmann_B = std::exp(-(m_energies[1])/m_kT);
    double boltzmann_C = std::exp(-(m_energies[2])/m_kT);

    double Z = boltzmann_A + boltzmann_B + boltzmann_C;

    m_probabilities[0] = boltzmann_A / Z;
    m_probabilities[1] = boltzmann_B / Z;
    m_probabilities[2] = boltzmann_C / Z;
    /*
    /////////////Energy Analysis ////////////////////////////////

    double min = m_probabilities[0];
    double max = m_probabilities[0];
    int min_ind = 0;
    int max_ind = 0;
    for (int i = 1; i < 3; i++)
    {
        if (m_probabilities[i] < min)
        {
            min = m_probabilities[i];
            min_ind = i;
        }
        if (m_probabilities[i] > max)
        {
            max = m_probabilities[i];
            max_ind = i;
        }
    }
    int middle_ind;
    if(min_ind == (max_ind+1)%3)
    {
        middle_ind = (max_ind+2)%3;
    } else
    {
        middle_ind = (max_ind+1)%3;
    }
    //ip1 = (min_ind+1)%3;
    //ip2 = (min_ind+2)%3;
    // If two domains share 90% of probability
    //if(min < 0.1 && (m_prospectiveBoundaryLengths[ip1]!=6))
    if(true)
    {
        if(true)
        {
            int dL12 = m_prospectiveBoundaryLengths[middle_ind]- m_prospectiveBoundaryLengths[max_ind];
            double dN12 = (m_nearestEnergy[middle_ind] - m_nearestEnergy[max_ind])/m_nearestNeighborFactor;
            int dL13 = m_prospectiveBoundaryLengths[min_ind]- m_prospectiveBoundaryLengths[max_ind];
            double dTensionEnergy12  = m_tensionEnergy[middle_ind] - m_tensionEnergy[max_ind];
            double dTensionEnergy13  = m_tensionEnergy[min_ind] - m_tensionEnergy[max_ind];
            double dNeighborEnergy12 = m_nearestEnergy[middle_ind] - m_nearestEnergy[max_ind];
            double dNeighborEnergy13 = m_nearestEnergy[min_ind] - m_nearestEnergy[max_ind];
            double dTensionEnergy23  = m_tensionEnergy[min_ind] - m_tensionEnergy[middle_ind];
            double dNeighborEnergy23 = m_nearestEnergy[min_ind] - m_nearestEnergy[middle_ind];

            //double predDL12 = (1/m_tensionFactor)*(m_kT - dNeighborEnergy12);
            //double predDL13 = (1/m_tensionFactor)*(m_kT - dNeighborEnergy13);
            //double quantity = ((dTensionEnergy + dNeighborEnergy)/(m_kT));
            //double logProbRatio12 = log(m_probabilities[max_ind]/m_probabilities[middle_ind]);
            //double logProbRatio13 = log(m_probabilities[max_ind]/m_probabilities[min_ind]);
            //std::cout << "X: " << dTensionEnergy  << std::endl;
            //std::cout << "Y: " << dNeighborEnergy << std::endl;
            //std::cout << dTensionEnergy  << "," << dNeighborEnergy << std::endl;
            //std::cout << m_nearestNeighborFactor/m_tensionFactor  << "," << dL << std::endl;
            //std::cout << quantity << "," << dL << std::endl;
            //std::cout << "Quantity: " << quantity << " LPR: " << logProbRatio << std::endl;
            //std::cout << "LPR12: " << logProbRatio12 << " LPR13: " << logProbRatio13 << std::endl;
            //std::cout << "d12TN: " << dTensionEnergy12 << "," << dNeighborEnergy12 << std::endl;
            //std::cout << "d13TN: " << dTensionEnergy13 << "," << dNeighborEnergy13 << std::endl;
            //std::cout << "d23TN: " << dTensionEnergy23 << "," << dNeighborEnergy23 << std::endl;
            //std::cout << "dL12,13: " << dL12 << "," << dL13 << std::endl;
            std::cout << dL12 << "," << dN12 << std::endl;

        }
    }
    //////////////////////////////// Energy Analysis ////////////////////////////////
    */
    // Reset neighbor counts and boundary lengths
    for (int i=0;i<3;i++)
    {
        m_inLayerNeighborCounts[i]=0;
        m_crossLayerNeighborCounts[i]=0;
        m_prospectiveBoundaryLengths[i]=0;
        m_prospectiveBoundaryOverlapCounts[i]=0;
    }
}

void Simulation::updateBoundaryBoardElements(int boardi, int boardj, bool top)
{
    int indices[6][2] = {   {boardj+1,boardi-1},
                            {boardj+1,boardi},
                            {boardj  ,boardi+1},
                            {boardj-1,boardi+1},
                            {boardj-1,boardi},
                            {boardj  ,boardi-1}
    };

    char ijValue = m_board[top][boardj][boardi];

    for (int neighbor = 0; neighbor < 6; neighbor++)
    {
        // UPDATE BOUNDARY BOARD

        // Note that this neighbor indexing is the same as boardNeighborIndex
        // Test the validity of the neighbor indices (this deals with edges and corners)
        bool coordIsValid = (indices[neighbor][0] >= 0)        && (indices[neighbor][1] >= 0)         &&
                            (indices[neighbor][0] < m_height)  && (indices[neighbor][1] < m_width);
        if(coordIsValid)
        {
            // Check the state of the board to see what the boundary should be
            char value = m_board[top][indices[neighbor][0]][indices[neighbor][1]];
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
                boundi += -1;
                boundj +=  1;
                break;
                case 1:
                boundj +=  1;
                break;
                case 2:
                boundi +=  1;
                break;
                case 3:
                boundi += +1;
                boundj += -1;
                break;
                case 4:
                boundj += -1;
                break;
                case 5:
                boundi += -1;
                break;
            }
            m_boundaryBoard[top][boundj][boundi] = boundaryType;
        } 
    }
}

char Simulation::roll()
{
    // roll a number between 0 and 1
    double roll = uniform_real(rand_generator);
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
    else // If kT is so low that some probs spike and others vanish, choose the lowest energy state
    {
        //std::cout << m_energies[0] << ":" << m_energies[1] << ":" << m_energies[2] << std::endl;
        // Select the minimum energy, or do a 1/3 roll if they are equal
        float smallestEnergy = std::min({m_energies[0],m_energies[1],m_energies[2]});
        //std::cout << "smallest: " << smallestEnergy << std::endl;


        // Logic vals

        bool abEqual = m_energies[0] == m_energies[1];
        bool acEqual = m_energies[0] == m_energies[2];
        bool bcEqual = m_energies[1] == m_energies[2];

        bool aSmall= ((m_energies[0] < m_energies[1]) && (m_energies[0] < m_energies[2]));

        bool bSmall= ((m_energies[1] < m_energies[0]) && (m_energies[1] < m_energies[2]));

        bool cSmall= ((m_energies[2] < m_energies[0]) && (m_energies[2] < m_energies[1]));

        bool allEqual = abEqual && acEqual;

        if(allEqual)
        {
            switch(uniform_3(rand_generator))
            {
                case 0:
                    val='A';
                    break;
                case 1:
                    val='B';
                    break;
                case 2:
                    val='C';
                    break;
            }
            return val;
        }
        else if (aSmall) // A has lowest energy
        {
            val='A';
        }
        else if (bSmall) // B has lowest energy
        {
            val='B';
        }
        else if (cSmall) // C has lowest energy
        {
            val='C';
        }
        else if (abEqual)
        {
            switch(uniform_2(rand_generator))
            {
                case 0:
                    val='A';
                    break;
                case 1:
                    val='B';
                    break;
            }
        }
        else if (acEqual)
        {
            switch(uniform_2(rand_generator))
            {
                case 0:
                    val='A';
                    break;
                case 1:
                    val='C';
                    break;
            }
        }
        else if (bcEqual)
        {
            switch(uniform_2(rand_generator))
            {
                case 0:
                    val='B';
                    break;
                case 1:
                    val='C';
                    break;
            }
        }
    }
    return val;
}

void Simulation::randomStep()
{
    if ( m_stepCounter % 100 == 0 )
    {
        // Print energy
        std::cout << "______________________________________________________________________________" << std::endl;
        std::cout << "Energy: " << getConfigurationEnergy() << std::endl;
        //printBoardWithBoundary();
        std::vector<int> lengths = m_counter.boundaryScan(1);
        int boundCount = lengths.size();
        std::cout<< "All boundaries found in top layer: " << boundCount << std::endl;
        double avg=0;
        int maxBound=0;
        for (int n = 0; n < boundCount; n++)
        {
            int length = lengths[n];
            if(length>maxBound){maxBound=length;}
            if(length>6)
            {
                avg += length;
            }
        }
        avg = avg/boundCount;
        double stdDev=0;
        for (int n = 0; n < boundCount; n++)
        {
            double length = lengths[n];
            stdDev+=std::pow((length - avg),2);
        }
        stdDev = (stdDev/((double)boundCount));
        stdDev = std::pow(stdDev,0.5);
        // Sort the lengths vector
        std::sort(lengths.begin(),lengths.end());
        double median;
        if(boundCount%2==0)
        {
            median = ((double)(lengths[boundCount/2]+lengths[boundCount/2+1]))/2;
        }
        else{median = lengths[boundCount/2];}

        // Predict the avg boundary length:
        double predBoundLength = (1*m_kT + m_nearestNeighborFactor*1.0)/m_tensionFactor;


        std::cout<<"Max boundary length " << maxBound << std::endl;
        std::cout<<"Average boundary length: " << avg << std::endl;
        std::cout<<"Predicted boundary length: " << predBoundLength << std::endl;
        std::cout<<"Std Dev boundary length: " << stdDev << std::endl;
        std::cout<<"Median boundary length " << median << std::endl;
        std::cout << "______________________________________________________________________________" << std::endl;

    }
    // for time keeping
    m_stepCounter ++;
    auto tBegin = std::chrono::high_resolution_clock::now();


    // Carry out the update element by element, choosing a random one each time
    for (int subStep = 0; subStep < m_stepSize; subStep++)
    {
        // Pick random indices
        int iRand = uniform_int_i(rand_generator);
        int jRand = uniform_int_j(rand_generator);
        int topRand = uniform_2(rand_generator);
        // Update an element
        char result = updateBoardElement(iRand,jRand,topRand);
        // DEBUGprintBoard();
        pointModifyBoundaryBoard(result, iRand,jRand, topRand);

    } 

    // Record time taken for the step, and print average every 1000 steps
    auto tDone = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double,std::milli> tDiff =  tDone-tBegin;
    double tDiffDouble = tDiff.count();
    m_stepDurations[m_stepCounter] = tDiffDouble;

    if ( m_stepCounter % 100 == 0 )
    {
        // get average calculation time
        double avgStepTime;
        for (int i = 0; i < m_stepCounter; i++)
        {
            avgStepTime += m_stepDurations[i];
        }
        avgStepTime = avgStepTime / m_stepCounter;
        //std::cout << "Calculated average over " << m_stepCounter << " steps:" << std::endl;

        //std::cout << "Average step took " << avgStepTime << " milliseconds to calculate." << std::endl;

        //std::cout << "Average update time per triplet/pixel: " << avgStepTime/(m_stepSize) << " ms" << std::endl;
        m_stepCounter = 0;

    }
    
    //printBoardWithBoundary();
}

///////////////////////////DEBUG FUNCTIONS/////////////////////////////////

void Simulation::printDebugBoard()
{
    std::cout << "_________________________DEBUG BOARD_________________________"<< std::endl;
    // TOP
    std::string space = " ";
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
                if(m_debugBoundaryBoard[1][doublej][2*i]=='+')
                {
                    std::cout <<m_debugBoundaryBoard[1][doublej][2*i] <<  " ";
                }
                else
                {
                    std::cout <<m_board[1][doublej/2][i] <<  " ";
                }

                if(i<(m_width - 1))
                {
                    std::cout << m_debugBoundaryBoard[1][doublej][2*i+1] <<  " ";
                }
            }
            std::cout << "\n";
        }
        else
        {
            for (int doublei = 0; doublei < 2*m_width - 1; doublei++)
            {
                std::cout << m_debugBoundaryBoard[1][doublej][doublei] <<  " ";
            }
            std::cout << "\n";
        }
    } 
    std::cout << "\n";

    // BOTTOM
    for (int doublej = 2*m_height-2; doublej >= 0; doublej--)
    {
        std::string space = "";
        for (int spaces = 2*m_height - 2 - doublej; spaces > 0; spaces--)
        {
            space.append(" ");
        }

        std::cout << space;

        if(doublej%2==0) // Sparse row
        {
            for (int i = m_width - 1; i >= 0; i--)
            {
                if(i<(m_width - 1))
                {
                    std::cout << m_debugBoundaryBoard[0][doublej][2*i+1] <<  " ";
                }
                if(m_debugBoundaryBoard[0][doublej][2*i]=='+')
                {
                    std::cout <<m_debugBoundaryBoard[0][doublej][2*i] <<  " ";
                }
                else
                {
                    std::cout <<m_board[0][doublej/2][i] <<  " ";
                }
                //std::cout <<m_board[0][doublej/2][i] <<  " ";

            }
            std::cout << "\n";
        }
        else
        {
            for (int doublei = 2*m_width - 2; doublei >=0; doublei--)
            {
                std::cout << m_debugBoundaryBoard[0][doublej][doublei] <<  " ";
            }
            std::cout << "\n";
        }
    } 
    std::cout << "\n";
}


void Simulation::setDebugBoardElement(int i, int j, bool top, char value)
{
    m_debugBoundaryBoard[top][j][i] = value;
}

void Simulation::setDebugBoardToBoundaryBoard()
{
    for (int top = 1; top>-1; top--)
    {
        for (int i = 0; i < 2*m_width-1; i++)
        {
            for (int j = 0; j < 2*m_height-1; j++)
            {
                m_debugBoundaryBoard[top][j][i] = m_boundaryBoard[top][j][i];
            }
        }
    }   
}

///////////////////////////DEBUG FUNCTIONS/////////////////////////////////

///////////////////////////PARAM UPDATE FUNCTIONS/////////////////////////////////
void Simulation::updateTemp(float value)
{
    m_kT = value;
}
void Simulation::updateTension(float value)
{
    m_tensionFactor = value;
}
void Simulation::updateNearestNeighbor(float value)
{
    m_nearestNeighborFactor = value;
}
void Simulation::updateCrossLayerNearestNeighbor(float value)
{
    m_crossLayerNearestNeighborFactor = value;
}
void Simulation::updateCrossLayerBoundaryInterference(float value)
{
    m_crossLayerBoundaryFactor = value;
}
///////////////////////////PARAM UPDATE FUNCTIONS/////////////////////////////////