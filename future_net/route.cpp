
#include "route.h"
#include "lib_record.h"
#include <stdio.h>
//
#include <queue>
#include <memory.h>

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{

    //================================================================================
    // DATA EXTRACTION:
    // Extract data from charater string array topo[][] to 2 dimensional array 'topoArray'.
    // string format:
    // 		linkID, sourceID, destinationID, cost (end of line is not \n!)
    int topoArray[edge_num][4];
    int includingSet[MAX_INCLUDING_SET];
    int cntPass;					// record valid number of elements in includingSet.
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
    testChange2List(nodeArray);


    //priority_queue<int,vector<int>,greater<int> > pq;
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
        pTemp = pNode;
        while(pNode->next!=NULL)
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
            printf("LinkID: %d,sourceID: %d, DestinationID: %d,cost: %d\n",pNode->linkID,i,pNode->noteID,pNode->weight);
            pNode = pNode->next;
        }
    }
}
