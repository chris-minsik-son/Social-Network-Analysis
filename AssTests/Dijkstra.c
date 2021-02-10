//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////    COMP2521: Assignment 2    ////////////////////////////
////////////////////////////    Social Network Analysis   ////////////////////////////
////////////////////////////    Min-Sik Son (z5310901)    ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Dijkstra ADT implementation ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////// INCLUDED HEADER FILES AND LIBRARIES ////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#include "PQ.h"
#include "Dijkstra.h"
#include "Graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// #DEFINES HERE ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#define INF 2147483647

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// HELPER FUNCTIONS FOR DJIKSTRA.C //////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
static void freePredList(PredNode *curr);
static PredNode *createPredNode(Vertex v);
static PredNode *insertPredNode(PredNode *predHead, PredNode *addNode);

/* From Djikstra.h, putting it here so I can see the struct in this file:

typedef struct ShortestPaths {
	int numNodes;    // The number of vertices in the graph
	
	Vertex src;      // The source vertex
	
	int *dist;       // An  array  of distances from the source vertex -
	                 // one for each vertex (the distance  from  src  to
	                 // itself is 0)
	                 // dist[v] contains the distance from src to v
	                 
	PredNode **pred; // An  array  of  predecessor  lists - one for each
	                 // vertex
	                 // pred[v] contains the predecessor list for vertex
	                 // v
} ShortestPaths;

*/

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Part-1: Dijkstra's algorithm ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// The following code is influenced by the pseudocode in John Shepherd's lecture slide,
// however, a few changes were made
// This was from COMP2521 20T2 ♢ Shortest Path [6/10] slides and youtube video

// The  function  returns  a 'ShortestPaths' structure with the required information:
// - the number of vertices in the graph
// - the source vertex
// - distance array
// - array of predecessor lists

 ShortestPaths dijkstra(Graph g, Vertex src) {
    // Let us initialise the shortest path:
    ShortestPaths sPath;

    // Update number of vertices in the graph
    sPath.numNodes = GraphNumVertices(g);
    
    // Initialising source vertex
    sPath.src = src;

    // Now, for the distance array we need to set all indexes to infinity since they
    // have not been reached yet *These will be updated during the progress
    sPath.dist = malloc(sPath.numNodes * sizeof(int));
    Vertex a = 0;
    while (a < sPath.numNodes) {
        sPath.dist[a] = INF;
        a++;
    }
    sPath.dist[src] = 0;

    // Now, for the predecessor array, according to the pseudocode we usually set 
    // to -1 but due to the struct type we will set to NULL for this one
    sPath.pred = malloc(sPath.numNodes * sizeof(PredNode *));
    Vertex b = 0;
    while (b < sPath.numNodes) {
        sPath.pred[b] = NULL;
        b++;
    }

    // Create a vSet and this will contain all the vertices of the Graph g
    PQ vSet = PQNew();
    Vertex c = 0;
    while (c < sPath.numNodes) {
        PQInsert(vSet, c, sPath.dist[c]);
        c++;
    }

    while (!PQIsEmpty(vSet)) {
        // Always dequeue the item with smallest priority value
        // In other words, this represents the shortest distance node
        Vertex v = PQDequeue(vSet);

        // After we dequeue this item, we will investigate its neighbours
        AdjList currList = GraphOutIncident(g, v);
        
        while (currList != NULL) {
        // Edge relax if necessary here:
        // If the new path we observe is shorter, we make the newPath the currPath
        // In other words, replace the current path with the new one we found

        // Basically, we are updating the cost of the new path here
        // If the distance of the vertex we dequeued + weight of neighbour is less than the current
        // distance of the neighbour:
        
        // The following code in the if statements have been influenced from the source below:
        // https://towardsdatascience.com/algorithm-shortest-paths-1d8fa3f50769
        // This site provided me more of a graphical representation of edge relaxation
        if (sPath.dist[v] + currList->weight < sPath.dist[currList->v] && sPath.dist[v] != INF) {
            sPath.dist[currList->v] = sPath.dist[v] + currList->weight;
            // Now afterwards, we have to free the previous predecessor list since we
            // replaced with a new path
            freePredList(sPath.pred[currList->v]);
            sPath.pred[currList->v] = insertPredNode(NULL, createPredNode(v));

        } else if (sPath.dist[v] + currList->weight == sPath.dist[currList->v]) {
            // If the new path we look at has the same path length (shortest path also), then we will 
            // insert to the predecessor list
            sPath.pred[currList->v] = insertPredNode(sPath.pred[currList->v], createPredNode(v));

        }
            // Update priority queue and increment to the next neighbour and repeat
            PQUpdate(vSet, currList->v, sPath.dist[currList->v]);
            currList = currList->next;
        }
    }

    // Using the function from PQ.h we will now free the vSet to free any memory associated with
    // the  priority queue
    PQFree(vSet);

    // At the very end, what we need to do is check for any vertices that we could not reach
    // Since we initialised all to infinity earlier, we just need to set them as 0
    int q = 0;
    while (q < sPath.numNodes) {
        if (sPath.dist[q] == INF) {
            sPath.dist[q] = 0;
            q++;
        } else {
            q++;
        }
    }

    return sPath;
 }

 // This  function  is  for  you to print out the ShortestPaths structure
 // while you are debugging/testing your implementation.
 // We  will  not call this function during testing, so you may print out
 // the given ShortestPaths structure in whatever format you want.

void showShortestPaths(ShortestPaths sps) {
    
}


// Frees all memory associated with the given ShortestPaths structure.
void freeShortestPaths(ShortestPaths sPath) {
    // Free all nodes in the predecessor list
    int i = 0;
    while (i < sPath.numNodes) {
        freePredList(sPath.pred[i]);
        i++;
    }
    free(sPath.dist);
    free(sPath.pred);
}

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// HELPER FUNCTIONS FOR DJIKSTRA.C //////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// This function will be used when we find a new shortest path and therfore, we have
// to free the predecessor list and update it
static void freePredList(PredNode *curr) {
    if (curr == NULL) {
        return;
    }
    freePredList(curr->next);
    free(curr);
}

// This function creates a new node for the predecessor list
static PredNode *createPredNode(Vertex v) {
    PredNode *newNode = malloc(sizeof(struct PredNode));
    newNode->v = v;
    newNode->next = NULL;
    return newNode;
}

// This function will add a new PredNode to the predecessor list
// Note, when we add the new node we set it as the head ALWAYS
static PredNode *insertPredNode(PredNode *predHead, PredNode *addNode) {
    // If we have an empty predecessor list, just insert it
    if (predHead == NULL) {
        predHead = addNode;
    } else {
        // Else, the predecessor list has existing nodes and we have to make
        // sure it inserts as the head of the list
        addNode->next = predHead;
        predHead = addNode;
    }
    return predHead;
}
