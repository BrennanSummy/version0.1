#include "Simulation.h"

Simulation::Simulation(double kT, double tensionFactor, double intraLayerNearestNeighbor, double crossLayerNearestNeighbor, double crossLayerBoundaryFactor)
:rand_device(), rand_generator(rand_device()), uniform_real(0,1), uniform_3(0,2), uniform_2(0,1), uniform_int_i(0,m_width-1),uniform_int_j(0,m_height-1),
m_kT(kT), m_tensionFactor(tensionFactor), m_nearestNeighborFactor(intraLayerNearestNeighbor), m_crossLayerNearestNeighborFactor(crossLayerNearestNeighbor),
m_crossLayerBoundaryFactor(crossLayerBoundaryFactor),m_counter(BoundaryCounter(m_board,m_boundaryBoard,&bWidth,&bHeight,&bBWidth,&bBWidth)),
m_fileWriter("defaultFile")

{
    initBoard();
    updateBoundaryBoardViaScan();
    setDebugBoardToBoundaryBoard();
    printBoardWithBoundary();
    clearNeighborVals();
    
    // First File Setup
    double newTemp    = temps            [0];
    double newNNF     = nearestNeighborFs[0];
    double newTension = tensionFs        [0];
    // Update the relevant parameters
    updateTemp(newTemp);
    updateNearestNeighbor(newNNF);
    updateTension(newTension);

    std::ostringstream osStream;
    osStream << newTemp << "," << newNNF << "," << newTension;
    std::string newName = osStream.str();
    // Create new file, reset board, and unpause
    resetWithNewName(newName);

}

void Simulation::initLogFile(std::string newFileName)
{
    if(newFileName!="none")
    {
        m_fileWriter.setNewFileName(newFileName);
        m_fileWriter.createFile();
    }

    std::ostringstream osStream;
    osStream << "width:" << bWidth << "\n"
             << "height:"<< bHeight<< "\n"
             << "temp:"  << m_kT   << "\n"
             << "NNF:"   << m_nearestNeighborFactor << "\n"
             << "TenF:"  << m_tensionFactor << "\n";
    std::string metaData = osStream.str();
    m_fileWriter.insertLine(metaData);
    //std::string line = "Number,Max,Average,Std_Dev,Median,Number_Over_6,AverageOver6,Std_Dev_Over6";
    //m_fileWriter.insertLine(line);
}

void Simulation::writeToLog(int num, int numOver6, int max, double avg, double avgWO6, double stdDev, double stdDevWO6, double median)
{
    std::string header = "Number,Max,Average,Std_Dev,Median,Number_Over_6,AverageOver6,Std_Dev_Over6";
    m_fileWriter.insertLine(header);
    std::ostringstream osStream;
    osStream << num << ","  << max << "," << avg << "," << stdDev << "," << median << "," << numOver6 << "," << avgWO6 << "," << stdDevWO6;
    std::string line = osStream.str();
    m_fileWriter.insertLine(line);
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

void Simulation::resetWithNewName(std::string newName)
{
    // Reset the board
    initBoard();
    updateBoundaryBoardViaScan();
    
    // Set a new file name
    initLogFile(newName);

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
            elementCoords element(i,j);
            // Count neighbors, then see how many are not similar
            updateNeighborVals(element, top);
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

void Simulation::logAllCounts(int subStep)
{
    bool write = false;
    std::ostringstream osStream;
    osStream << subStep;
    for (int i = 0; i<3; i++)
    {
        short neighborCount = m_inLayerNeighborCounts[i];
        int prospBoundLength =  m_prospectiveBoundaryLengths[i];

        if(prospBoundLength !=0 && prospBoundLength !=6 && prospBoundLength != 4)
        {
            write = true;
        }

        osStream << "," << neighborCount << "," << prospBoundLength;
    }
    std::string counts = osStream.str();
    if(write)
    {
        m_fileWriter.insertLine(counts);
    }
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
    m_board[top][element.j][element.i] = result;
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
        char inLayerValue    = m_board[ top][surroundingElements[n].j][surroundingElements[n].i];
        char crossLayerValue = m_board[!top][surroundingElements[n].j][surroundingElements[n].i];
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
    char complement = m_board[!top][element.j][element.i];
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
        //T//m_tensionEnergy[i] = m_tensionFactor*m_prospectiveBoundaryLengths[i];
        ////////////////////////////////     Tension      ////////////////////////////////

        //////////////////////////////// Cross Layer Nearest Neighbor ////////////////////////////////
        m_cLNearestEnergy[i] = m_crossLayerNearestNeighborFactor * m_matchingComplement[i];
        //////////////////////////////// Cross Layer Nearest Neighbor ////////////////////////////////

        //////////////////////////////// Cross Layer Boundary Interference ////////////////////////////////
        m_cLBoundaryInterferenceEnergy[i] = 100*m_crossLayerBoundaryFactor*m_prospectiveBoundaryOverlapCounts[i];
        //////////////////////////////// Cross Layer Boundary Interference ////////////////////////////////


        // Total Energy
        //T//m_energies[i] = (m_nearestEnergy[i] + m_tensionEnergy[i] + m_cLNearestEnergy[i] + m_cLBoundaryInterferenceEnergy[i]);
        m_energies[i] = (m_nearestEnergy[i] + m_cLNearestEnergy[i] + m_cLBoundaryInterferenceEnergy[i]);
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
    // Logging / data keeping
    /*
    // THIS SHOULD BE  % 100 == 0 FOR NORMAL FUNCTION
    if ( m_stepCounter % scanBoundsPeriodInSteps == 0 )
    {
        // save snapshot every 10 reads
        if ( m_reads%grabScreenPeriodInReads == 0)
        {
            emit saveSnapshot("logs/bulk/pics/" + m_fileWriter.getName() + "_" + std::to_string(m_reads));
        }
        // Should a new file be started?
        if( m_reads == readsPerFile )
        {
            // Pause the simulation
            emit toggleStartStop();


            // Get the parameters for this upcoming file
            fileCount++;
            int tSize =std::size(tensionFs);
            int nSize =std::size(nearestNeighborFs);

            double newTemp    = temps            [fileCount/(tSize*nSize)];
            double newNNF     = nearestNeighborFs[(fileCount/(tSize))%nSize];
            double newTension = tensionFs        [fileCount%tSize];
            // Update the relevant parameters
            updateTemp(newTemp);
            updateNearestNeighbor(newNNF);
            //updateTension(newTension);

            std::ostringstream osStream;
            osStream << newTemp << "," << newNNF << "," << newTension;
            std::string newName = osStream.str();
            // Create new file, reset board, and unpause
            resetWithNewName(newName);

            // Play
            emit toggleStartStop();

            // Reset counter
            m_reads = 0;
            m_stepCounter = 0;
        }
        // Print energy
        //std::cout << "______________________________________________________________________________" << std::endl;
        //std::cout << "Energy: " << getConfigurationEnergy() << std::endl;
        std::cout << "Read Count: " << m_reads << std::endl;


        std::vector<int> lengths = m_counter.boundaryScan(1);
        int boundCount = lengths.size();
        int boundsOver6 = 0;
        //std::cout<< "All boundaries found in top layer: " << boundCount << std::endl;
        // True average boundary length
        double avg=0;
        // Average excluding all thermal blips (6 long boundaries)
        double avgWO6=0;
        int maxBound=0;
        for (int n = 0; n < boundCount; n++)
        {
            int length = lengths[n];
            avg += length;
            if(length>maxBound){maxBound=length;}
            if(length>6)
            {
                avgWO6 += length;
                boundsOver6++;
            }
        }
        avgWO6 = avgWO6/boundsOver6;
        avg = avg/boundCount;
        double stdDev   =0;
        double stdDevWO6=0;
        for (int n = 0; n < boundCount; n++)
        {
            double length = lengths[n];
            stdDev+=std::pow((length - avg),2);
            if(length>6)
            {
                stdDevWO6+=std::pow((length - avgWO6),2);
            }
        }
        stdDev = (stdDev/((double)boundCount));
        stdDev = std::pow(stdDev,0.5);
        stdDevWO6 = (stdDevWO6/((double)boundsOver6));
        stdDevWO6 = std::pow(stdDevWO6,0.5);
        // Sort the lengths vector
        std::sort(lengths.begin(),lengths.end());
        double median;
        if(boundCount==0)
        {
            median = 0;
        }
        else if(boundCount%2==0)
        {
            median = ((double)(lengths[boundCount/2-1]+lengths[boundCount/2]))/2;
        }
        else{median = lengths[boundCount/2];}

        // Predict the avg boundary length:
        double predBoundLength = (1*m_kT + m_nearestNeighborFactor*1.0)/m_tensionFactor;

        writeToLog(boundCount,boundsOver6,maxBound,avg,avgWO6,stdDev,stdDevWO6,median);

        //std::string header = "subStep,ANeighbor,ALength,BNeighbor,BLength,CNeighbor,CLength";

        //m_fileWriter.insertLine(header);
        //std::cout<<"Max boundary length " << maxBound << std::endl;
        //std::cout<<"Average boundary length: " << avg << std::endl;
        //std::cout<<"Average boundary length excluding 6s: " << avgWO6 << std::endl;
        //std::cout<<"Predicted boundary length: " << predBoundLength << std::endl;
        //std::cout<<"Std Dev boundary length: " << stdDev << std::endl;
        //std::cout<<"Median boundary length " << median << std::endl;
        //std::cout << "______________________________________________________________________________" << std::endl;
        // Increment reads
        m_reads++;
    }
    // for time keeping
    m_stepCounter ++;
    //auto tBegin = std::chrono::high_resolution_clock::now();
    */


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

    // Record time taken for the step, and print average every 1000 steps
    //auto tDone = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double,std::milli> tDiff =  tDone-tBegin;
    //double tDiffDouble = tDiff.count();
    //m_stepDurations[m_stepCounter % scanBoundsPeriodInSteps] = tDiffDouble;

    //if ( m_stepCounter % scanBoundsPeriodInSteps == 0 )
    //{
    //    // get average calculation time
    //    double avgStepTime;
    //    for (int i = 0; i < m_stepCounter%scanBoundsPeriodInSteps; i++)
    //    {
    //        avgStepTime += m_stepDurations[i];
    //    }
    //    avgStepTime = avgStepTime / scanBoundsPeriodInSteps;
    //    //std::cout << "Calculated average over " << m_stepCounter << " steps:" << std::endl;

    //    std::cout << "Average step took " << avgStepTime << " milliseconds to calculate." << std::endl;

    //    //std::cout << "Average update time per triplet/pixel: " << avgStepTime/(m_stepSize) << " ms" << std::endl;
    //    //m_stepCounter = 0;

    //}
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