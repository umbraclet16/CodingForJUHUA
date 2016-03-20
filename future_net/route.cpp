#include "route.h"
#include "lib_record.h"
#include <stdio.h>
#include <queue>
#include <memory.h>
using namespace std;

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
	// DATA EXTRACTION:
	// Extract data from charater string array topo[][] to 2 dimensional array 'topoArray'.
	// string format:
	// 		linkID, sourceID, destinationID, cost (end of line is not \n!)
	int topoArray[edge_num][4];
	int includingSet[MAX_INCLUDING_SET];
	int cntNode;					// total amount of nodes in the graph.
	int cntPass;					// record valid number of elements in includingSet.
	int sourceID = 0, destinationID = 0;
	memset(topoArray, 0, sizeof(topoArray));	// initialize to 0.
	memset(includingSet, 0, sizeof(includingSet));	// 0 could be a node in includingSet. initialization of 0 is not safe???
	//for(int i = 0; i < MAX_INCLUDING_SET; i++)
	//	includingSet[i] = -1;

	getTopoArray(edge_num, topo, topoArray);
	//cntNode = cntNodeNum(topoArray);
	printf("cntNode = %d\n",cntNode);
	getDemand(demand, includingSet, sourceID, destinationID, cntPass);

	int passSet[MAX_INCLUDING_SET+2];		// passSet = source + includingSet + destination.
	passSet[0] = sourceID;
	for(int i = 0; i < cntPass; i++)
		passSet[i+1] = includingSet[i];
	passSet[cntPass+1] = destinationID;


	//================================================================================
	// CREAT ADJACENT LIST.
	EdgeNode* nodeArray[MAX_VERTEX_NUM];		// array containing the first element of each list. Index of the array is nodeID.
	memset(nodeArray,0,sizeof(nodeArray));
	change2List(nodeArray,topoArray,edge_num);
#ifdef ROUTEDEBUG
	testChange2List(nodeArray);
#endif // DEBUG


	int shortestDistSet[MAX_INCLUDING_SET+2][MAX_INCLUDING_SET+2];	// source and destination included.(actually used [cntPass+2][cntPass+2])
	int pathSet[MAX_INCLUDING_SET][MAX_VERTEX_NUM];			// (actually used: [cntPass+2][MAX_VERTEX_NUM])
	int ds[MAX_VERTEX_NUM];		// record distance between node s and other nodes.
	int father[MAX_VERTEX_NUM];	// record father of each node. -1: no father.
	memset(shortestDistSet, INF, sizeof(shortestDistSet));
	memset(pathSet, -1, sizeof(pathSet));
	
	// calculate shortest path between each pair of node in the passSet.
	for(int i = 0; i < cntPass + 2; i++)
	{
		dijkstra(passSet[i], ds, nodeArray, father);
		for(int j = 0; j < cntPass + 2; j++)
			shortestDistSet[i][j] = ds[j];
		for(int j = 0; j < MAX_VERTEX_NUM; j++)
			pathSet[i][j] = father[j];


	}

	//for()


	/*
	dijkstra(0, ds, nodeArray, father);
	for(int i = 0; i < 10; i++)
		printf("distance from node0 to node%d is %d.\n", i, ds[i]);
	
	int dad = father[3];
	while(dad != -1)
	{
		printf("%d->",dad);
		dad = father[dad];
	}
	*/

	unsigned short result[] = {2, 6, 3};//示例中的一个解
	for (int i = 0; i < 3; i++)
		record_result(result[i]);
}


//====================================================================================================
// FUNCTIONS
//====================================================================================================
void getTopoArray(int edge_num, char *topo[5000], int topoArray[][4] )
{
	int i, j, k;
	for(i = 0; i < edge_num; i++) 			// extract data from topo to topoArray. (topo[i][j], topoArray[i][k])
	{
		k = 0;
		for(j = 0; (topo[i][j]>='0' && topo[i][j]<='9') || topo[i][j]==','; j++) 	// end of line is not \n!
		{
			if(topo[i][j] != ',')
				topoArray[i][k] = topoArray[i][k] * 10 + topo[i][j] - '0';
			else
				k++;
		}
	}

	/*
	// check the correctness of topoArray. CHECKED!
	printf("topoArray is:\n");
	for(i = 0; i < edge_num; i++){
		for(j = 0; j < 4; j++)	printf("%d ",topoArray[i][j]);
		printf("\n");
	}
	*/
	return;
}

//====================================================================================================
void getDemand(char *demand, int includingSet[MAX_INCLUDING_SET], int& sourceID, int& destinationID, int& cntPass)
{
	// Extract data from demand[] to sourceID, destinationID and includingSet[]. includingSet contains invalid info!!(0)
	int i, j;
	for(i = 0; demand[i] != ','; i++)
		sourceID = sourceID * 10 + demand[i] - '0';
	i++;
	for(	 ; demand[i] != ','; i++)
		destinationID = destinationID * 10 +demand[i] - '0';
	i++;
	cntPass = 1;
	for(j = 0; (demand[i]>='0' && demand[i]<='9') || demand[i]=='|'; i++)
	{
		if(demand[i] != '|')
			includingSet[j] = includingSet[j] * 10 + demand[i] - '0';
		else
		{
			j++;
			cntPass++;
		}
	}

	/*
	// check the correctness of demand. CHECKED!
	printf("srcID:%d, destID:%d,\nIncludingSet:",sourceID,destinationID);
	for(i = 0; i < cntPass; i++) printf("%d ",includingSet[i]); printf("\n");
	*/

	return;
}

//====================================================================================================

///转换成邻接链表存储
void change2List(EdgeNode *node[MAX_VERTEX_NUM],int topoArray[][4],int edge_num)
{
	EdgeNode *pNode;
	EdgeNode *pTemp;
	bool repetition; ///检查两点之间的多条边
	for(int i=0; i<edge_num; i++)
	{
		repetition = false;
		pNode = node[topoArray[i][1]];
		if(pNode==NULL)
		{
			pNode = new EdgeNode();
			node[topoArray[i][1]] = pNode;
			pNode->nodeID = topoArray[i][2];
			pNode->linkID = topoArray[i][0];
			pNode->weight = topoArray[i][3];
			continue;
		}
		pTemp = pNode;
		while(pNode!=NULL)
		{
			if(pNode->nodeID==topoArray[i][2])
			{
				repetition = true;
				break;
			}
			pTemp = pNode;
			pNode = pNode->next;
		}
		if(repetition)
		{
			if(pNode->weight>topoArray[i][3])
			{
				pNode->weight = topoArray[i][3];
				pNode->linkID = topoArray[i][0];
			}
		}
		else
		{
			pNode= new EdgeNode();
			pTemp->next = pNode;
			pNode->nodeID = topoArray[i][2];
			pNode->linkID = topoArray[i][0];
			pNode->weight = topoArray[i][3];
		}
	}
}

void testChange2List(EdgeNode *node[MAX_VERTEX_NUM])
{
	EdgeNode *pNode;
	for(int i=0; i<MAX_VERTEX_NUM; i++)
	{
		pNode = node[i];
		while(pNode!=NULL)
		{
			printf("LinkID: %d,sourceID: %d, DestinationID: %d,cost: %d\n",pNode->linkID,i,pNode->nodeID,pNode->weight);
			pNode = pNode->next;
		}
	}
}


/*
int cntNodeNum(int topoArray[MAX_VERTEX_NUM*8])
{
	int cntArray[MAX_VERTEX_NUM];
	memset(cntArray, 0, sizeof(cntArray));
	int cnt = 0;
	for(int i = 0; i < MAX_VERTEX_NUM*8; i++)
	{
		int n1 = topoArray[i][1];
		int n2 = topoArray[i][2];
		cntArray[n1] = 1;
		cntArray[n2] = 1;
	}
	for(int i = 0; i < MAX_VERTEX_NUM*8; i++)
		if(cntArray[i] != 0)
			cnt++;
	return cnt;
}
*/


/*
void dijkstra(int s, int ds[MAX_VERTEX_NUM], EdgeNode* node[MAX_VERTEX_NUM])		// distance from node s to each node.
{
	priority_queue<HeapNode> Q;
	bool done[MAX_VERTEX_NUM];
	for(int i = 0; i < n; i++) ds[i] = INF;
	ds[s] = 0;
	memset(done, 0, sizeof(done));
	Q.push((HeapNode){0,s});
	while(!Q.empty()){
		HeapNode x = Q.top(); Q.pop();
		int u = x.u;
		if(done[u]) continue;
		done[u] = true;
		for(int i = 0; i < G[u].size(); i++){
			Edge& e = edges[G[u][i]];
			if(d[e.to] > ds[u] + e.dist){
				ds[e.to] = ds[u] + e.dist;
				p[e.to] = G[u][i];
				Q.push((HeapNode{d[e.to], e.to}));
			}
		}
	}
}
*/

void dijkstra(int s, int ds[MAX_VERTEX_NUM], EdgeNode* nodeArray[MAX_VERTEX_NUM], int father[MAX_VERTEX_NUM])
{
	priority_queue<HeapNode> Q;
	bool done[MAX_VERTEX_NUM];
	
	// INITIALIZE-SINGLE-SOURCE(G,s)
	for(int i = 0; i < MAX_VERTEX_NUM; i++){
		ds[i] = INF;
		father[i] = -1;
	}
	ds[s] = 0;

	memset(done, 0, sizeof(done));
	Q.push((HeapNode){0,s});
	while(!Q.empty()){
		HeapNode x = Q.top(); Q.pop();
		//printf("queue out ID:%d\n",x.endID);
		int ID = x.endID;			// first round: ID = s.
		if(done[ID]) continue;
		done[ID] = true;
		if(nodeArray[ID] != NULL){
			EdgeNode* traverse = nodeArray[ID];	// traverse initially points to the first edge(if at least 1 edge exists).
			while(traverse != NULL){
				if(ds[traverse->nodeID] > ds[ID] + traverse->weight){
					ds[traverse->nodeID] = ds[ID] + traverse->weight;
					father[traverse->nodeID] = ID;
					Q.push((HeapNode){ds[traverse->nodeID],traverse->nodeID});
					//printf("queue in ID:%d\n",traverse->nodeID);
				}
				traverse = traverse -> next;
			}
		}
	}

}




