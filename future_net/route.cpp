#include "route.h"
#include "lib_record.h"
#include <stdio.h>
#include <queue>
#include <memory.h>

#include <time.h>

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
	// DATA EXTRACTION:
	// Extract data from charater string array topo[][] to 2 dimensional array 'topoArray'.
	// string format:
	// 		linkID, sourceID, destinationID, cost (end of line is not \n!)
	int topoArray[edge_num][4];
	int includingSet[MAX_INCLUDING_SET];
	int cntPass = 0;					// record valid number of elements in includingSet.
	int sourceID = 0, destinationID = 0;
	memset(topoArray, 0, sizeof(topoArray));	// initialize to 0.
	memset(includingSet, 0, sizeof(includingSet));

	getTopoArray(edge_num, topo, topoArray);
	getDemand(demand, includingSet, sourceID, destinationID, cntPass);

	//================================================================================
	// CREAT ADJACENT LIST.
	EdgeNode* nodeArray[MAX_VERTEX_NUM];		// array containing the first element of each list. Index of the array is nodeID.
	memset(nodeArray,0,sizeof(nodeArray));
	change2List(nodeArray,topoArray,edge_num);
#ifdef ROUTEDEBUG
	testChange2List(nodeArray);
#endif // DEBUG

#ifdef ROUTEDEBUG
	//getDijkstra(nodeArray,dijkstraDistance,recordRoute,recordDepth,sourceID);
#endif // ROUTEDEBUG

#ifdef ROUTEDEBUG
	//testGetDijkstra(dijkstraDistance,recordRoute,recordDepth);
#endif // ROUTEDEBUG
	//getDFS(nodeArray,includingSet,cntPass,sourceID,destinationID);
	insert2Solve(nodeArray,includingSet,cntPass,sourceID,destinationID);
	return;
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

#ifdef ROUTEDEBUG
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
#endif // ROUTEDEBUG

#ifdef ROUTEDEBUG
//actualNodes用来将来进一步做优化
void getDijkstra(EdgeNode *node[MAX_VERTEX_NUM],int dijkstraDistance[MAX_VERTEX_NUM],int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int recordDepth[MAX_VERTEX_NUM],int sourceID)
{
	bool ready2Get[MAX_VERTEX_NUM];
	memset(ready2Get,0,sizeof(ready2Get));
	//初始化alreadyGet,和ready2Get,1表示已经找到最短路径
	ready2Get[sourceID] = true;
	EdgeNode *pNode = NULL;
	for(int i=1; i<MAX_VERTEX_NUM; i++)
	{
		pNode = node[sourceID];
		updateBaseOnCurrentNode(pNode,dijkstraDistance,recordRoute,recordDepth,sourceID);
		sourceID = getMinNode(dijkstraDistance,ready2Get);
	}
	return;
}
#endif // ROUTEDEBUG

#ifdef ROUTEDEBUG
//实现获取距离currentID最短的node，更新recordRoute和recordDepth
void updateBaseOnCurrentNode(EdgeNode *node,int dijkstraDistance[MAX_VERTEX_NUM],int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int recordDepth[MAX_VERTEX_NUM],int currentID)
{
	while(node!=NULL)
	{
		//对于还没有更新距离点的更新方法
		if(dijkstraDistance[node->nodeID]>0)
		{
			if(dijkstraDistance[currentID]+node->weight<dijkstraDistance[node->nodeID])
			{
				dijkstraDistance[node->nodeID] = dijkstraDistance[currentID]+node->weight;
				//涉及到路径的存储方法
				copyRoute(recordRoute,recordDepth,currentID,node->nodeID);
			}
			else if(dijkstraDistance[currentID]+node->weight==dijkstraDistance[node->nodeID])
			{
				//相等的时候去涉及点数较大的边
				if((recordDepth[currentID]+1)>recordDepth[node->nodeID])
				{
					copyRoute(recordRoute,recordDepth,currentID,node->nodeID);
				}
			}
		}
		else
		{
			dijkstraDistance[node->nodeID] = dijkstraDistance[currentID]+node->weight;
			copyRoute(recordRoute,recordDepth,currentID,node->nodeID);
		}
		node = node->next;
	}
	return;
}
#endif // ROUTEDEBUG

#ifdef ROUTEDEBUG
//将currentID对应的路径复制到nodeID中
void copyRoute(int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int recordDepth[MAX_VERTEX_NUM],int currentID,int nodeID)
{
	memcpy(recordRoute[nodeID],recordRoute[currentID],sizeof(recordRoute[nodeID]));
	recordRoute[nodeID][recordDepth[currentID]] = currentID;
	recordDepth[nodeID] = recordDepth[currentID]+1;
	return;
}
#endif // ROUTEDEBUG

#ifdef ROUTEDEBUG
//从待选队列中选取一个最短点
int getMinNode(int dijkstraDistance[MAX_VERTEX_NUM],bool ready2Get[MAX_VERTEX_NUM])
{
	int cost = 15000;
	int ID = 0;
	for(int i=0; i<MAX_VERTEX_NUM; i++)
	{
		//为零说明此点没有得到更新
		if(dijkstraDistance[i]==0)
		{
			continue;
		}
		if(!ready2Get[i])
		{
			if(cost>dijkstraDistance[i])
			{
				cost = dijkstraDistance[i];
				ID = i;
			}
		}
	}
	ready2Get[ID] = true;
	return ID;
}
#endif // ROUTEDEBUG

#ifdef ROUTEDEBUG
void testGetDijkstra(int dijkstraDistance[MAX_VERTEX_NUM],int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int recordDepth[MAX_VERTEX_NUM])
{
	FILE *fp;
	fp = fopen("dijkstraDistance.txt","w+");
	for(int i=0; i<MAX_VERTEX_NUM; i++)
	{
		fprintf(fp,"%d ",dijkstraDistance[i]);
	}
	fprintf(fp,"\n");
	fclose(fp);
	fp = fopen("recordRoute.txt","w+");
	for(int i=0; i<MAX_VERTEX_NUM; i++)
	{
		for(int j=0; j<recordDepth[i]; j++)
		{
			fprintf(fp,"%d ",recordRoute[i][j]);
		}
		fprintf(fp,"%d\n",i);
	}
	fclose(fp);
	fp = fopen("recordDepth.txt","w+");
	for(int i=0; i<MAX_VERTEX_NUM; i++)
	{
		fprintf(fp,"%d  ",recordDepth[i]);
	}
	fprintf(fp,"\n");
	fclose(fp);
	return;
}
#endif // ROUTEDEBUG

void getDFS(EdgeNode *node[MAX_VERTEX_NUM],int nodeDemand[MAX_INCLUDING_SET],int cntPass,int sourceID,int destinationID)
{
	bool hasVisited[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
	memset(hasVisited,0,sizeof(hasVisited));
	//bool inStack[MAX_VERTEX_NUM];
	//memset(inStack,0,sizeof(inStack));
	int nodeSatck[MAX_VERTEX_NUM];
	memset(nodeSatck,0,sizeof(nodeSatck));
	int stackDepth = 0;
	nodeSatck[stackDepth] = sourceID;
	stackDepth++;
	//inStack[sourceID] = true;
	hasVisited[sourceID][sourceID] = true;
	int tempID = sourceID;
	EdgeNode *pNode = node[sourceID];
	int numbers = 0;
	int recordRoute[MAX_VERTEX_NUM];
	memset(recordRoute,0,sizeof(recordRoute));
	int tempRoute[MAX_VERTEX_NUM];
	memset(tempRoute,0,sizeof(tempRoute));
	int length = 0;
	int cost = 150000;
	int tempCost = 0;
	int a = clock();
	int b=0;
	while(stackDepth>0)
	{
		b = clock();
		if((b-a)>8*1000)
		{
			break;
		}
		while(pNode!=NULL)
		{
			if(!hasVisited[pNode->nodeID][pNode->nodeID]&&!hasVisited[tempID][pNode->nodeID])
			{
				break;
			}
			else
			{
				pNode = pNode->next;
			}
		}
		//如果pNode为空则选择退栈
		if(pNode==NULL)
		{
			stackDepth--;
			if(stackDepth>0)
			{
				pNode = node[nodeSatck[stackDepth-1]];
				tempID = nodeSatck[stackDepth-1];
				if(checkInDemand(nodeDemand,nodeSatck[stackDepth]))
				{
					numbers--;
				}
				memset(hasVisited[nodeSatck[stackDepth]],0,sizeof(hasVisited[nodeSatck[stackDepth]]));
				//inStack[nodeSatck[stackDepth]] = false;
			}
		}
		else  //入栈
		{
			nodeSatck[stackDepth] = pNode->nodeID;
			hasVisited[tempID][pNode->nodeID] = true;
			//inStack[pNode->nodeID] = true;
			hasVisited[pNode->nodeID][pNode->nodeID] = true;
			if(checkInDemand(nodeDemand,pNode->nodeID))
			{
				numbers++;
			}
			stackDepth++;
			if(pNode->nodeID==destinationID)
			{
				if(numbers==cntPass)
				{
					//检查路径权值
					//memset(tempRoute,0,sizeof(tempRoute));
					tempCost = getCost(node,nodeSatck,tempRoute,stackDepth);
					if(tempCost<cost)
					{
						cost = tempCost;
						memcpy(recordRoute,tempRoute,sizeof(tempRoute));
						length = stackDepth-1;
#ifdef ROUTEDEBUG
						printRoute(recordRoute,length,cost);
#endif // ROUTEDEBUG
					}
				}
				stackDepth--;
				if(checkInDemand(nodeDemand,nodeSatck[stackDepth]))
				{
					numbers--;
				}
				memset(hasVisited[nodeSatck[stackDepth]],0,sizeof(hasVisited[nodeSatck[stackDepth]]));
				//inStack[nodeSatck[stackDepth]] = false;
			}
			pNode = node[nodeSatck[stackDepth-1]];
			tempID = nodeSatck[stackDepth-1];
		}
	}
	if(cost<150000)
	{
		for(int i=0; i<length; i++)
		{
			record_result(recordRoute[i]);
		}
	}
}

bool checkInDemand(int nodeDemand[MAX_INCLUDING_SET],int ID)
{
	for(int i=0; i<MAX_INCLUDING_SET; i++)
	{
		if(nodeDemand[i]==ID)
		{
			return true;
		}
	}
	return false;
}

//获得路径序列的cost
int getCost(EdgeNode *node[MAX_VERTEX_NUM],int sequence[MAX_VERTEX_NUM],int route[MAX_VERTEX_NUM],int length)
{
	EdgeNode* pNode = NULL;
	int cost = 0;
	for(int i=0; i<length-1; i++)
	{
		pNode = node[sequence[i]];
		while(pNode!=NULL&&pNode->nodeID!=sequence[i+1])
		{
			pNode = pNode->next;
		}
		if(pNode!=NULL)
		{
			cost += pNode->weight;
			route[i] = pNode->linkID;
		}
	}
	return cost;
}

#ifdef ROUTEDEBUG
void printRoute(int route[MAX_VERTEX_NUM],int length,int value)
{
	printf("Cost = %d\n",value);
	for(int i=0; i<length; i++)
	{
		printf("%d ",route[i]);
	}
	printf("\n");
}
#endif // ROUTEDEBUG

void minDistance2Node2(EdgeNode *node[MAX_VERTEX_NUM],int dijkstraDistance[MAX_VERTEX_NUM],int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int recordDepth[MAX_VERTEX_NUM],int sourceID,int destinationID)
{
	bool ready2Get[MAX_VERTEX_NUM];
	memset(ready2Get,0,sizeof(ready2Get));
	//初始化alreadyGet,和ready2Get,1表示已经找到最短路径
	ready2Get[sourceID] = true;
	EdgeNode *pNode = NULL;
	for(int i=1; i<MAX_VERTEX_NUM; i++)
	{
		pNode = node[sourceID];
		updateBaseOnCurrentNode(pNode,dijkstraDistance,recordRoute,recordDepth,sourceID);
		sourceID = getMinNode(dijkstraDistance,ready2Get);
		if(sourceID==destinationID)
		{
			break;
		}
	}
	return;
}

void insert2Solve(EdgeNode *node[MAX_VERTEX_NUM],int nodeDemand[MAX_INCLUDING_SET],int cntPass,int sourceID,int destinationID)
{
	//寻路
	int dijkstraDistance[MAX_VERTEX_NUM];
	memset(dijkstraDistance,0,sizeof(dijkstraDistance));
	//记录进过的节点
	int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
	memset(recordRoute,0,sizeof(recordRoute));
	//记录每条路径的深度
	int recordDepth[MAX_VERTEX_NUM];
	memset(recordDepth,0,sizeof(recordDepth));
	int nodeStack[MAX_VERTEX_NUM];
	memset(nodeStack,0,sizeof(nodeStack));
	int tempRoute[MAX_VERTEX_NUM];
	memset(tempRoute,0,sizeof(tempRoute));
	bool hasVisited[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
	memset(hasVisited,0,sizeof(hasVisited));
	bool inStack[MAX_VERTEX_NUM];
	memset(inStack,0,sizeof(inStack));
	int stackDepth = 0;
	nodeStack[stackDepth++] = sourceID;
	inStack[sourceID] = true;
	int tempID = sourceID;
	int numbers = 0;
	int distance = 0;
	int length = 0;
	while(true)
	{
		if(numbers<=cntPass)
		{
			printf("HELLO\n");
			tempID = getNodeInDemand(node,nodeDemand,inStack,tempRoute,distance,length,tempID);
			printf("HELLO\n");
			if(tempID!=-1)
			{
				hasVisited[nodeStack[stackDepth-1]][tempID] = true;
				nodeStack[stackDepth++] = tempID;
				inStack[tempID] = true;
				memcpy(recordRoute[tempID],tempRoute,sizeof(tempRoute));
				recordDepth[tempID] = length;
				dijkstraDistance[tempID] = distance;
				if(checkInDemand(nodeDemand,tempID))
				{
					numbers++;
				}
			}
			else
			{
				tempID = nodeStack[stackDepth-1];
				memset(hasVisited[tempID],0,sizeof(hasVisited));
				dijkstraDistance[tempID] = 0;
				recordDepth[tempID] = 0;
				inStack[tempID] = false;
				stackDepth--;
				if(checkInDemand(nodeDemand,tempID))
				{
					numbers--;
				}
				tempID = nodeStack[stackDepth-1];
			}
		}
		else
		{
			bool temp = getRoute2DestinationID(node,inStack,tempRoute,length,distance,nodeStack[stackDepth-1],destinationID);
			if(temp)
			{
				hasVisited[nodeStack[stackDepth-1]][destinationID] = true;
				memcpy(recordRoute[destinationID],tempRoute,sizeof(tempRoute));
				recordDepth[destinationID] = length;
				dijkstraDistance[destinationID] = distance;
			}
			else
			{
				tempID = nodeStack[stackDepth-1];
				memset(hasVisited[tempID],0,sizeof(hasVisited));
				dijkstraDistance[tempID] = 0;
				recordDepth[tempID] = 0;
				inStack[tempID] = false;
				stackDepth--;
				if(checkInDemand(nodeDemand,tempID))
				{
					numbers--;
				}
				tempID = nodeStack[stackDepth-1];
			}
		}
	}
}

int getNodeInDemand(EdgeNode *node[MAX_VERTEX_NUM],int *nodeDemand,bool *inStack,int *route,int& distance,int& length,int sourceID)
{
	printf("HEllo\n");
	EdgeNode *pNode = NULL;
	int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
	memset(recordRoute,0,sizeof(recordRoute));
	int dijkstraDistance[MAX_VERTEX_NUM];
	memset(dijkstraDistance,0,sizeof(dijkstraDistance));
	int recordDepth[MAX_VERTEX_NUM];
	memset(recordDepth,0,sizeof(recordDepth));
	for(int i=1; i<MAX_VERTEX_NUM; i++)
	{
		pNode = node[sourceID];
		updateBaseOnCurrentNode(pNode,dijkstraDistance,recordRoute,recordDepth,sourceID);
		sourceID = getMinNode(dijkstraDistance,inStack);
		if(checkInDemand(nodeDemand,sourceID))
		{
			route = recordRoute[sourceID];
			length = recordDepth[sourceID];
			distance = dijkstraDistance[sourceID];
			return sourceID;
		}
	}
	return -1;
}

bool getRoute2DestinationID(EdgeNode *node[MAX_VERTEX_NUM],bool inStack[MAX_VERTEX_NUM],int route[MAX_VERTEX_NUM],int &length,int &distance,int sourceID,int destinationID)
{
	EdgeNode *pNode = NULL;
	int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
	memset(recordRoute,0,sizeof(recordRoute));
	int dijkstraDistance[MAX_VERTEX_NUM];
	memset(dijkstraDistance,0,sizeof(dijkstraDistance));
	int recordDepth[MAX_VERTEX_NUM];
	memset(recordDepth,0,sizeof(recordDepth));
	for(int i=1; i<MAX_VERTEX_NUM; i++)
	{
		pNode = node[sourceID];
		updateBaseOnCurrentNode(pNode,dijkstraDistance,recordRoute,recordDepth,sourceID);
		sourceID = getMinNode(dijkstraDistance,inStack);
		if(sourceID==destinationID)
		{
			route = recordRoute[sourceID];
			length = recordDepth[sourceID];
			distance = dijkstraDistance[sourceID];
			return true;
		}
	}
	return false;
}
