#include "route.h"
#include "lib_record.h"
#include <stdio.h>
#include <queue>
#include <stack>
//#include <vector>
#include <memory.h>
//using namespace std;

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
	// DATA EXTRACTION:
	// Extract data from charater string array topo[][] to 2 dimensional array 'topoArray'.
	// string format:
	// 		linkID, sourceID, destinationID, cost (end of line is not \n!)
	int topoArray[edge_num][4];
	int includingSet[MAX_INCLUDING_SET];
	//int cntNode;					// total amount of nodes in the graph.
	int cntPass;					// record valid number of elements in includingSet.
	int sourceID = 0, destinationID = 0;
	memset(topoArray, 0, sizeof(topoArray));	// initialize to 0.
	memset(includingSet, 0, sizeof(includingSet));	// 0 could be a node in includingSet. initialization of 0 is not safe.
							// yet it has to be to 0 due to the realization of function getDemand()...
	//for(int i = 0; i < MAX_INCLUDING_SET; i++)	// make compensation after calling getDemand().
	//	includingSet[i] = -1;
	
	vector<unsigned short> result;

	getTopoArray(edge_num, topo, topoArray);
	//cntNode = cntNodeNum(topoArray);
	//printf("cntNode = %d\n",cntNode);
	getDemand(demand, includingSet, sourceID, destinationID, cntPass);
	for(int i = cntPass; i < MAX_INCLUDING_SET; i++) includingSet[i] = -1;	// see line 23.

	
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

	//================================================================================
	// cut edges from destination to other nodes.
	nodeArray[destinationID] = NULL;

	//================================================================================
	// 无解判断
	for(int i = 0; i < cntPass+1; i++)
		if(nodeArray[passSet[i]] == NULL) return;		// 起点或V'中的任一点出度为0，则无解
	//TODO: V'中任一点或终点入度为0，则无解;
	//TODO: V'中所有点、起点、终点必在同一深度优先树上，否则无解。跑一遍DFS的时间消耗？


	int shortestDistSet[MAX_INCLUDING_SET+2][MAX_INCLUDING_SET+2];	// source and destination included.(actually used [cntPass+2][cntPass+2])
	//int pathSet[MAX_INCLUDING_SET][MAX_VERTEX_NUM];			// (actually used: [cntPass+2][MAX_VERTEX_NUM])
	//int ds[MAX_VERTEX_NUM];		// record distance between node s and other nodes.
	//int father[MAX_VERTEX_NUM];	// record father of each node. -1: no father.
	memset(shortestDistSet, INF, sizeof(shortestDistSet));
	//memset(pathSet, -1, sizeof(pathSet));
	

	int shortest = dfsTraverse(sourceID,destinationID,result,nodeArray,includingSet);
	printf("shortest dist = %d\n",shortest);


	//unsigned short result[] = {2, 6, 3};//示例中的一个解
	for (unsigned short i = 0; i < result.size(); i++)
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


/*//template
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
	memset(done, 0, sizeof(done));
	
	// INITIALIZE-SINGLE-SOURCE(G,s)
	for(int i = 0; i < MAX_VERTEX_NUM; i++){
		ds[i] = INF;
		father[i] = -1;
	}
	ds[s] = 0;

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



int dfsTraverse(int s, int destinationID, vector <unsigned short> &result, EdgeNode* nodeArray[MAX_VERTEX_NUM], int includingSet[MAX_VERTEX_NUM])
{
	int distance = INF;
	bool done[MAX_VERTEX_NUM];
	memset(done, 0, sizeof(done));
	stack<StackNode,vector<StackNode> > c;
	int ds[MAX_VERTEX_NUM];
	vector<unsigned short> result_temp;
	vector<int> passNodeSet;
	int temp;
	int tempID;
	bool passAll;

	// INITIALIZE-SINGLE-SOURCE(G,s)
	for(int i = 0; i < MAX_VERTEX_NUM; i++){
		ds[i] = INF;
	}
	ds[s] = 0;

	EdgeNode* traverse = nodeArray[s];

	c.push((StackNode){s,traverse});					printf("c.top().ID=%d\n",c.top().ID);
	//>>存储路径>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	result_temp.push_back(traverse->linkID);				printf("IN STACK:\t EDGE %d\n",traverse->linkID);
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	while(!c.empty())
	{			
										printf("in while loop, c.size=%d\n",c.size());
		if(traverse != NULL){
			while(done[traverse->nodeID] == true){		// easily cause "segmentation fault(core dump)"!!!


								printf("traverse->node=%d already visited!\n",traverse->nodeID);
				// !!! 0->2->3->1后，边4退栈，边6入栈，6是无效边，需弹出；
				// 另需压入一个点，否则点2早于边5退栈，后面程序全部跑乱.
				//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
									//printf("!!OUT STACK:\t EDGE %d\n",*(--result_temp.end()));
				//result_temp.pop_back();			//!!!!!!!!!!!!
				//passNodeSet.pop_back();		// doesn't matter what it is.
				//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

				if(traverse->next != NULL){
					traverse = traverse->next;	printf("new traverse is %d.\n",traverse->nodeID);
				}
				else{
					traverse = NULL;	printf("new traverse is NULL.\n");
					c.push((StackNode){c.top().ID,NULL});
					passNodeSet.push_back(c.top().ID);
					break;
				}
			}

			// deal with the 'else' condition in the 'while' loop above.
			if(traverse == NULL){
											printf("CONTINUE!!!!!!!!!\n");
				continue;
			}


			ds[traverse->nodeID] = ds[c.top().ID] + traverse->weight;
										printf("ds[%d]=%d\n",traverse->nodeID,ds[traverse->nodeID]);
			
			temp = traverse->nodeID;					printf("next=%d\n",temp);
			traverse = nodeArray[temp];					//printf("%p\n",traverse);
			//if((traverse!=NULL) && (done[traverse->nodeID]!=true)){//必须拆开，否则第二个条件可能导致崩溃.
			if(traverse!=NULL){
				if(done[traverse->nodeID])
					continue;
				c.push((StackNode){temp,traverse});
				//>>存储路径和此边的起点！>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				result_temp.push_back(traverse->linkID);		printf("IN STACK:\t EDGE %d\n",traverse->linkID);
				passNodeSet.push_back(temp);				printf("IN STACK: NODE %d\n",c.top().ID);
				//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

			}
			else{
				c.push((StackNode){temp,NULL});
				
				// DEBUG!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//printf("node[%d] visited!!!!\n",traverse->nodeID);
				printf("size of passNodeSet is %d\n",passNodeSet.size());
				for(int index = 0; index < passNodeSet.size(); index++)
					printf("%d\t",passNodeSet[index]);
				printf("\n");
				// DEBUG!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


				//>>路径最后一条边，只存边的起点！>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
				passNodeSet.push_back(temp);			printf("ENDPOINT IN STACK: NODE %d\n",c.top().ID);
				// 无路径，不存
				//result_temp.push_back(600);//边压栈一个无效ID 600？
				//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

			}
				done[temp] = true;
										printf("IN STACK: %d, done[%d] = true.\n",temp,temp);
		}
		else{
			// record path and passing nodes.
			if(c.top().ID == destinationID && ds[destinationID] < distance){	if(temp==19)printf("trigger!c.top:{19,NULL}\n");
				passAll = checkIncludingSet(result_temp,passNodeSet,includingSet);
										//printf("passAll = %d\n",(int)passAll);
				
				// update path and passing nodes.
				if(passAll){
					result.clear();
					//result.swap(result_temp);		// update result with result_temp.now result_temp is cleared!
					result.assign(result_temp.begin(),result_temp.end());	// should not clear result_temp!!!!!!!!
					//result_temp.clear();
					distance = ds[destinationID];
				}
			}
			
			done[c.top().ID] = false;		printf("OUT STACK: %d, done[%d]=false\n",c.top().ID,c.top().ID);
			c.pop();
								printf("1 OUT STACK: NODE %d\n",*(--passNodeSet.end()));
			passNodeSet.pop_back();
			//result_temp.pop_back();			
			


			// Traverse is over!!! without this break, occurs segmentation fault.
			if(c.empty()) break;

			//==================================================================
			tempID = c.top().ID;
			traverse = c.top().ptr;			//printf("STACK TOP is %d, next is %d.\n",c.top().ID,traverse->nodeID);

			if(traverse->next != NULL)
				traverse = traverse->next;	
			else
				traverse = NULL;
			
			/*
			do{						printf("traverse->node = %d.\n",traverse->nodeID);
				if(traverse->next != NULL)
					traverse = traverse->next;
				else{
					traverse = NULL;
					break;
				}
			}while(done[traverse->nodeID] == true);
			*/



			// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
			// update top element of the stack!!! or it goes into dead loop!!!
			// 遍历完0->2->3->1, 3出栈后栈顶元素为{node2,2指向1的指针},下一次又会将{node3,2指向3的指针}入栈。
			c.pop();
			c.push((StackNode){tempID,traverse});//printf("STACK TOP is %d, next is %d.\n",c.top().ID,traverse->nodeID);

			// 随栈的更新同步更新路径和节点>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
										printf("1 OUT STACK:\t EDGE %d\n",*(--result_temp.end()));
			result_temp.pop_back();
			// 某点弹出一条边后，转而压栈下一条边(终点需未访问！)，此时需更新记录的路径，而不需更新记录的节点（因为两条边起点相同）
			if(traverse != NULL){
				result_temp.push_back(traverse->linkID); 	printf("IN STACK:\t EDGE %d\n",traverse->linkID);
			}
			/*
			else{
										//printf("2 OUT STACK: NODE %d\n",*(--passNodeSet.end()));
				passNodeSet.pop_back();
			}
			*/
			// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		}
	}

	return distance;
}



bool checkIncludingSet(vector<unsigned short> &result, vector<int> &passNodeSet, int includingSet[MAX_INCLUDING_SET])
{
	

	// calculate number of nodes in includingSet.
	int cnt = 0;
	for(int i = 0; i < MAX_INCLUDING_SET; i++)
		if(includingSet[i] != -1)
			cnt++;
	
	/*
	// DEBUG>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	printf("TRIGGER!\n"); 
	for(int i = 0; i < passNodeSet.size(); i++)
		printf("%d\t",passNodeSet[i]);
	printf("\n");
	printf("passNodeSet size:%d, includingSet size: %d.\n",passNodeSet.size(),cnt);
	// DEBUG<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	*/

	/*
	// source is included in passNodeSet. remove it.
	passNodeSet.erase(passNodeSet.begin());		// delete the first element(i.e. source).
							// notice that remove() won't change size of vector
	*/

	if(passNodeSet.size() < cnt+1){			// destination is included in passNodeSet.
		return false;
	}

	int flag = 0;
	for(int i = 0; i < cnt; i++){
		flag = 0;
		for(unsigned int j = 0; j < passNodeSet.size(); j++){
			//if(includingSet[i] == -1) break;
			if(passNodeSet[j] == includingSet[i]){
			       flag = 1;
		       	       break;
			}
		}
		if(flag == 0) return false;
	}
	return true;
}

