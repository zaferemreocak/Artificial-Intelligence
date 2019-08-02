#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>

#define maxAction 8

//MAX BOARD SIZE = 127

struct node {
	struct path* pathHead;
	struct node* next;
};

struct path {
	int x;
	int y;
	struct path* next; 
};

struct location {
	int x;
	int y;
};

int main();
void init();
int search(char*);
void addToFrontier(struct node*);
struct node* popFromBeginningFrontier();
struct node* popFromEndFrontier();
void printFrontier();
bool goalTest(struct node*);
int findAllChildren(struct node*);
int findAllChildrenH(struct node*);
int findLeastOption(struct node*, int, int);
bool tieCondition(int,int,int,int);
void createLocationMatrix();
bool isInPath(struct node*,int,int);
bool isInPathH(struct node* ,int ,int ,int ,int);
struct path* getLastLocation(struct node*);
struct node* createNode();
struct path* createPath(int,int);
void createPathOfChild(int,int,struct node*,struct node*);
void printPath(struct node*);

struct location locationMatrix[maxAction];
struct node* root;
struct node* frontier = NULL;
int failure = 0;
int solutionFound = 1;
int N;

int nodeExpanded=1;
clock_t t_bound;
clock_t t;

int limit = INT_MAX;
unsigned int mem=0;

int main(int argc, char* argv[]){ //n, searchMethod, t
	printf("Usage: arg1=N , arg2=searchMethod , arg3=t(Time Bound)\n");
	N = atoi(argv[1]);
	char searchMethod[2] = "";
	strcpy(searchMethod, argv[2]);
	searchMethod[1] = '\0';
	
	
	//printf("N:%d searchMethod=%s timeLimit:%d\n",N,searchMethod,timeLimit);
	printf("Program is running with N=%d Search Method=%s Time Limit=%lld...\n",N,searchMethod,t_bound);
	
	init();
	int status; //0(failure)
	
    t = clock();
    status = search(searchMethod);
	t = clock()-t;
	
	double timeTaken = ((double)t)/CLOCKS_PER_SEC;
	printf("STATUS=%d, TIME=%f seconds NODES EXPANDED:%u",status,timeTaken,nodeExpanded);
	
	return 0;
}



void init(){
	createLocationMatrix();
	
	root = createNode(); //init
	root->pathHead = createPath(1,1); //init
}

int search(char *searchMethod){
	int status;
	struct node* poppedFromFrontier;
	if(goalTest(root)){
		printf("Root is goal!\n");
		poppedFromFrontier = NULL;
		free(poppedFromFrontier);
		return solutionFound;
	}
	addToFrontier(root);
	while(status != solutionFound){
		
		if(frontier == NULL) {		
			poppedFromFrontier = NULL;
			free(poppedFromFrontier);
			return failure;	
		}
		else{
			if(strcmp(searchMethod,"a")==0) {
				poppedFromFrontier = popFromBeginningFrontier();
				nodeExpanded++;
				status = findAllChildren(poppedFromFrontier);
			} 
			else if(strcmp(searchMethod,"b")==0) {
				poppedFromFrontier = popFromEndFrontier();
				nodeExpanded++;
				status = findAllChildren(poppedFromFrontier);
			}
			else if(strcmp(searchMethod,"c")==0) {
				poppedFromFrontier = popFromEndFrontier();
				nodeExpanded++;
				status = findAllChildrenH(poppedFromFrontier);
			}
			else{
				printf("No matched search type!\nFor bfs enter a\n");
				printf("For dfs enter b\n");
				printf("For dfs enter c\n");
				return;
			}
			
		}
	}
	poppedFromFrontier = NULL;
	free(poppedFromFrontier);

	return status;
}

void addToFrontier(struct node* newNode){
	struct node* last = (struct node*) malloc(sizeof(struct node));
	last = frontier;
	
	if(frontier == NULL){
		frontier = newNode;
		free(last);
		return;
	}
	while (last->next != NULL){
		last = last->next;
	}
	last->next = newNode;
	last = NULL;
	free(last);
}

struct node* popFromBeginningFrontier(){
	struct node* temp = (struct node*) malloc(sizeof(struct node));
	struct node* tempDelete = (struct node*) malloc(sizeof(struct node));
	
	if(frontier != NULL){
		temp = frontier->next;
		tempDelete = frontier;
		frontier = temp;
	}
	
	tempDelete->next = NULL;
	temp = NULL;
	free(temp);
	return tempDelete;
}

struct node* popFromEndFrontier(){
	struct node* temp = (struct node*) malloc(sizeof(struct node));
	struct node* last = (struct node*) malloc(sizeof(struct node));
	struct node* tempDelete = (struct node*) malloc(sizeof(struct node));
	
	temp = frontier;
	last = temp;
	
	while(temp != NULL && temp->next != NULL){
		last = temp;
		temp = temp->next;
	}
	if(last == temp){
		tempDelete = temp;
		frontier = NULL;
	}
	else {
		tempDelete = last->next;
		last->next = NULL;
	}
	last = NULL;
	temp = NULL;
	free(last);
	free(temp);
	
	return tempDelete;
}

void printFrontier(){
	int index;
	struct node* temp = (struct node*) malloc(sizeof(struct node));
	temp = frontier;
	if(temp == NULL) {
		printf("--------Frontier is empty--------\n");
	}
	else {
		printf("--------Frontier node path--------\n");
		index = 1;
	}
	while(temp != NULL){
		printf("%d.Node ",index);
		printPath(temp);
		printf("\n");
		temp = temp->next;
		index = index + 1;
	}
	printf("--------------------------------------\n\n\n\n");
	
	temp = NULL;
	free(temp);
}

bool goalTest(struct node* startNode){
	bool isGoalFound = false;
	struct path* temp = (struct path*) malloc(sizeof(struct path));
	int count = -1;
	temp = startNode->pathHead;
	while(temp != NULL) {
		temp = temp->next;
		count = count+1;
	}
	
	if(count == N*N-1){ // maxdepth
		isGoalFound = true;
	}
	
	temp = NULL;
	free(temp);
	
	if(isGoalFound) {
		//printf("GoalFound Count =%d\n ",count);	
	}
	else{
		//printf("GoalNotFound Count =%d\n ",count);	
	}
	
	return isGoalFound;
}

int findLeastOption(struct node* childNode, int x, int y){
	int babyX, babyY;
	int i; int countOption=0; int whereMinOption;
	for(i=0; i<maxAction; i++){
		babyX = x + locationMatrix[i].x;	//new youngest child's X
		babyY = y + locationMatrix[i].y;	//new youngest child's Y
		//check validity
		//In for loop, there are children that has visited before
		if((babyX > 0 && babyX <= N) && (babyY > 0 && babyY <= N) && !isInPathH(childNode,x,y,babyX,babyY)){
			countOption++;
		}
		
		//printf("\nParent node: %d,%d\n", loc->x, loc->y);
		//printf("Child node: %d,%d\n", x, y);
		//printf("findLeastOption has found %d different options (%d,%d)", countOption, babyX, babyY);
	}
	return countOption;
}

bool tieCondition(int x1, int y1, int x2, int y2){
	int distanceArray1[4], distanceArray2[4];
	int i,low1,low2;
	distanceArray1[0]= (x1-1) * (x1-1) + (y1-1) * (y1-1);	//(1,1)
	distanceArray1[1]= (x1-1) * (x1-1) + (y1-N) * (y1-N);	//(1,N)
	distanceArray1[2]= (x1-N) * (x1-N) + (y1-1) * (y1-1);	//(N,1)
	distanceArray1[3]= (x1-N) * (x1-N) + (y1-N) * (y1-N);	//(N,N)
	
	distanceArray2[0]= (x2-1) * (x2-1) + (y2-1) * (y2-1);	//(1,1)
	distanceArray2[1]= (x2-1) * (x2-1) + (y2-N) * (y2-N);	//(1,N)
	distanceArray2[2]= (x2-N) * (x2-N) + (y2-1) * (y2-1);	//(N,1)
	distanceArray2[3]= (x2-N) * (x2-N) + (y2-N) * (y2-N);	//(N,N)
	
	low1=distanceArray1[0]; low2=distanceArray2[0];
	
	for(i=0; i<4; i++){
		if(distanceArray1[i] < low1){
			low1=distanceArray1[i];
		}
		if(distanceArray2[i] < low2){
			low2=distanceArray2[i];
		}
	}
	if(low1>low2 || low1==low2){return false;}
	else{return true;}
}

int findAllChildrenH(struct node* parentNode){
	int childX, childY;
	struct path* loc = getLastLocation(parentNode);
	int i,numberOfOptions,saveIndex,prevX,prevY; int lowest=8;
	struct node* childNode;
	for(i=0; i<maxAction; i++) {
		childX = loc->x + locationMatrix[i].x;
		childY = loc->y + locationMatrix[i].y;
		
		if((childX > 0 && childX <= N) && (childY > 0 && childY <= N) && !isInPath(parentNode,childX,childY)){
			numberOfOptions=findLeastOption(parentNode, childX, childY);
			if((numberOfOptions <= lowest) && (numberOfOptions >= 0)){
				if(numberOfOptions == lowest){
					
					if(tieCondition(childX,childY,prevX,prevY)){	//latter child is better than former child
						lowest=numberOfOptions;
						saveIndex=i;
						prevX=childX;
						prevY=childY;
					}
					else{											//former child is better than latter child
						//do nothing
					}
				}
				else{
					lowest=numberOfOptions;
					saveIndex=i;
					//Save previous child node's X and Y
					prevX=childX;
					prevY=childY;
				}
				
			}
			//call your findLeastOption method and check every single child of the parent node
			//createPathOfChild( loc->x + locationMatrix[minOption] , loc->y + locationMatrix[minOption] , parentNode , childNode)
		}
	}
	
	if((loc->x+locationMatrix[saveIndex].x > 0) && (loc->x+locationMatrix[saveIndex].x <= N) && (loc->y+locationMatrix[saveIndex].y > 0) && (loc->y+locationMatrix[saveIndex].y <= N)
		&& !isInPath(parentNode,loc->x+locationMatrix[saveIndex].x,loc->y+locationMatrix[saveIndex].y)){
			
		childNode = createNode();
		createPathOfChild(locationMatrix[saveIndex].x, locationMatrix[saveIndex].y, parentNode, childNode);
		if(goalTest(childNode)){
			printf("Goal Found!\nGoal");
			printPath(childNode);
			printf("\n");
			loc = NULL;
			free(loc);
			childNode = NULL;
			free(childNode);
			return solutionFound;
		} 
		
		else {
			addToFrontier(childNode);
		}
		
		//printf("**********************************\n");
		//printPath(childNode);
		childNode = NULL;
		free(childNode);
		loc = NULL;
		free(loc);
		return failure;
	}
	
}

int findAllChildren(struct node* parentNode){
	int childX, childY;
	struct path* loc = getLastLocation(parentNode);
	int i;
	for(i=0; i<maxAction; i++) {
		childX = loc->x + locationMatrix[i].x;
		childY = loc->y + locationMatrix[i].y;
		if((childX > 0 && childX <= N) && (childY > 0 && childY <= N) && !isInPath(parentNode,childX,childY)) {
			struct node* childNode = createNode();
			createPathOfChild(locationMatrix[i].x, locationMatrix[i].y, parentNode, childNode);
			if(goalTest(childNode)){ 
				printf("Goal Found!\nGoal");
				printPath(childNode);
				printf("\n");
				loc = NULL;
				free(loc);
				childNode = NULL;
				free(childNode);
				return solutionFound;
			} 
			else {
				addToFrontier(childNode);
			}
			childNode = NULL;
			free(childNode);
		}
	}
	loc = NULL;
	free(loc);
	return failure;
}

void createLocationMatrix(){
	locationMatrix[0].x = -2; locationMatrix[7].y = -1;
	locationMatrix[1].x = -1; locationMatrix[6].y = -2;
	locationMatrix[2].x = 1; locationMatrix[5].y = -2;
	locationMatrix[3].x = 2; locationMatrix[4].y = -1;
	locationMatrix[4].x = 2; locationMatrix[3].y = 1;
	locationMatrix[5].x = 1; locationMatrix[2].y = 2;
	locationMatrix[6].x = -1; locationMatrix[1].y = 2;
	locationMatrix[7].x = -2; locationMatrix[0].y = 1;
}

bool isInPath(struct node* parentNode,int childX,int childY){
	bool isinPath = false;
	struct path* parentPath = (struct path*) malloc(sizeof(struct path));

	parentPath = parentNode->pathHead;

	while(parentPath) {
		if(parentPath->x == childX && parentPath->y == childY){
			isinPath = true;
			return isinPath;		
		} 
		parentPath = parentPath->next;
	}
	parentPath = NULL;
	free(parentPath);
	return isinPath;
}

bool isInPathH(struct node* parentNode,int childX,int childY,int babyX,int babyY){
	bool isinPath = false;
	struct path* parentPath = (struct path*) malloc(sizeof(struct path));

	parentPath = parentNode->pathHead;

	while(parentPath) {
		if((parentPath->x == childX && parentPath->y == childY) || (parentPath->x == babyX && parentPath->y == babyY)){	//check all visited nodes and other babies visited before
			isinPath = true;
			return isinPath;		
		} 
		parentPath = parentPath->next;
	}
	parentPath = NULL;
	free(parentPath);
	return isinPath;
}

struct path* getLastLocation(struct node* startNode){
	struct path* temp = (struct path*) malloc(sizeof(struct path));
	struct path* prev = (struct path*) malloc(sizeof(struct path));
	temp = startNode->pathHead;
	while(temp != NULL) {
		prev = temp;
		temp = temp->next;
	}
	
	temp = NULL;
	
	free(temp);
	
	return prev;
}

struct node* createNode(){
	struct node* newNode;
	
	if(mem >= limit/*malloc(sizeof(struct node)) == NULL*/){	//if return 0, no memory
		printf("OUT OF MEMORY![cannot create a new node]\n");
		printf("Total nodes expanded: %d\n", nodeExpanded);
		printf("Program is aborting now...");
		abort();	//Directly terminates program in case of memory overflow
		return newNode;
	}
	else{
		newNode = (struct node*) malloc(sizeof(struct node));
		mem += sizeof(struct node);
	}
	newNode->pathHead = NULL;
	newNode->next = NULL;
	
	//printf("New node is created\n");
	
	return newNode;
}

struct path* createPath(int x, int y){
	struct path* newPath;
	if(mem >= limit/*malloc(sizeof(struct path)) == NULL*/){	//if return 0, no memory
		printf("OUT OF MEMORY![cannot create a new path]\n");
		printf("Total nodes expanded: %d\n", nodeExpanded);
		printf("Program is aborting now...");
		abort();	//Directly terminates program  in case of memory overflow
		return newPath;
	}
	else{
		newPath = (struct path*) malloc(sizeof(struct path));
		mem += sizeof(struct path);
	}
	newPath->x = x;
	newPath->y = y;
	newPath->next = NULL;
	
	//printf("New path is created with (x:%d, y:%d)\n",x,y);
	return newPath;
}

void createPathOfChild(int x, int y, struct node* startNode, struct node* endNode){
	struct path* prev = (struct path*) malloc(sizeof(struct path));
	
	struct path* list = (struct path*) malloc(sizeof(struct path));
	list = startNode->pathHead;
	
	struct path* newHead = (struct path*) malloc(sizeof(struct path));
	newHead = createPath(list->x, list->y);
	
	struct path* p = newHead;
	prev = list;
	list = list -> next;
	while(list != NULL){
		p->next = createPath(list->x, list->y);
		p = p->next;
		prev = list;
		list = list -> next;
	}
	
	p->next = createPath(prev->x+x, prev->y+y);
	p->next->next = NULL;
	endNode->pathHead = newHead;
	
	prev = NULL;
	list = NULL;
	newHead = NULL;
	
	free(prev);
	free(list);
	free(newHead);
	
	//printf("New path is added\n");
}
	
void printPath(struct node* printNode){
	struct path* tempPath = (struct path*) malloc(sizeof(struct path));
	tempPath = printNode->pathHead;
	printf(" Path");
	while(tempPath){
		printf("->(x:%d,y:%d)",tempPath->x,tempPath->y);
		tempPath=tempPath->next;
	}
	
	tempPath = NULL;
	free(tempPath);
}
	
	
