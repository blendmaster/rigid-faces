#include<stdio.h>
#include<math.h>

/*
node

earliest and latest

count = number of activites starting from the node
next = pointers to the nodes connected to the current node
dij = weight between the current(ith node) to other nodes (jth node)

*/


typedef struct node
{

    struct node ** next;
    int count;
    int ** dij;
    int earliest;
    int latest;
}node;


/*
malloc creates a node and gives the pointer to it
which is returned by the function

*/
node * createNewNode()
{
    node * newNode;
    newNode = (node *)malloc(sizeof(node));
    newNode->earliest=0;
    newNode->latest=0;
    return newNode;
}


void main()
{

    int i,j,n;
    // How many nodes do you need ?
    printf("Enter the Number of Nodes \n");
    scanf("%d",&n);

// How the edge table (negative no edge) (zero dummy edge ) (positive weight of the edge)
    int *edgeInfo=  (int *)malloc(n*n*sizeof(int));

  // pointers to nodes

    node  ** nodes  = (node **)malloc(n*sizeof(node *));


  // create those N nodes
    for (i=0;i<n;i++)
    {
        nodes[i]=  createNewNode();
    }
    int k;



// get the edge info table
    for (i=0;i<n;i++)
    {
        int count=0;
        for (j=0;j<n;j++)
        {
            scanf("%d",&k);
            *(edgeInfo+ i*n +j)=k;
            if(k>=0)
            {count++;

               printf("%d  --->%d  \n",i,j);

            }
        }

        // Create node pointers to keep the connected nodes
        nodes[i]->next =(node **)malloc(count*sizeof(node *));
        nodes[i]->dij =(int **)malloc(count*sizeof(int *));
        nodes[i]->count=count;

        printf("\n");
    }

/*

store the node pointers connected from the current node

*/
    for (i=0;i<n;i++)
    {
        int index=0;
        for (j=0;j<n;j++)
        {

            if(*(edgeInfo+ i*n +j)>=0)
                {
                       *(nodes[i]->next +index) =nodes[j];
                       *(nodes[i]->dij +index) =*(edgeInfo+ i*n +j);
                       index++;
                      // printf("%d->%d \n",i,j);
                }
            printf("%d ",*(edgeInfo +i*n +j));
        }
        printf("\n");

    }



}
