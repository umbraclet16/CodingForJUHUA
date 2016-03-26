#ifndef __ROUTE_H__
#define __ROUTE_H__

void search_route(char *graph[5000], int edge_num, char *condition);

const int MAX_VERTEX_NUM = 600;		// max vertex number in the graph
const int MAX_INCLUDING_SET = 50;	// max vertex number in the including set

#define ROUTEDEBUG

//#define actualNodes MAX_VERTEX_NUM

typedef struct EdgeNode
{
	int linkID;
	int nodeID;
	int weight;
	EdgeNode *next;
} EdgeNode;

void getTopoArray(int edge_num, char *topo[5000], int topoArray[][4]);

void getDemand(char *demand, int includingSet[MAX_INCLUDING_SET], int& sourceID, int& destinationID, int& cntPass);

void change2List(EdgeNode *node[MAX_VERTEX_NUM],int topoArray[][4],int edge_num);

#ifdef ROUTEDEBUG
//测试函数
void testChange2List(EdgeNode *node[MAX_VERTEX_NUM]);
#endif // ROUTEDEBUG

#ifdef ROUTEDEBUG
void getDijkstra(EdgeNode *node[MAX_VERTEX_NUM],int dijkstraDistance[MAX_VERTEX_NUM],int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int recordDepth[MAX_VERTEX_NUM],int sourceID);

void updateBaseOnCurrentNode(EdgeNode *node,int dijkstraDistance[MAX_VERTEX_NUM],int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int recordDepth[MAX_VERTEX_NUM],int currentID);

void copyRoute(int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int recordDepth[MAX_VERTEX_NUM],int currentID,int nodeID);

int getMinNode(int dijkstraDistance[MAX_VERTEX_NUM],bool ready2Get[MAX_VERTEX_NUM]);

//测试函数
void testGetDijkstra(int dijkstraDistance[MAX_VERTEX_NUM],int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int recordDepth[MAX_VERTEX_NUM]);
#endif // ROUTEDEBUG

void getDFS(EdgeNode *node[MAX_VERTEX_NUM],int nodeDemand[MAX_INCLUDING_SET],int cntPass,int sourceID,int destinationID);

bool checkInDemand(int nodeDemand[MAX_INCLUDING_SET],int ID);

int getCost(EdgeNode *node[MAX_VERTEX_NUM],int sequence[MAX_VERTEX_NUM],int route[MAX_VERTEX_NUM],int length);

#ifdef ROUTEDEBUG
void printRoute(int route[MAX_VERTEX_NUM],int length,int value);
#endif // ROUTEDEBUG

//求两点之间的最短路径
void minDistance2Node2(EdgeNode *node[MAX_VERTEX_NUM],int dijkstraDistance[MAX_VERTEX_NUM],int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int recordDepth[MAX_VERTEX_NUM],int sourceID,int destinationID);

void insert2Solve(EdgeNode *node[MAX_VERTEX_NUM],int nodeDemand[MAX_INCLUDING_SET],int cntPass,int sourceID,int destinationID);

int getNodeInDemand(EdgeNode *node[MAX_VERTEX_NUM],int *nodeDemand,bool *inStack,int *route,int& dijkstraDistance,int& recordDepth,int sourceID);

bool getRoute2DestinationID(EdgeNode *node[MAX_VERTEX_NUM],bool inStack[MAX_VERTEX_NUM],int route[MAX_VERTEX_NUM],int &length,int &distance,int sourceID,int destinationID);

#endif
