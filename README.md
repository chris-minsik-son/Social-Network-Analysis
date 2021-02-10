# Social-Network-Analysis
UNSW COMP2521 2020 Assignment 2

PART 1: Dijkstra's algorithm
In order to discover say "influencers", we need to repeatedly find shortest paths between all pairs of nodes. In this section, you need to implement Dijkstra's algorithm to
discover shortest paths from a given source to all other nodes in the graph. The function offers one important additional feature, the function keeps track of multiple
predecessors for a node on shortest paths from the source, if they exist. In the following example, while discovering shortest paths from source node '0', we discovered that
there are two possible shortests paths from node '0' to node '1' (0->1 OR 0->2->1), so node '1' has two possible predecessors (node '0' or node '2') on possible shortest paths,
as shown below.

The function returns 'ShortestPaths' structure with the required information (i.e. 'distance' array, 'predecessor' arrays, source and no_of_nodes in the graph).

Part 2: Centrality Measures for Social Network Analysis
Centrality measures play very important role in analysing a social network. For example, nodes with higher "betweenness" measure often correspond to "influencers" in the given
social network. In this part you will implement two well known centrality measures for a given directed weighted graph.

Part 3: Discovering Community
In this part you need to implement the Hierarchical Agglomerative Clustering (HAC) algorithm to discover communities in a given graph. In particular, you need to implement
Lance-Williams algorithm, as described below.
