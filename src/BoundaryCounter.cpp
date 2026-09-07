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