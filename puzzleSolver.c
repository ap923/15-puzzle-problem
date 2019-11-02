#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include<stdbool.h>
#define N 4
#define NxN (N*N)
#define TRUE 1
#define FALSE 0
struct node {
 int cells[N][N];
 int f, g, h;
 short z_row, z_col;
 struct node *next;
 struct node *parent;
};
int goal_rows[NxN];
int goal_col[NxN];
int rep;
pthread_barrier_t barrier_before_filtering, barrier_after_filtering;

int finish=0, multithread=0;
//obj stands for objective.
struct node *start, *goal;
struct node *open = NULL, *closed = NULL;
struct node *succ_nodes[4];
int nodes_same(struct node *a, struct node *b);
void print_a_node(struct node *pnode) {
 int i, j;
 for (i=0; i<N; i++){
  for (j=0; j<N; j++)
   printf("%2d", pnode->cells[i][j]);
  printf("\n");
 }
 printf("\n");
}
void fprint_a_node(FILE *f, struct node *pnode) {
 int i, j;
 for (i=0; i<N; i++){
  for (j=0; j<N; j++)
    fprintf(f, "%2d", pnode->cells[i][j]);
  fprintf(f, "\n");
 }
 fprintf(f, "\n");
}

struct node *initialize(int argc, char **argv) {
 int i, j, k, index, cell;
 struct node *pnode;
 pnode=(struct node *) malloc(sizeof(struct node));
 index=1;
 for (j=0; j<N;j++)
  for (k=0; k<N; k++) {
  cell=atoi(argv[index++]);
   pnode->cells[j][k]=cell;
   if(cell == 0) {
    pnode->z_row=j;
    pnode->z_col=k;
   } 
  }
 pnode->f=0;
 pnode->g=0;
 pnode->next=NULL;
 pnode->parent=NULL;
 start=pnode;
 //printf("initial state\n");
 //print_a_node(start);
 pnode=(struct node*) malloc(sizeof(struct node));
 goal_rows[0]=3;
 goal_col[0]=3;
 for(index=1; index<NxN; index++) {
  j=(index-1)/N;
  k=(index-1)%N;
  goal_rows[index]=j;
  goal_col[index]=k;
  pnode->cells[j][k]=index;
 }
 pnode->cells[N-1][N-1]=0;
 pnode->f=0;
 pnode->g=0;
 pnode->h=0;
 pnode->next=NULL;
 goal=pnode;
 //printf("goal state\n");
 //rint_a_node(goal);
 



	
 return start;
}
void swap(int row1, int col1, int row2, int col2, struct node * pnode) { 
 int cell = pnode->cells[row1][col1];
 pnode->cells[row1][col1]=pnode->cells[row2][col2];
 pnode->cells[row2][col2]=cell;
}
void update_fgh(struct node *pnode) { }
//zero row and zero col are locations of the black tile!
void move_down(struct node * pnode)
{
 swap(pnode->z_row, pnode->z_col, pnode->z_row+1, pnode->z_col, pnode);
 pnode->z_row++;
}
void move_right(struct node * pnode) {
 swap(pnode->z_row, pnode->z_col, pnode->z_row, pnode->z_col+1, pnode);
 pnode->z_col++;
}
void move_up(struct node * pnode) {
 swap(pnode->z_row, pnode->z_col, pnode->z_row-1, pnode->z_col, pnode);
 pnode->z_row--;
}
void move_left(struct node * pnode) {
 swap(pnode->z_row, pnode->z_col, pnode->z_row, pnode->z_col-1, pnode);
 pnode->z_col--;  
}
/* expand a node, get its children nodes, and organize the chilrden nodes
   using array succ_nodes */
int nodes_same(struct node *a, struct node *b) {
 int flg=FALSE;
 if (memcmp(a->cells, b->cells, sizeof(int)*NxN) == 0)
  flg=TRUE;
 return flg;
}
int manhattan(struct node *curr_node, struct node *goal) {
 //this returns a heuristic value using manhattan.
 int f, g, h, hsum;
 f=0;
 g=0;
 h=0;
 hsum=0;
 int j, k;
 int curr;
  for(j=0; j<N; j++){
  for(k=0; k<N; k++) {
	 curr = curr_node->cells[j][k];
	  h += (abs(j- goal_rows[curr]) + abs (k - goal_col[curr]));
  }}
 f = curr_node->g + h;
 //printf("%d\n",f);
 
 return f;  
}

void SortedInsert(struct node** head, struct node* newNode)
{


	        // Special case for the head end
        if (*head == NULL || (*head)->f >= newNode->f)
        {
                newNode->next = *head;
                *head = newNode;
                return;
        }

        // Locate the node before the point of insertion
        struct node* current = *head;

        while(current->next != NULL &&
                        newNode->f > current->next->f)
                current = current->next;
       
	newNode->next = current->next;
        current->next = newNode;

}


void * merge_to_open() {

for(int i = 0; i <4; i++){
if(succ_nodes[i] != NULL){
	SortedInsert(&open,succ_nodes[i]);
	
}
}

}
void * print_zero(struct node *x) {
 //print out the locaton of the blank tile.
 // printf("zero row = %d, zero col = %d\n", x->z_row, x->z_col); 
}

void * succ_sort()
{ //remark, sorts the succ_nodes by heuristics
 //printf("sorting succ_sort\n");
 int i, j;
 struct node *temp;
 for (i=0;i<N;i++) {
  for (j=0;j<N-1;j++){
     if (succ_nodes[j]->f < succ_nodes[j+1]->f) {
   temp = succ_nodes[j];
   succ_nodes[j] = succ_nodes[j+1];
   succ_nodes[j+1] = temp;
     }
  }
 }
}//No need sort in expand, insert in right position in merge
void expand(struct node *selected) {
 //this is our first move.
 struct node * out;
 int m, cell; 
 int tf;
 int itex;
 
    for(itex=0; itex<N; itex++) { 
 succ_nodes[itex]=(struct node *) malloc(sizeof(struct node));
 memcpy (succ_nodes[itex]->cells, selected->cells, sizeof(selected->cells));
 succ_nodes[itex]->z_row = selected->z_row;
 succ_nodes[itex]->z_col = selected->z_col;
 succ_nodes[itex]->f = selected->f;
 succ_nodes[itex]->parent = selected;
 m=itex;
 //copy the location of the cells from last node
 //make a move.
  if (m == 0 && succ_nodes[itex]->z_row >0) {
   move_up(succ_nodes[itex]);
   succ_nodes[itex]->g+=1;
  // printf("UP\n");
  }
  if (m == 1 && succ_nodes[itex]->z_row<N-1) {
   move_down(succ_nodes[itex]);
   succ_nodes[itex]->g+=1;
  // printf("DOWN\n");
  }
  if (m == 2 && succ_nodes[itex]->z_col>0) {
   move_left(succ_nodes[itex]);
   succ_nodes[itex]->g+=1;
  // 	printf("LEFT\n");
  }
  if (m == 3 && succ_nodes[itex]->z_col<N-1) {
   move_right(succ_nodes[itex]);
   succ_nodes[itex]->g+=1;
// 	printf("RIGHT\n");
         } 
 //ensure the next is always null..
 succ_nodes[itex]->next=NULL;
 succ_nodes[itex]->f = manhattan(succ_nodes[itex], goal);
    }

 int chk;
 chk = 0;
     out = succ_nodes[0];
     out->next=NULL;

} //firstmove
void filter(int i, struct node* pnode_list){


 if(pnode_list != NULL){
        while(pnode_list->next != NULL){
                if(succ_nodes[i]!=NULL){
                if(nodes_same(pnode_list,succ_nodes[i]) == 1){
                free(succ_nodes[i]);
                succ_nodes[i] = NULL;
        break;
                }
                }
        pnode_list = pnode_list->next;

        }
        }


}

void *filter_threads(void *id){
	int *myid = (int *)id;
//printf("thread %d\n",*myid);
	while(1){   
	       	/* check the found flag, and exit when found is set */
//	pthread_barrier_wait(&barrier_before_filtering);
		filter(*myid, open);
		filter(*myid, closed);
        	pthread_barrier_wait(&barrier_after_filtering);
		pthread_exit(0);
		break;
	}
}

int main(int argc, char **argv) {
	int iter,cnt;
	struct node *copen, *cp, *solution_path;
	pthread_t thread[N-1];
	int finish;int ret, i, pathlen=0, index[N-1];
	bool multithread;
	if(strcmp(argv[1],"-m")==0){
		multithread = TRUE;
	}else multithread = false;
	solution_path=NULL;
	start=initialize(argc-1,argv+1);	/* init initial and goal states */
	//open = (struct node*) malloc(sizeof(struct node));
	open=start;
	if(multithread){
		srand(time(NULL));
		pthread_barrier_init(&barrier_before_filtering,NULL,4);
		pthread_barrier_init(&barrier_after_filtering,NULL,4);
		

	}
	closed = (struct node*) malloc(sizeof(struct node));
	copen = closed;
	cnt = 0;
	ret =0;
	iter = 0;
	int short_ids[3];
	
	while (open != NULL) {	/* Termination cond with a solution is tested in expand. */
		copen = open;
		open=open->next;	
		if(nodes_same(copen,goal)){ /* goal is found */
			if(multithread){
				finish=1;
			}
			do{ /* trace back and add the nodes on the path to a list */
				copen->next = solution_path;
				solution_path=copen;
				copen =copen->parent;
				pathlen++;
			} while(copen !=NULL);
			printf("Path (lengh=%d):\n", pathlen); 
			copen=solution_path;
			while(solution_path!=NULL){
			print_a_node(solution_path);
			solution_path = solution_path->next;
			}
			/* print out the nodes on the list */
			break;
		}
	expand(copen);
	if(multithread){
			for(i = 0; i < N-1;i++){
				short_ids[i] = i;
				pthread_create(&thread[i], NULL,filter_threads, &short_ids[i]);
			}
			
			filter(3,open);
			filter(3,closed);
			pthread_barrier_wait(&barrier_after_filtering);

			for(i =0; i< 3;i++){
				pthread_join(thread[i],NULL);
				
			}
			pthread_barrier_destroy(&barrier_after_filtering);
		
		}else{
		for(i = 0; i <4; i++){
			filter(i,open);
		        filter(i,closed);
		}}
	merge_to_open();
        copen->next = closed;
	closed = copen;
	}
	return 0;
}
