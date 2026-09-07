#pragma once
#include "constants.h"
#include <iostream>
#include <vector>

struct edgeCoords
{
    int i;
    int j;
    edgeCoords(int i, int j):i(i),j(j){}
    bool isValid()
    {
        return ((i>-1) && (j>-1) && (j<bBHeight) && (i<bBWidth));
    }
    
    bool equals(edgeCoords edge)
    {
        return ((edge.i==i) && (edge.j==j));
    }
    // Remove the appropriate element and return true if this edge position is in a vector of edge positions
    bool removeIfInVector(std::vector<edgeCoords>& vec)
    {
        for (auto edge=vec.begin();edge!=vec.end(); ++edge)
        {
            edgeCoords e = *edge;
            if((e.i == i) && (e.j == j))
            {
                vec.erase(edge);
                return true;
            }
        }
        return false;
    }
    /*
    Return the orientation of the edge, as judged by someone looking down at the board.
    For the bottom board, we still look from above, and imagine looking through the top board.
        Right-Up facing line: 'R'
        Left-Up  facing line: 'L'
        Vertical line:        'V'
    */
    char orientation()
    {
        if(j%2==0)
        {return 'V';}
        else if(i%2==0)
        {return 'L';}
        else
        {return 'R';}
    }

    /*
    Returns a boolean which allows the next edge-finding step to proceed quickly.
        In short, this logic table allows us to cross off 2 of 4 indices that an edge finding step would otherwise have to check
        in order to proceed along a boundary without double counting any edges.
    chosenEdge indices are defined as follows:
        Vertical Edges:
            0   1
             \ / 
              |
             / \  
            3   2
        Left Up Edges:
              3
              |   0
             / \ /
            2   |
                1
        Right Up Edges:
                0
            3   |
             \ / \
              |   1
              2
    */
    bool getNextEdgeFindingDirection(int chosenEdge)
    {
        char orientation = this->orientation();
        bool nextDir;
        switch(chosenEdge)
        {
            case 0:
                if     (orientation=='V'){nextDir = true;}
                else if(orientation=='L'){nextDir = false;}
                else   /*'R'*/           {nextDir = false;}
                break;
            case 1:
                if     (orientation=='V'){nextDir = false;}
                else if(orientation=='L'){nextDir = true;}
                else   /*'R'*/           {nextDir = false;}
                break;
            case 2:
                if     (orientation=='V'){nextDir = false;}
                else if(orientation=='L'){nextDir = true;}
                else   /*'R'*/           {nextDir = true;}
                break;
            case 3:
                if     (orientation=='V'){nextDir = true;}
                else if(orientation=='L'){nextDir = false;}
                else   /*'R'*/           {nextDir = true;}
                break;

        }
        return nextDir;
    }

    // Get the edge at the given index 0-3
    edgeCoords getAdjEdge(int chosenEdge)
    {
        char orientation = this->orientation();
        int outi=i;
        int outj=j;
        switch(chosenEdge)
        {
            case 0:
                if     (orientation=='V'){outi--; outj++;}
                else if(orientation=='L'){outi++;        }
                else   /*'R'*/           {        outj++;}
                break;
            case 1:
                if     (orientation=='V'){        outj++;}
                else if(orientation=='L'){outi++; outj--;}
                else   /*'R'*/           {outi++;        }
                break;
            case 2:
                if     (orientation=='V'){outi++; outj--;}
                else if(orientation=='L'){outi--;        }
                else   /*'R'*/           {        outj--;}
                break;
            case 3:
                if     (orientation=='V'){        outj--;}
                else if(orientation=='L'){outi--; outj++;}
                else   /*'R'*/           {outi--;        }
                break;
        }
        return edgeCoords(outi,outj);
    }
};
struct elementCoords
{
    int i;
    int j;
    elementCoords(int i, int j):i(i),j(j)
    {}

    bool isValid()
    {
        return ((i>-1) && (j>-1) && (j<height) && (i<width));
    }

    std::vector<edgeCoords> getValidSurroundingEdges()
    {
        // Get the coordinates of this element in the boundary board
        int boundi = 2*i;
        int boundj = 2*j;

        // Assemble all of the neighboring edge indices
        int edgeIndices[6][2] = {   
            {boundj+1,boundi-1},
            {boundj+1,boundi  },
            {boundj  ,boundi+1},
            {boundj-1,boundi+1},
            {boundj-1,boundi  },
            {boundj  ,boundi-1}
        };
        // Put all of the valid edge positions in a vector and return it
        std::vector<edgeCoords> surroundingEdges;
        for (int index=0; index<6; index++)
        {
            edgeCoords edge = edgeCoords(edgeIndices[index][1],edgeIndices[index][0]);
            if(edge.isValid())
            {
                surroundingEdges.push_back(edge);
            }
        }
        return surroundingEdges;
    }
};
struct ScreenPosition
{
    const double x;
    const double y;
};

// Templates
template<typename T>
void p(T val)
{
    std::cout << val << std::endl;
}
