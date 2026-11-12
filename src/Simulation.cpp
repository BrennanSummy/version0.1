#include "Simulation.h"

Simulation::Simulation(double kT, double tensionFactor, double intraLayerNearestNeighbor, double crossLayerComplement, double crossLayerNearestNeighbor, double crossLayerBoundaryFactor)
:rand_device(), rand_generator(rand_device()), uniform_real(0,1), uniform_3(0,2), uniform_2(0,1), uniform_int_checker(0,8), uniform_int_i(min_updating_i,max_updating_i),uniform_int_j(min_updating_j,max_updating_j),
m_kT(kT), m_tensionFactor(tensionFactor), m_nearestNeighborFactor(intraLayerNearestNeighbor), m_crossLayerComplementFactor(crossLayerComplement),m_crossLayerNearestNeighborFactor(crossLayerNearestNeighbor),
m_crossLayerBoundaryFactor(crossLayerBoundaryFactor)
{
    //initBoard();
    //updateBoundaryBoardViaScan();
    reset();
    setDebugBoardToBoundaryBoard();
    //printBoardWithBoundary();
    clearNeighborVals();
    // debug

    
    //// First File Setup
    //double newTemp    = temps            [0];
    //double newNNF     = nearestNeighborFs[0];
    //double newTension = tensionFs        [0];
    //// Update the relevant parameters
    //updateTemp(newTemp);
    //updateNearestNeighbor(newNNF);
    //updateTension(newTension);

    //std::ostringstream osStream;
    //osStream << newTemp << "," << newNNF << "," << newTension;
    //std::string newName = osStream.str();
    //// Create new file, reset board, and unpause
    //resetWithNewName(newName);
    engines = sycl::malloc_device<oneapi::dpl::minstd_rand>((bWidth-noUpdateOffset)*(bHeight-noUpdateOffset),q);
    auto* engs = engines;
    q.submit([&](sycl::handler& cgh) {
    cgh.parallel_for(sycl::range<1>((bWidth-noUpdateOffset)*(bHeight-noUpdateOffset)), [=](sycl::id<1> idx) {
        // Initialize with a base seed plus a unique sequence offset per site
        engs[idx] = oneapi::dpl::minstd_rand(42, idx[0] * 16);
    });
    }).wait();

}



void Simulation::initBoard()
{
    // Easy ~half A quarter B quarter C default initialization
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            //if((j==0 || i==0) || (j==m_height-1 || i == m_width-1))
            if(true)
            {
                set_m_board(i,j,0,m_X[uniform_3(rand_generator)]);
                set_m_board(i,j,1,m_X[uniform_3(rand_generator)]);
                //set_m_board(i,j,0,m_X[0]);
                //set_m_board(i,j,1,m_X[1]);
            }
        }
    } 
    
}

void Simulation::reset()
{
    // Reset the board
    initBoard();
    //loadBoardFromFile(dir+"rand_board_1.csv");
    updateBoundaryBoardViaScan();
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
                char ijValue   = get_m_board(i,j,top);

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
                        char neighborValue = get_m_board(neighbor_i,neighbor_j,top);

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
            //std::cout <<m_board[1][j][i] <<  " ";
            std::cout <<get_m_board(i,j,1) <<  " ";
        }
        std::cout << "\n";
    } 
    std::cout << "\n";
    
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
            std::cout <<get_m_board(i,j,0)<<  " ";
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
                std::cout <<get_m_board(i,doublej/2,1)<<  " ";

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
                std::cout <<get_m_board(i,doublej/2,0)<<  " ";

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
            elementCoords element(i,j);
            // Count neighbors, then see how many are not similar
            updateNeighborVals(element, top);
            char value = get_m_board(i,j,top);

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
        energy += m_nearestNeighborFactor*countDissimNeighbors(top);
    }
    return energy;
}


char Simulation::updateBoardElement(elementCoords element, bool top, int subStep)
{
    updateNeighborVals(element, top);
    updateMatchingComplement(element,top);
    //T//for (int x = 0; x<3;x++)
    //T//{
        //T//updateProspectiveBoundaryLengths(m_X[x],element, top);
    //T//}
    updateProbabilities();
    char result = roll();
    //m_board[top][element.j][element.i] = result;
    set_m_board(element.i,element.j,top,result);
    return result;
}

void Simulation::updateNeighborVals(elementCoords element, bool top)
{
    // List the relevant neighbor indices, starting with the upper left and going clockwise
    std::vector<elementCoords> surroundingElements = element.getValidSurroundingElements();

    
    /* Loop through all of the neighbors:
        - check char value of board[coordinate], return A, B, or C
        - add 1 to the count of A, B, or C respectively
    */
    for (int n = 0; n < surroundingElements.size(); n++)
    {
        // Check the state of the board to get the value of this element
        //char inLayerValue    = m_board[ top][surroundingElements[n].j][surroundingElements[n].i];
        //char crossLayerValue = m_board[!top][surroundingElements[n].j][surroundingElements[n].i];
        char inLayerValue    = get_m_board(surroundingElements[n].i,surroundingElements[n].j,top);
        char crossLayerValue = get_m_board(surroundingElements[n].i,surroundingElements[n].j,!top);
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
        }

    }

    std::vector<elementCoords> secondNearestElements = element.getValidSecondNearestElements();
    for (int n = 0; n < surroundingElements.size(); n++)
    {
        // Check the state of the board to get the value of this element
        char inLayerValue    = get_m_board(surroundingElements[n].i,surroundingElements[n].j,top);
        //char crossLayerValue = m_board[!top][surroundingElements[n].j][surroundingElements[n].i];
        // Increment the appropriate counter in m_inLayerNeighborCounts
        switch (inLayerValue)
        {
            case 'A':
                m_inLayerSecondNeighborCounts[0]++;
                //m_neighborVals[neighbor] = 0;
                break;
            case 'B':
                m_inLayerSecondNeighborCounts[1]++;
                //m_neighborVals[neighbor] = 1;
                break;
            case 'C':
                m_inLayerSecondNeighborCounts[2]++;
                //m_neighborVals[neighbor] = 2;
                break;
        }
        char crossLayerValue    = get_m_board(surroundingElements[n].i,surroundingElements[n].j,!top);
        switch (crossLayerValue)
        {
            case 'A':
                m_crossLayerSecondNeighborCounts[0]++;
                break;
            case 'B':
                m_crossLayerSecondNeighborCounts[1]++;
                break;
            case 'C':
                m_crossLayerSecondNeighborCounts[2]++;
                break;
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

void Simulation::updateMatchingComplement(elementCoords element, bool top)
{
    char complement = get_m_board(element.i,element.j,!top);
    for(int prospValIndex = 0; prospValIndex < 3; prospValIndex++)
    {
        m_matchingComplement[prospValIndex] = (complement==m_X[prospValIndex]);
    }
}

bool Simulation::elementPositionCheck(int i, int j)
{
    return (j >= 0) && (i >= 0) && (j < m_height) && (i < m_width);
}

void Simulation::updateProspectiveBoundaryLengths(char prospectiveValue, elementCoords element, bool top)
{
    //DEBUGstd::cout << "Element (i,j) = (" << i << "," << j << ")" << std::endl;
    //DEBUGstd::cout << "Top? " << top << std::endl;
    //DEBUGstd::cout << "Prospective Value: " << prospectiveValue << std::endl;

    //char currentValue = get_m_board[top][element.j][element.i];

    pointModifyBoundaryBoard(prospectiveValue,element.i,element.j, top);
    // DEBUG
    //setDebugBoardToBoundaryBoard();
    //setDebugBoardElement(2*element.i, 2*element.j, top, '+');
    //printDebugBoard();
    // DEBUG

    int boundaryOverlaps;

    //int length = m_counter.countProspectiveBoundaryLength(prospectiveValue,element,top,&boundaryOverlaps);
    
    //m_prospectiveBoundaryLengths[domainToIntMap.at(prospectiveValue)] = length;

    //m_prospectiveBoundaryOverlapCounts[domainToIntMap.at(prospectiveValue)] = boundaryOverlaps;

    // Revert point modification
    //pointModifyBoundaryBoard(currentValue,element.i,element.j, top);
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
            char neighborValue = get_m_board(indices[neighbor][1],indices[neighbor][0],top);

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

        ///////////////////////////// Second Nearest Neighbor ////////////////////////////
        m_secondNearestEnergy[i] = m_secondNearestNeighborFactor * (m_inLayerSecondNeighborCounts[ip1] + m_inLayerSecondNeighborCounts[ip2]);
        ///////////////////////////// Second Nearest Neighbor ////////////////////////////

        ////////////////////////////////     Tension      ////////////////////////////////
        //T//m_tensionEnergy[i] = m_tensionFactor*m_prospectiveBoundaryLengths[i];
        ////////////////////////////////     Tension      ////////////////////////////////
        
        //////////////////////////////// Cross Layer Immediate Complement ////////////////////////////////
        m_cLComplementEnergy[i] = m_crossLayerComplementFactor * m_matchingComplement[i];
        //////////////////////////////// Cross Layer Immediate Complement ////////////////////////////////

        //////////////////////////////// Cross Layer Nearest Neighbor ////////////////////////////////
        //m_cLNearestEnergy[i] = m_crossLayerNearestNeighborFactor * m_crossLayerNeighborCounts[i];
        m_cLNearestEnergy[i] = m_crossLayerNearestNeighborFactor * (m_crossLayerNeighborCounts[ip1] + m_crossLayerNeighborCounts[ip2]);
        //////////////////////////////// Cross Layer Nearest Neighbor ////////////////////////////////

        //////////////////////////////// Cross Layer Second Nearest Neighbor ////////////////////////////////
        m_cLSecondNearestEnergy[i] = m_crossLayerSecondNearestNeighborFactor * (m_crossLayerSecondNeighborCounts[ip1]+m_crossLayerSecondNeighborCounts[ip2]);
        //////////////////////////////// Cross Layer Second Nearest Neighbor ////////////////////////////////

        //////////////////////////////// Cross Layer Boundary Interference ////////////////////////////////
        m_cLBoundaryInterferenceEnergy[i] = m_crossLayerBoundaryFactor*m_prospectiveBoundaryOverlapCounts[i];
        //////////////////////////////// Cross Layer Boundary Interference ////////////////////////////////


        // Total Energy
        //T//m_energies[i] = (m_nearestEnergy[i] + m_tensionEnergy[i] + m_cLNearestEnergy[i] + m_cLBoundaryInterferenceEnergy[i]);
        m_energies[i] = (m_nearestEnergy[i] + m_secondNearestEnergy[i] + m_cLComplementEnergy[i] + m_cLNearestEnergy[i] + m_cLSecondNearestEnergy[i] + m_cLBoundaryInterferenceEnergy[i]);
    }

    double boltzmann_A = std::exp(-(m_energies[0])/m_kT);
    double boltzmann_B = std::exp(-(m_energies[1])/m_kT);
    double boltzmann_C = std::exp(-(m_energies[2])/m_kT);

    double Z = boltzmann_A + boltzmann_B + boltzmann_C;

    m_probabilities[0] = boltzmann_A / Z;
    m_probabilities[1] = boltzmann_B / Z;
    m_probabilities[2] = boltzmann_C / Z;
    // Reset neighbor counts and boundary lengths
    for (int i=0;i<3;i++)
    {
        m_inLayerNeighborCounts[i]           =0;
        m_inLayerSecondNeighborCounts[i]     =0;
        m_crossLayerNeighborCounts[i]        =0;
        m_crossLayerSecondNeighborCounts[i]  =0;
        m_prospectiveBoundaryLengths[i]      =0;
        m_prospectiveBoundaryOverlapCounts[i]=0;
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
        float smallestEnergy = std::min(m_energies[0],std::min(m_energies[1],m_energies[2]));
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
    if(m_reads%1000==1)
    {
        tBegin = std::chrono::high_resolution_clock::now();
    }



    // Carry out the update element by element, choosing a random one each time
    for (int subStep = 0; subStep < stepSizeInPixelUpdates; subStep++)
    {
        // Pick random indices
        int iRand   = uniform_int_i(rand_generator);
        int jRand   = uniform_int_j(rand_generator);
        int topRand = uniform_2(rand_generator);
        elementCoords randElement(iRand,jRand);
        // Update an element
        char result = updateBoardElement(randElement,topRand, subStep);
        pointModifyBoundaryBoard(result, iRand,jRand, topRand);

    } 

    m_reads ++;
}

void Simulation::randomStepGPU()
{
    if(m_reads%1000==1)
    {
        tBegin = std::chrono::high_resolution_clock::now();
    }



    // Carry out the update checkerboard by checkerboard, choosing a random one each time
    //for (int subStep = 0; subStep < stepSizeInCheckerboardUpdates; subStep++)
    for (int subStep = 0; subStep < 2560; subStep++)
    {
        // Pick random checkerboard
        int cRand = uniform_int_checker(rand_generator);
        int topRand = uniform_2(rand_generator);
        // Update that checkerboard
        updateCheckerboard(cRand,topRand);
    } 
    m_reads ++;
}

void Simulation::updateCheckerboard(int checkerBoardIndex, bool top)
{
    // Any checkerboard will need all of the board data to update. Read only is good.
    // But each individual worker only needs the intra and inter layer 1st and 2nd nearest neighbors.


    {
        //DEBUG
        //top = false;
    // Indexing vars
    const int offset = top * bArea + checkerBoardIndex%3 + (checkerBoardIndex/3)*bWidth+noUpdateOffset*(bWidth+1);
    const int numPerRow = (((bWidth-(1+checkerBoardIndex%3+2*noUpdateOffset)) / 3) + 1);
    const int numRows = (bHeight - (1+checkerBoardIndex/3 +2*noUpdateOffset))/3 + 1;
    const int div = 3*numPerRow;
    const int vertSpace = 3*bWidth;

    //std::uint32_t rand_seed = 666;
    //std::uint32_t rand_seed = gpu_update_seed(rand_generator);
    // Print the device being used
    //std::cout << q.get_device().get_info<sycl::info::device::name>() << "\n";
    // Make a range to iterate over
    //auto rg = sycl::range<1>{2*bArea};
    auto rg = sycl::range<1>{(size_t)numPerRow*numRows};
    // copy the board into buffer
    //sycl::buffer<char,1> buf{m_board,2*bArea};

    char* m_board_ptr = m_board;

    auto* engs = engines;
    // Submit work to queue
    q.submit
    (
        [&](sycl::handler& h)
        {
            //sycl::accessor out{buf,h};
            h.parallel_for
            (
                rg, [=](sycl::item<1> idx)
                {
                    int baseIdx = (3*idx)%(div) + (3*idx / div)*(vertSpace) + offset;
                    //out[baseIdx] = 'C';
                    // NEAREST-NEIGHBOR
                    int nId[6]={baseIdx-1,baseIdx+bWidth-1,baseIdx+bWidth,baseIdx+1,baseIdx-bWidth+1, baseIdx-bWidth};
                    //int nId[2]={baseIdx-1,baseIdx+1};
                    int iNN_A_Count = 0;
                    int iNN_B_Count = 0;
                    int iNN_C_Count = 0;
                    for (int i = 0; i<6; i++)
                    //for (int i = 0; i<2; i++)
                    {
                        //if(out[nId[i]]!='A')
                        if(m_board_ptr[nId[i]]!='A')
                        {
                            iNN_A_Count++;
                        }
                        if(m_board_ptr[nId[i]]!='B')
                        {
                            //out[nId[i]] = 'C';
                            iNN_B_Count++;
                        }
                        if(m_board_ptr[nId[i]]!='C')
                        {
                            iNN_C_Count++;
                        }
                    }
                    float E_iNN_A = 1*iNN_A_Count;
                    float E_iNN_B = 1*iNN_B_Count;
                    float E_iNN_C = 1*iNN_C_Count;

                    // 2nd NEAREST-NEIGHBOR
                    // CROSS LAYER NEAREST-NEIGHBOR
                    // CROSS LAYER 2nd NEAREST-NEIGHBOR
                    // PROBABILITY CALCULATION
                    float E_A = E_iNN_A;
                    float E_B = E_iNN_B;
                    float E_C = E_iNN_C;
                    // kT = 1
                    float kT = 0.5;
                    float B_A = sycl::exp(-E_A/kT);
                    float B_B = sycl::exp(-E_B/kT);
                    float B_C = sycl::exp(-E_C/kT);
                    float Z = B_A + B_B + B_C;
                    float P_A = B_A/Z;
                    float P_B = B_B/Z;
                    float P_C = B_C/Z;
                    // ROLL
                    // Random number offset
                    std::uint64_t offset = idx.get_linear_id();

                    // Random number engine
                    //oneapi::dpl::minstd_rand engine(rand_seed, offset);
                    oneapi::dpl::minstd_rand& engine = engs[idx];
                    // Uniform real distribution from 0 to 1
                    oneapi::dpl::uniform_real_distribution<float> unif;

                    float r = unif(engine);
                    ///*
                    if(r < P_A)
                    {
                        m_board_ptr[baseIdx] = 'A';
                    }
                    else if(P_A <= r && r < P_A + P_B)
                    {
                        m_board_ptr[baseIdx] = 'B';
                    }
                    else if(P_A + P_B <= r)
                    {
                        m_board_ptr[baseIdx] = 'C';
                    }
                    //*/
                }
            );
        }
    );
    }
    
    //sycl::host_accessor result{buf};

    //for (int i = 0; i < bArea; i++)
    //{
        //m_board[i] = result[i];
    //}

    //printBoard();
}

char Simulation::get_m_board(int i, int j, bool top)
{
    int idx = j*bWidth + i + bArea*top;
    return m_board[idx];
}
void Simulation::set_m_board(int i, int j, bool top, char val)
{
    int idx = j*bWidth + i + bArea*top;
    m_board[idx] = val;
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
                    std::cout << get_m_board(i,doublej/2,1) <<  " ";
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
                    std::cout << get_m_board(i,doublej/2,0) <<  " ";
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
void Simulation::updateSecondNearestNeighbor(float value)
{
    m_secondNearestNeighborFactor = value;
}
void Simulation::updateCrossLayerComplement(float value)
{
    m_crossLayerComplementFactor = value;
}
void Simulation::updateCrossLayerNearestNeighbor(float value)
{
    m_crossLayerNearestNeighborFactor = value;
}
void Simulation::updateCrossLayerSecondNearestNeighbor(float value)
{
    m_crossLayerSecondNearestNeighborFactor = value;
}
void Simulation::updateCrossLayerBoundaryInterference(float value)
{
    m_crossLayerBoundaryFactor = value;
}
///////////////////////////PARAM UPDATE FUNCTIONS/////////////////////////////////

void Simulation::loadBoardFromFile(std::string filename)
{
    // init file
    std::ifstream f(filename);
    std::string line;
    int i;
    // read the file row by row (line by line)
    for (int top = 0; top < 2; top++)
    {
        for (int j = 0; j < bHeight; j++)
        {
            i=0;
            // load the line string
            std::getline(f,line); 
            if(line[0] == '-')
            {
                // this effectively skips the line between layers
                j--;
            }
            for (char elem : line)
            {
                if(elem=='A' || elem=='B' || elem=='C')
                {
                    set_m_board(i,j,top,elem);
                    i++;
                }
            }


        }
    }
}
//void Simulation::logAllCounts(int subStep)
//{
//    bool write = false;
//    std::ostringstream osStream;
//    osStream << subStep;
//    for (int i = 0; i<3; i++)
//    {
//        short neighborCount = m_inLayerNeighborCounts[i];
//        int prospBoundLength =  m_prospectiveBoundaryLengths[i];
//
//        if(prospBoundLength !=0 && prospBoundLength !=6 && prospBoundLength != 4)
//        {
//            write = true;
//        }
//
//        osStream << "," << neighborCount << "," << prospBoundLength;
//    }
//    std::string counts = osStream.str();
//    if(write)
//    {
//        m_fileWriter.insertLine(counts);
//    }
//}
//void Simulation::initLogFile(std::string newFileName)
//{
//    if(newFileName!="none")
//    {
//        m_fileWriter.setNewFileName(newFileName);
//        m_fileWriter.createFile();
//    }
//
//    std::ostringstream osStream;
//    osStream << "width:" << bWidth << "\n"
//             << "height:"<< bHeight<< "\n"
//             << "temp:"  << m_kT   << "\n"
//             << "NNF:"   << m_nearestNeighborFactor << "\n"
//             << "TenF:"  << m_tensionFactor << "\n";
//    std::string metaData = osStream.str();
//    m_fileWriter.insertLine(metaData);
//    //std::string line = "Number,Max,Average,Std_Dev,Median,Number_Over_6,AverageOver6,Std_Dev_Over6";
//    //m_fileWriter.insertLine(line);
//}
//
//void Simulation::writeToLog(int num, int numOver6, int max, double avg, double avgWO6, double stdDev, double stdDevWO6, double median)
//{
//    std::string header = "Number,Max,Average,Std_Dev,Median,Number_Over_6,AverageOver6,Std_Dev_Over6";
//    m_fileWriter.insertLine(header);
//    std::ostringstream osStream;
//    osStream << num << ","  << max << "," << avg << "," << stdDev << "," << median << "," << numOver6 << "," << avgWO6 << "," << stdDevWO6;
//    std::string line = osStream.str();
//    m_fileWriter.insertLine(line);
//}