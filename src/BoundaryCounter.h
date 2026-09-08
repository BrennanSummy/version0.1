#pragma once
#include "constants.h"
#include "structs.h"

class BoundaryCounter
{
private:

    // Dimensions
    const size_t* m_bWidth;
    const size_t* m_bHeight;
    const size_t* m_bBWidth;
    const size_t* m_bBHeight;

    // Pointer to the board
    const char (&board)[2][height][width];

    // Pointer to the boundary board
    const char (&boundaryBoard)[2][bBHeight][bBWidth];

    // A board the size of one side of the simulation element board,
    // but stored here and used for checking off elements
    char m_checkBoard[height][width];

    // Top (1) or bottom (0) layer of the board
    bool m_top;

    // Length(s) (in the case of non-contiguous doublets or triplets, there will be multiple boundaries)
    std::vector<int> lengths;


    bool edgeIsCompatibleWithValue(edgeCoords edge, char value);

    void explore(std::vector<edgeCoords>& remainingInitEdges, edgeCoords startEdge, edgeCoords edge, bool direction, char value, int* length, bool* loop);

    void exploreLineNonContiguous(std::vector<edgeCoords>& remainingInitEdges, edgeCoords startEdge, edgeCoords edge,
                              bool direction, char value, int* length, bool* loop,
                              std::vector<edgeCoords>& visitedEdges);

    void setTop(bool top);

    void resetCheckBoard();

    void checkOffElement(elementCoords element);

    bool checkElement(elementCoords element);

public:
    BoundaryCounter(char (&board)[2][height][width], char (&boundaryBoard)[2][bBHeight][bBWidth],
                    const size_t* m_bWidth, const size_t* m_bHeight, const size_t* m_bBWidth, const size_t* m_bBHeight
                    );
    ~BoundaryCounter();
    // Fast boundary counter for prospective boundary counting (REQUIRES THE RESPECTIVE BOUNDARY BOARD POINT MODIFICATION TO HAVE BEEN DONE ALREADY)
    int countProspectiveBoundaryLength(char value, elementCoords element, bool top, int* boundaryOverlaps);

    std::vector<int> countAllBoundaryLengths(elementCoords element, bool top);

    std::vector<int> boundaryScan(bool top);
};

/* IDEA FOR BOUNDARY SCANNER */
/*
FAST METHOD: USED FOR UPDATING PROBABILITIES. FOLLOWS ALL BOUNDARIES CONNECTED TO THE STARTING ELEMENT.
Start with an element: Check all surrounding edges and store their coordinates. Choose one to move down.
    Remove the moved-down edge coordinates from the storage container.
    increment the length counter

    if the length of the current boundary has not been exhausted:
        move down the boundary by one edge.

    else:
        if the other direction from the starting edge has not been explored:
            start moving down the initial edge in the other direction.
        else:
            Check if there is anything left in the edge coordinate storage container.
            If there is, start near the top with the first remaining edge as the starting edge.

Utility functions:
    1. Get the edge positions (vector) surrounding an element position
    2. Check if an edge type is compatible with given value
        bool isEdgeCompatible(char value)
    3. Get the edge positions that are adjacent to a given edge position and in a certain direction


SLOW SCAN METHOD: USED FOR COUNTING THE LENGTHS OF ALL BOUNDARIES ON THE BOARD WITHOUT SKIPPING OR OVERCOUNTING ANY
Start with an element: Check all surrounding edges and store their coordinates. Choose one to move down.
    Remove the moved-down edge coordinates from the storage container.
    if the new boundary is attached to new element:
        check all element-surrounding edges and store their coordinates (don't keep multiples of coordinates)
    increment the length counter
    if the length of the current boundary has not been exhausted:
        move down the boundary by one edge.
    else:
        Check if there is anything left in the edge coordinate storage container. If there is, this means that
        there was at least one true doublet or triplet in this contiguous element clump.
        Start a new length counter.
        Start near the top, choosing the first unchecked edge as the starting point.
*/