//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////    COMP2521: Assignment 2    ////////////////////////////
////////////////////////////    Social Network Analysis   ////////////////////////////
////////////////////////////    Min-Sik Son (z5310901)    ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////// Centrality Measures ADT implementation /////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////// INCLUDED HEADER FILES AND LIBRARIES ////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#include "PQ.h"
#include "Dijkstra.h"
#include "CentralityMeasures.h"

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////////
////////////////////// HELPER FUNCTIONS FOR CENTRALITYMEASURES.C /////////////////////
//////////////////////////////////////////////////////////////////////////////////////
static double CCvalue(int sumDist, int numReachV, int nV);
static double BCvalue(PredNode **pred, Vertex src, Vertex middle, Vertex dest);
static int nodeAppearance(Vertex src, Vertex v, Vertex dest, PredNode **pred);
static int getNumPaths(Vertex src, Vertex dest, PredNode **pred);
static bool validSrcDest(Vertex srcNode, Vertex middle, Vertex destNode);
//////////////////////////////////////////////////////////////////////////////////////
/////////////// Part-2: Centrality Measures for Social Network Analysis //////////////
//////////////////////////////////////////////////////////////////////////////////////

// Explanations for Part-2:
// http://www.cse.unsw.edu.au/~cs2521/20T3/assigns/ass2/ExplanationsPart2.html

/* From CentralityMeasures.h, putting it here so I can see the struct in this file:
typedef struct NodeValues {
	int numNodes;   // The number of nodes in the graph
	double *values; // An  array  of  values, one  for  each vertex. The
	                // meaning of the values depends on  which  function
	                // is being called.
} NodeValues;
*/

// Finds the closeness centrality for each vertex in the given graph and
// returns the results in a NodeValues structure:

NodeValues closenessCentrality(Graph g) {
	// Let us make a NodeValues struct to store the data:
	NodeValues dataCC;
	int nV = GraphNumVertices(g);
	dataCC.numNodes = nV;
	dataCC.values = calloc(nV, sizeof(double));

	// We need to iterate through all the nodes/vertices in the graph here
	int i = 0;
	while (i < nV) {
		ShortestPaths sPath = dijkstra(g, i);
		
		// Find the number of reachable nodes
		// Recall from the djikstra function, we set all unreachable nodes (INF)
		// back to 0
		// Set 1 here since we basically reached the starting node
		int numReachV = 1;
		int j = 0;
		while (j < nV) {
			if (sPath.dist[j] == 0) {
				j++;
			} else {
				numReachV++;
				j++;
			}
		}

		// Calculate the accumulated distance from the source vertex to all
		// other reachable vertices:
		int sumDist = 0;
		j = 0;
		while (j < nV) {
			sumDist = sumDist + sPath.dist[j];
			j++;
		}

		// Before we calculate the closeness centrality we need to consider whether the
		// node is isolated
		if (GraphOutIncident(g, i) == NULL && GraphInIncident(g, i) == NULL) {
			// If vertex is isolated, we set the closeness centrality to 0
			dataCC.values[i] = 0;
		} else if (sumDist == 0) {
			// Also if the accumulated distance of the shortest path is 0, we also set the
			// closeness centrality measure to 0
			dataCC.values[i] = 0;
		} else {
			// Otherwise, we are safe to compute the closeness centrality -
			// Below is the formula for calculating the closeness centrality:
			double cc = CCvalue(sumDist, numReachV, nV);
			
			// Notice that we use n - 1 instead of n to avoid counting the connection to the
			// node itself ^^
			dataCC.values[i] = cc;
		}
		// Free memory associated with sPath
		freeShortestPaths(sPath);
		// Iterate for the next node now
		i++;
	}

	return dataCC;
}

// Finds  the  betweenness centrality for each vertex in the given graph
// and returns the results in a NodeValues structure:
// Note, we can measure this by counting how many shortest paths pass through a certain node
// Then we also need to find the number of appearances 'in between' the src and dest vertex

NodeValues betweennessCentrality(Graph g) {
    // Make a struct to store the data for the betweenness centrality measure
	NodeValues dataBC;
	int nV = GraphNumVertices(g);
	dataBC.numNodes = nV;
	dataBC.values = calloc(nV, sizeof(double));

	Vertex middle = 0;
	// Looping through the vertices in between, we know the path will go through the middles so
	// set this first
	while (middle < nV) {
		Vertex srcNode = 0;
		// Loop through potential source vertices in the graph network
		// Set the source node and we use djikstra to find all possible shortest paths from the source to
		// all other reachable nodes:
		while (srcNode < nV) {
			ShortestPaths sPathSrc = dijkstra(g, srcNode);
			Vertex destNode = 0;
			// Similarly we'll also be looping through the possible destination vertices to find path
			while (destNode < nV) {
				if (validSrcDest(srcNode, middle, destNode)) {
					// Note, that we don't add 1 to the counts of the source and destination only the
					// vertices in between
					// Recursively update the betweenness centrality values here, in other words we will
					// add onto the previously added BC values in the struct
					dataBC.values[middle] = dataBC.values[middle]
											+ BCvalue(sPathSrc.pred, srcNode, middle, destNode);
				}
				destNode++;
			}
			// Free the path here since we need to free memory associated with this struct
			freeShortestPaths(sPathSrc);
			srcNode++;
		}
		middle++;
	}

    return dataBC;
}

// Finds  the  normalised  betweenness centrality for each vertex in the
// given graph and returns the results in a NodeValues structure:

NodeValues betweennessCentralityNormalised(Graph g) {
	// Retrieve number of vertices in graph
	int nV = GraphNumVertices(g);
	// Make a struct to store the data of this normalised betweenness centrality measure
	NodeValues dataBCN = betweennessCentrality(g);

	Vertex i = 0;
	// Iterate for each node
	while (i < nV) {
		if (nV < 2) {
			// Not possible, illegal values are inputted, therefore we set as 0
			dataBCN.values[i] = 0;
		} else {
			// Compute into the given normalised betweenness centrality formula:
			dataBCN.values[i] = (double) 1 / ((nV - 1) * (nV - 2)) * dataBCN.values[i];
		}
		i++;
	}
	return dataBCN;
}

// Prints  the values in the given NodeValues structure to stdout in the
// * format:
// * 0: <value for node 0>
// * 1: <value for node 1>
// * ...

void showNodeValues(NodeValues nvs) {
	int i = 0;
	while (i < nvs.numNodes) {
		printf("%d: %lf\n", i, nvs.values[i]);
		i++;
	}
}

// Frees all memory associated with the given NodeValues structure:

void freeNodeValues(NodeValues nvs) {
	free(nvs.values);
}

//////////////////////////////////////////////////////////////////////////////////////
////////////////////// HELPER FUNCTIONS FOR CENTRALITYMEASURES.C /////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// Calculate the closeness centrality with the given inputs
static double CCvalue(int sumDist, int numReachV, int nV) {
	// https://stackoverflow.com/questions/49808703/how-to-convert-int-to-double-in-c-programming
	// Comment: When passing in integers to make a double we do the following:
	return (double) (numReachV - 1) / (nV-1) * (numReachV - 1) / (sumDist);
}

// Calculating the betweeness centrality measure
static double BCvalue(PredNode **pred, Vertex src, Vertex middle, Vertex dest) {
    // Total number of shortest paths from s to t
	int numPath = getNumPaths(src, dest, pred);
    
	// Cannot divide by 0 in the formula, set restriction
	if (numPath == 0) {
		return 0;
	}
	// Finding the number of paths that pass through a vertex v
    int numThruV = nodeAppearance(src, middle, dest, pred);
	
	// Sub into betweeness centrality formula:
    return (double)(numThruV)/numPath;
}

// We need to iterate through the path from the source to destination and count how many
// times the vertex v appears
static int nodeAppearance(Vertex src, Vertex v, Vertex dest, PredNode **pred) {
    int numAppearances = 0;
	// If the vertex is the destination, we find number of paths
	if (v == dest) {
		numAppearances = getNumPaths(src, v, pred);
	} else {
		PredNode *curr = pred[dest];
		while (curr != NULL) {
			// Recursively backtracking and constantly updating numAppearances
			numAppearances = numAppearances + nodeAppearance(src, v, curr->v, pred);
			curr = curr->next;
		}
	}
    
    return numAppearances;
}

// Function computes the number of paths from a given source to a given destination
static int getNumPaths(Vertex src, Vertex dest, PredNode **pred) {
    int numPaths = 0;
	PredNode *curr = pred[dest];
	// If the source is destination there is only a single path
	if (src == dest) {
		numPaths = 1;
	} else {
		// Otherwise we will recursively call the function to constantly update the number
		// of paths
		while (curr != NULL) {
			numPaths = numPaths + getNumPaths(src, curr->v, pred);
			curr = curr->next;
		}
	}
    return numPaths;
}

// Check whether our inputs for the source, middle and destination vertices are valid
static bool validSrcDest(Vertex srcNode, Vertex middle, Vertex destNode) {
    if (srcNode != destNode && srcNode != middle && destNode != middle) {
		return true;
	}
    return false;
}