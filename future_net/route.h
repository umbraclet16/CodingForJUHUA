#ifndef __ROUTE_H__
#define __ROUTE_H__

#include <vector>
using namespace std;

void search_route(char *graph[5000], int edge_num, char *condition);

const int MAX_VERTEX_NUM = 600;		// max vertex number in the graph
const int MAX_INCLUDING_SET = 50;	// max vertex number in the including set
const int INF = 1000000;		// weight is integer between 1 and 20
const int NOFATHER = -1;

//#define ROUTEDEBUG

typedef struct EdgeNode
{
	int linkID;
	int nodeID;
	int weight;
	EdgeNode *next;
} EdgeNode;

struct HeapNode		// for priority queue. containing distance and corresponding nodeID.
{
	int distance, endID;
	//bool operator < (const HeapNode& rhs) const {
	//	return distance > rhs.distance;
	//}
	friend bool operator < (HeapNode n1, HeapNode n2)
	{
		return n1.distance > n2.distance;	// I don't understand this shit... this behaves normally.
	}						// if using '<', larger distance has higher priority...
};

struct StackNode	// for DFS Traverse.
{
	int ID;
	EdgeNode* ptr;
};


void getTopoArray(int edge_num, char *topo[5000], int topoArray[][4]);

void getDemand(char *demand, int includingSet[MAX_INCLUDING_SET], int& sourceID, int& destinationID, int& cntPass);

void change2List(EdgeNode *node[MAX_VERTEX_NUM],int topoArray[][4],int edge_num);

void testChange2List(EdgeNode *node[MAX_VERTEX_NUM]);

int cntNodeNum(int topoArray[MAX_VERTEX_NUM*8]);

void dijkstra(int s, int ds[MAX_VERTEX_NUM], EdgeNode* nodeArray[MAX_VERTEX_NUM], int father[MAX_VERTEX_NUM]);

int dfsTraverse(int s, int destinationID, vector<unsigned short> &result, EdgeNode* nodeArray[MAX_VERTEX_NUM], int includingSet[MAX_VERTEX_NUM]);

bool checkIncludingSet(vector<unsigned short> &result, vector<int> &passNodeSet, int includingSet[MAX_INCLUDING_SET]);

#endif
