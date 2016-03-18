#ifndef __ROUTE_H__
#define __ROUTE_H__

void search_route(char *graph[5000], int edge_num, char *condition);

const int MAX_VERTEX_NUM = 600;		// max vertex number in the graph
const int MAX_INCLUDING_SET = 50;	// max vertex number in the including set


typedef struct EdgeNode
{
	int edgeID;
	int nextID;
	int weight;
	EdgeNode *next;
}EdgeNode;

void getTopoArray(int edge_num, char *topo[5000], int topoArray[][4]);

void getDemand(char *demand, int includingSet[50], int& sourceID, int& destinationID, int& cntPass);


#endif
