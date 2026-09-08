#include "BoundaryCounter.h"

BoundaryCounter::BoundaryCounter(char (&board)[2][height][width], char (&boundaryBoard)[2][bBHeight][bBWidth],
                    const size_t* m_bWidth, const size_t* m_bHeight, const size_t* m_bBWidth, const size_t* m_bBHeight)
:board(board),boundaryBoard(boundaryBoard),m_bWidth(m_bWidth),m_bHeight(m_bHeight),m_bBWidth(m_bBWidth),m_bBHeight(m_bBHeight)
{
}

BoundaryCounter::~BoundaryCounter()
{
}

void BoundaryCounter::setTop(bool top)
{
    m_top = top;
}
int BoundaryCounter::countProspectiveBoundaryLength(char value, elementCoords element, bool top, int* boundaryOverlaps)
{
    setTop(top);
    //// Get the surrounding edge positions and narrow them down to those which have boundary edges ////
    std::vector<edgeCoords> remainingInitEdges = element.getValidSurroundingEdges();
    // This is the number of edges which are on the board (6 unless this element is a corner or edge element)
    int numberOfValidEdges = remainingInitEdges.size();

    auto iterator = remainingInitEdges.begin();
    while(iterator != remainingInitEdges.end())
    {
        // Dereferencing the iterator gives the edge object at that element of the vector
        if(!edgeIsCompatibleWithValue(*iterator,value))
        {
            // Erase the element if it is not compatible (if it is not a boundary at all)
            iterator = remainingInitEdges.erase(iterator);
        }
        else{iterator++;}
    }
    // Count the number of edges around this element that overlap with edges on the other side of the board
    int count = 0;
    iterator = remainingInitEdges.begin();
    while(iterator != remainingInitEdges.end())
    {
        edgeCoords e = *iterator;
        if(boundaryBoard[m_top][e.j][e.i] == boundaryBoard[!m_top][e.j][e.i])
        {
            count++;
        }
        iterator++;
    }
    *boundaryOverlaps=count;

    // If this element is surrounded by similar elements, there are no boundaries to count.
    if(remainingInitEdges.empty()){return 0;}

    // If this element is fully surrounded by dissimilar elements, return that number.
    if(remainingInitEdges.size()==numberOfValidEdges){return numberOfValidEdges;}

    //// Now iterate through the edges, exploring all contiguous boundaries ////
    int length = 1;
    int edgeIndex = -1;
    while(remainingInitEdges.size()!=0)
    {
        edgeIndex = (edgeIndex+1)%remainingInitEdges.size();
        edgeCoords initEdge = remainingInitEdges[edgeIndex];
        // Reset the loop check
        bool loop = false;
        for (int initDirection = 0; initDirection<2; initDirection++)
        {
            explore(remainingInitEdges, initEdge,  initEdge, initDirection, value, &length, &loop);
            if(loop)
            {
                break;
            }
        }
    }
    return length;
}

/*
Explore a boundary edge by edge. Starts at startEdge, moves forward recursively, crossing off any remainingInitEdges on the way.
Only boundary types compatible with 'value' are considered. Increments length with each recursion, and sets loop to true if startEdge
is found again by completing a loop.
*/
void BoundaryCounter::explore(std::vector<edgeCoords>& remainingInitEdges, edgeCoords startEdge, edgeCoords edge, bool direction, char value, int* lengthPtr, bool* loop)
{
    // Remove this edge if it is in the remainingInitEdges vector
    edge.removeIfInVector(remainingInitEdges);
    
    // Look at edge neighbors 0,1 if direction is 0, or 2,3 if direction is 1
    for (int edgeInd = direction*2; edgeInd < direction*2 + 2; edgeInd++)
    {
        // If there is a valid compatible edge at the given edgeIndex, choose that edge, increment length, and recurse
        edgeCoords prospectiveEdge = edge.getAdjEdge(edgeInd);
        if(edgeIsCompatibleWithValue(prospectiveEdge,value))
        {
            // Break if this closes a loop
            if(prospectiveEdge.equals(startEdge)){*loop=true; break;}
            // Increment the length, then recurse
            *lengthPtr = *lengthPtr + 1;
            //DEBUG
            //int l = *lengthPtr;
            //std::cout<<"Length: " << l<< std::endl;
            explore(remainingInitEdges,startEdge,prospectiveEdge,edge.getNextEdgeFindingDirection(edgeInd),value,lengthPtr,loop);
            break;
        }
    }

}

std::vector<int> BoundaryCounter::countAllBoundaryLengths(elementCoords element, bool top)
{
    setTop(top);
    checkOffElement(element);
    char value = board[top][element.j][element.i];
    std::vector<edgeCoords> remainingEdges = element.getValidSurroundingEdges();
    int numberOfValidEdges = remainingEdges.size();
    // Throw out edges that aren't boundaries
    auto iterator = remainingEdges.begin();
    while(iterator != remainingEdges.end())
    {
        // Dereferencing the iterator gives the edge object at that element of the vector
        if(!edgeIsCompatibleWithValue(*iterator,value))
        {
            // Erase the element if it is not compatible (if it is not a boundary at all)
            iterator = remainingEdges.erase(iterator);
        }
        else{iterator++;}
    }

    // If this element is surrounded by similar elements, there are no boundaries to count.
    if(remainingEdges.empty()){return std::vector<int> {0};}

    // If this element is fully surrounded by dissimilar elements, return that number.
    if(remainingEdges.size()==numberOfValidEdges){return std::vector<int> {numberOfValidEdges};}

    //// Now iterate through the edges, exploring all boundaries, including noncontiguous but adjacent ones ////
    std::vector<int> lengths;
    int numberOfNonContiguous = 0;
    int edgeIndex = -1;
    std::vector<edgeCoords> visitedEdges;
    while(remainingEdges.size()!=0)
    {
        lengths.push_back(1);
        edgeIndex = (edgeIndex+1)%remainingEdges.size();
        edgeCoords initEdge = remainingEdges[edgeIndex];
        // Reset the loop check
        bool loop = false;
        for (int initDirection = 0; initDirection<2; initDirection++)
        {
            exploreLineNonContiguous(remainingEdges, initEdge,  initEdge, initDirection, value, &lengths[numberOfNonContiguous],
                                     &loop, visitedEdges);
            if(loop)
            {
                break;
            }
        }
        numberOfNonContiguous++;
    }
    return lengths;
}

void BoundaryCounter::exploreLineNonContiguous(std::vector<edgeCoords>& remainingEdges, edgeCoords startEdge,
                                           edgeCoords edge, bool direction, char value, int* lengthPtr,
                                           bool* loop, 
                                           std::vector<edgeCoords>& visitedEdges)
{
    edge.addIfNotInVector(visitedEdges);
    // Remove this edge if it is in the remainingInitEdges vector
    edge.removeIfInVector(remainingEdges);

    // Check the relevant board element for noncontiguous adjacent boundaries
    std::unique_ptr<std::vector<elementCoords>> adjElems = edge.getAdjElements();
    // Choose the element that has the same char value
    bool elementSelector;
    if(board[m_top][(*adjElems)[0].j][(*adjElems)[0].i]==value){elementSelector=0;} else{elementSelector=1;}
    checkOffElement((*adjElems)[elementSelector]);
    // Add any novel edges to the remainingEdges vector
    std::vector<edgeCoords> candidateEdges = (*adjElems)[elementSelector].getValidSurroundingEdges();
    auto iterator = candidateEdges.begin();
    while(iterator != candidateEdges.end())
    {
        // Dereferencing the iterator gives the edge object at that element of the vector
        edgeCoords e = *iterator;
        if(edgeIsCompatibleWithValue(e,value)&&!e.isInVector(visitedEdges))
        {
            e.addIfNotInVector(remainingEdges);
        }
        iterator++;
    }

    
    // Look at edge neighbors 0,1 if direction is 0, or 2,3 if direction is 1
    for (int edgeInd = direction*2; edgeInd < direction*2 + 2; edgeInd++)
    {
        // If there is a valid compatible edge at the given edgeIndex, choose that edge, increment length, and recurse
        edgeCoords prospectiveEdge = edge.getAdjEdge(edgeInd);
        if(edgeIsCompatibleWithValue(prospectiveEdge,value))
        {
            // Break if this closes a loop
            if(prospectiveEdge.equals(startEdge)){*loop=true; break;}
            // Increment the length, then recurse
            *lengthPtr = *lengthPtr + 1;
            //DEBUG
            //int l = *lengthPtr;
            //std::cout<<"Length: " << l<< std::endl;
            exploreLineNonContiguous(remainingEdges,startEdge,prospectiveEdge,edge.getNextEdgeFindingDirection(edgeInd),
                                 value,lengthPtr,loop,visitedEdges);
            break;
        }
    }

}

// Sets every element of the checkBoard to 'O'
void BoundaryCounter::resetCheckBoard()
{
    for (int j = 0; j<*m_bHeight; j++)
    {
        for (int i = 0; i<*m_bWidth; i++)
        {
            m_checkBoard[j][i] = 'O';
        }
    }
}
// Sets the given element of the checkBoard to 'X'
void BoundaryCounter::checkOffElement(elementCoords element)
{
    m_checkBoard[element.j][element.i] = 'X';
}
bool BoundaryCounter::checkElement(elementCoords element)
{
    if(m_checkBoard[element.j][element.i]=='X')
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<int> BoundaryCounter::boundaryScan(bool top)
{
    std::vector<int> allLengths;
    for (int j = 0; j<*m_bHeight; j++)
    {
        for (int i = 0; i<*m_bWidth; i++)
        {
            elementCoords elem = elementCoords(i,j);
            // If the element is not yet checked off
            if(!checkElement(elem))
            {
                std::vector<int> lengths = countAllBoundaryLengths(elem, top);
                //std::cout<<"Number of boundaries found from this element: "<<lengths.size()<<std::endl;
                for (int n =0; n<lengths.size(); n++)
                {
                    if(lengths[n]!=0)
                    {
                        //std::cout<<"this length: " << lengths[n]<<std::endl;
                        allLengths.push_back(lengths[n]);
                    }
                }
                lengths.clear();
            }

        }
    }
    resetCheckBoard();
    return allLengths;
}

// Test the edge value at the given coordinates to see if it is compatible with the given value (A <-> DE, B <-> DF, C <-> EF)
bool BoundaryCounter::edgeIsCompatibleWithValue(edgeCoords edge, char value)
{
    if(edge.isValid())
    {
        char edgeValue = boundaryBoard[m_top][edge.j][edge.i];

        switch(value)
        {
            case 'A':
                if(edgeValue=='D'||edgeValue=='E'){return true;}
                break;
            case 'B':
                if(edgeValue=='D'||edgeValue=='F'){return true;}
                break;
            case 'C':
                if(edgeValue=='E'||edgeValue=='F'){return true;}
                break;
        }
    }
    return false;
}