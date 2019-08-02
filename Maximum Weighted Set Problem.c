#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>
#include<float.h>

struct node{
	int nodeIndex;
	struct node *next;
};

// Function definitions
void createNode(int, float);
void createEdge(int, int);
void makeZero();
void generateString();
void calculateFitness();
float randomNumberGenerator();
int parentSelection(float/*, bool*/);
void init();
int checkOnes(int);
int checkAll(int);
void add(int);
int getIndex(int);
void fixIt(int, int);
void destroy();
void copy();
void crossover(int, int);
void mutation(int, int);
void getResult();

// Global Variables
char nameOfFile[100];
int numberOfGenerations, populationSize, totalNode=0, prev;
float crossProb, mutationProb, totalEdge, argf1, sum, avgFitness;
int arg0, arg1, leastFit1, leastFit2;
char *splitter;
bool isFloat;
// Array of Weights
float *array_weight;
// Array of Fitness
float *array_fitness;
// Array of Indexes
int *array_index;
// 2D Array of Population
int **array_population;
// 2D Array of Edges
int **array_edge;
// Global pointer for the function
struct node *g_pointer=NULL;

// Initialize first generation of array_population
void init(){
	int i,j,r;
	srand(time(NULL));
	
	for(i=0; i<populationSize; i++){
		for(j=0; j<totalNode; j++){
			r = rand()%2;	//Random binary
			array_population[i][j] = r;
			//printf("%d",array_population[i][j]);
		}
			
		//printf("\n");
	}
	
	
}

// Our PRECIOUS REPAIR algorithm
void repair(){
	int totalZero,r,i,bound,returnedIndex;
	srand(time(NULL));
	
	// iterate for each population member and repair
	for(i=0; i<populationSize; i++){
		totalZero = checkOnes(i);
		if(totalZero == 0){
			returnedIndex = rand() % populationSize;
			//printf("\nReturn Index is: %d", returnedIndex);
			array_index[i] = returnedIndex;
			fixIt(i,returnedIndex);
		}
		else{
			bound = checkAll(totalZero);
			if(bound == 0){
				returnedIndex = rand() % populationSize;
				//printf("\nReturn Index is: %d", returnedIndex);
				array_index[i] = returnedIndex;
				fixIt(i,returnedIndex);
			}
			else{
				r=rand() % bound;
				returnedIndex = getIndex(r);
				//printf("\nReturn Index is: %d", returnedIndex);
				array_index[i] = returnedIndex;
				fixIt(i,returnedIndex);
				destroy();	
			}
				
		}
		
		
	}
	
	g_pointer = NULL;
	
	 
}

// Check and store nodes have same amount of 1s
int checkAll(int t){
	int i,j,temp=0,bound=0;
		
	for(i=0; i<totalNode; i++){
		for(j=0; j<totalNode; j++){
			if(array_edge[i][j] == 0){
				temp++;
			}
		}
		if(temp == t){
			add(i);
			bound++;
		}
		temp=0;
	}
	return bound;
}

// Calculate total number of 1s at the array_population[index]
int checkOnes(int index){
	int i,totalOnes=0;
	
	for(i=0; i<totalNode; i++){
		if(array_population[index][i] == 0){
			totalOnes++;
		}
	}
	
	return totalOnes;
}

// Insertion operation of Repair linked list
void add(int t){
	struct node* newNode = (struct node*) malloc(sizeof(struct node));
	newNode->nodeIndex = t;
	newNode->next = NULL;
	struct node *temp = NULL;
	temp = g_pointer;
	
	if(g_pointer == NULL){
		g_pointer = newNode;
		free(temp);
		return;
	}
	while (temp->next != NULL){
		temp = temp->next;
	}
	temp->next = newNode;
	temp = NULL;
	free(temp);
}

// Eliminates allocated memory inside add function
void destroy(){
	struct node *temp=NULL;
	temp = g_pointer;
	
	while(g_pointer!=NULL){
		g_pointer = g_pointer->next;
		free(temp);
		temp=g_pointer;
	}
	
	g_pointer = NULL;
}

// Returns an index with the corresponding integer parameter
int getIndex(int dice){
	int i=0;
	struct node *temp=NULL;
	temp = g_pointer;
	
	while(temp != NULL && i<dice){
		temp = temp->next;
		i++;
	}
	
	return temp->nodeIndex;
}

// Assign array_edge content to the corresponding array_population content
void fixIt(int populationIndex, int arrayEdgeIndex){
	int i;
	
	for(i=0; i<totalNode; i++){
		array_population[populationIndex][i] = array_edge[arrayEdgeIndex][i];
	}
}

// Initialize node weights
void createNode(int nodeNumber, float weight){
	
	array_weight[nodeNumber] = weight;
	//printf("\n%d , %.3f",nodeNumber,array_weight[nodeNumber]);
}

// Change accessible points to 1
void createEdge(int nodeNumber1, int nodeNumber2){
	
	array_edge[nodeNumber1][nodeNumber2] = 1;
}

// Calculate fitness values of nodes
void calculateFitness(){
	int i, c;
	float temp1=FLT_MAX, temp2=FLT_MAX;	//max float value
	//leastFit1 = 0;
	//leastFit2 = 0;
	// find sum of nodes
	for(i=0; i<populationSize; i++){
		c=array_index[i];
		sum += array_weight[c];
	}i=0;c=0;
	// assign fitness values to array_fitness
	for(i=0; i<populationSize; i++){
		c= array_index[i];
		array_fitness[i] = array_weight[c]/sum;
	}i=0;c=0;
	
	avgFitness = sum / populationSize;
	
	// find first and second least fit members
	for(i=0; i<populationSize; i++){
		//c = array_index[i];
		if(array_fitness[i] < temp1){
			temp2 = temp1;
			temp1 = array_fitness[i];	//get least fit
			leastFit1 = i;
		}
		else if((array_fitness[i] < temp2) && (array_fitness[i] != temp1)){
			temp2 = array_fitness[i];
			leastFit2 = i;
		}
	}
	//leastFit1 = c;
	//printf("\nLeastFit1: %d , LeastFit2: %d", leastFit1, leastFit2);
}

// Make all elements of array_edge 0
void makeZero(){
	int i,j;
	for(i=0; i<totalNode; i++){
		for(j=0; j<totalNode; j++){
			array_edge[i][j]= 0;
		}
	}
}

// Copy Array of Edge to Array of Population
void copy(){
	int i, j, c;
	for(i=0; i<populationSize; i++){
		c = array_index[i];
		for(j=0; j<totalNode; j++){
			array_population[i][j] = array_edge[c][j];
		}
	}
}

// Make parent selection relying on roulette wheel selection method
int parentSelection(float limit/*, bool io*/){
	
	float partialSum=0.0;
	int index=0,zero=0;
	/*if(io == true){
		prev=0;
	}*/
	while(partialSum <= limit && index < populationSize){
		
		//printf("\nFitness value: %f",array_fitness[index]);
		partialSum += array_fitness[index];
		if(partialSum > limit){
			/*if(io == false && prev == index){		//parent1 belli ve prev=index
				return index+1;						//bir fazlasýný al
			}*/
			//prev = index;							//parent1 belli veya deðilse i
			return index;
		}
		index++;
		
	}
	return populationSize-1;
	/*
	if(io == true){
		prev = index;
		return index;
	}
	else if(io == false && index == prev){
		return index+1;
	}
	else{
		return index;
	}*/
}

// Crossover operation
void crossover(int parent1, int parent2){
	
	srand(time(NULL));
	int one_point = rand() % (totalNode);
	int buffer[2][totalNode];
	
	if(one_point==0)	// in case of cutting point is 0
		one_point==1;
		
	int i,temp;
	
	//copy contents of parents to keep them safe
	for(i=0; i<totalNode; i++){
		buffer[0][i] = array_population[parent1][i];
		buffer[1][i] = array_population[parent2][i];
	}
	
	//int x2=array_index[parent2];
	for(i=0; i<totalNode-one_point; i++){
		//temp = array_population[parent1][one_point+i];									// store first parent's index
		//array_population[parent1][one_point+i] = array_population[parent2][one_point+i];	// Parent1 crossover
		//array_population[parent2][one_point+i] = temp;									// Parent2 crossover
		
		temp = buffer[0][one_point+i];									// store first parent's index
		buffer[0][one_point+i] = buffer[1][one_point+i];				// Parent1 crossover
		buffer[1][one_point+i] = temp;									// Parent2 crossover
		
	}
	//copy contents back to the population
	for(i=0; i<totalNode; i++){
		array_population[leastFit1][i] = buffer[0][i];
		array_population[leastFit2][i] = buffer[0][i];
	}
}

// Mutation operation
void mutation(int leastFit1, int leastFit2){
	int randomIndex,i;
	float mutantNinjaTurtle;
	srand(time(NULL));
	
	
	for(i=0; i<10; i++){
		//
		mutantNinjaTurtle = randomNumberGenerator();
		//printf("\nmutant ninja turtle is %f", mutantNinjaTurtle);
		if(mutantNinjaTurtle > mutationProb){
			randomIndex = rand() % totalNode;	//generates a random index for offspring #1
			array_population[leastFit1][randomIndex] = (array_population[leastFit1][randomIndex] + 1) % 2;	//bit flip	
		}
		
		mutantNinjaTurtle = randomNumberGenerator();
		//printf("\nmutant ninja turtle is %f", mutantNinjaTurtle);
		if(mutantNinjaTurtle > mutationProb){
			randomIndex = rand() % totalNode;	//generates a random index for offspring #2
			array_population[leastFit2][randomIndex] = (array_population[leastFit2][randomIndex] + 1) % 2;	//bit flip	
		}
		
	}
}

// Generate a random floating point number
float randomNumberGenerator(){
	float min=0.0; 
    float div = RAND_MAX / sum;
    return min + (rand() / div);	//return float value less than sum
}

// Get final resolution
void getResult(){
	int i,k;
	float maxFit=0.0, sum=0.0;
	
	for(i=0; i<populationSize; i++){
		if(array_fitness[i] > maxFit){
			maxFit = array_fitness[i];
			k=i;
		}
	}
	printf("\nMaximum weighted Independent Set: ");
	for(i=0; i<totalNode; i++){
		if(array_population[k][i] == 0){
			printf("%d, ", i);
			sum += array_weight[i];
		}
		//printf("%d", array_population[k][i]);
	}
	printf("\nwith the WEIGHT SUM of %f", sum);
}

// Main Method
int main(int argc, char *argv[]){
    
    
   if(argc != 6){
      printf("Number of arguments don't match!");
      exit(0);
   }
   

   	strcpy(nameOfFile, argv[1]);
   	numberOfGenerations = atoi(argv[2]);
   	populationSize = atoi(argv[3]);
   	crossProb = atof(argv[4]);
   	mutationProb = atof(argv[5]);
   	
   	printf("File Name: %s\nNumber Of Generations: %d\nPopulation Size: %d\nCross-Over Probability: %.2f\nMutation Probability: %.2f"
	   , nameOfFile, numberOfGenerations, populationSize, crossProb, mutationProb);
	   
    FILE* reader = fopen(nameOfFile, "r"); /* should check the result */
    char line[256];					//Buffer
    char s1[16], s2[16];			//Buffers for line arguments
    int i,p;
    
    // Get total Node and total Edge
    fgets(line, sizeof(line), reader);
    totalNode = atoi(line);
    fgets(line, sizeof(line), reader);
    totalEdge = atof(line);
    
    printf("\n%d --- %f\n",totalNode,totalEdge);
    
    array_weight 	= malloc(totalNode * sizeof(float));								//Dynamic allocation size of total nodes
    array_index 	= malloc(populationSize * sizeof(int));								//Dynamic allocation of size of population size
    array_fitness 	= malloc(populationSize * sizeof(float));							//Dynamic allocation of size of population size
    
    array_population = /*(int *)*/malloc(populationSize * sizeof(int *));				//Dynamic allocation size of populationSize * totalNode
    for (i=0; i<populationSize; i++){
    	array_population[i] = (int *)malloc(totalNode * sizeof(int));
	}i=0;
	     
    array_edge = /*(int *)*/malloc(totalNode * sizeof(int *));							//Dynamic allocation of size of totalNode * totalNode
    for (i=0; i<totalNode; i++){
    	array_edge[i] = (int *)malloc(totalNode * sizeof(int));
	}i=0;
	
									
	
    makeZero();																			// Array of edge is 0
    						
    
    float randomLimit;
    int parent1, parent2;
    

    while (fgets(line, sizeof(line), reader)){
    	
    	for(i=0; i < sizeof(line); i++){
    		if(line[i] == ','){
    			line[i] = '.';
    			isFloat = true;
			}
		}
		////////////////////////////////////////////
		// Split the line into arg1 and arg2 strings
  		splitter = strtok(line, " ");
  		strcpy(s1, splitter);
  		//printf("\nArg1: %s", s1);
  		
  		while(splitter != NULL){	
  			splitter = strtok(NULL, " ");
  			if(splitter != NULL){
  				strcpy(s2, splitter);
  				//printf("\nArg2: %s", s2);
			}
		}
		////////////////////////////////////////////
		
		arg0 = atoi(s1);
		
		// Here, you collect node weights
		if(isFloat == true){
			argf1 = atof(s2);
			//printf("\n%d, %.3f", arg0,argf1);
			// create node
			createNode(arg0,argf1);
		}
		
		// Here, you collect edges
		else{
			arg1 = atoi(s2);
			//printf("\n%d, %d", arg0,arg1);
			// create edge
			createEdge(arg0,arg1);
		}
		
		isFloat = false;
	}
	
	//call generate_string function to generate strings
	printf("\n");
	init();
	repair();
	copy();
	srand(time(NULL));
	float randomProb;
	for(p=0; p<numberOfGenerations; p++){
		calculateFitness();
		printf("\nGENERATION #%d: %f[Average Fitness]", p, avgFitness);
		randomLimit = randomNumberGenerator();
		//printf("\nRandom limit for first parent is: %f", randomLimit);
		parent1 = parentSelection(randomLimit/*,true*/);
		randomLimit = randomNumberGenerator();
		//printf("\nRandom limit for second parent is: %f", randomLimit);
		parent2 = parentSelection(randomLimit/*,false*/);
		
		
		randomProb = ((float)rand()/(float)(RAND_MAX));
		
		// crossover check
		if(randomProb >= crossProb){
			
			crossover(parent1, parent2);
			mutation(parent1, parent2);
			repair();
			copy();
			
		}
	}
	// Print the result out
	getResult();
	
	// Close file pointer and deallocate memory
    fclose(reader);
    free(array_weight);
    free(array_fitness);
    free(array_population);
    free(array_index);
    free(array_edge);
	   
}

