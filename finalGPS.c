/*
 * finalGPS.c — Grid Pathfinder
 *
 * Generates a random 10x10 grid where each cell is either 0 (footpath) or
 * 1 (road).  The user selects a travel mode (walking=0 / driving=1), then
 * enters departure and destination coordinates.  A recursive depth-first
 * search with backtracking discovers every valid route that stays on cells
 * matching the chosen mode.  The shortest route is printed as a list of
 * (row,col) waypoints followed by cardinal-direction instructions.
 *
 * Data structures
 * ---------------
 *   Pair        – (x,y) grid coordinate
 *   Node        – singly-linked list node wrapping a Pair
 *   LinkedList  – linked list with head+last pointers; one instance = one route
 *   ans[]       – global array collecting all complete routes (max SIZE=500)
 *
 * Build
 * -----
 *   gcc finalGPS.c -o gps          (requires STD_TYPES.h in the same directory)
 *
 * Note: system("cls") is Windows-specific; replace with system("clear") on
 * Linux/macOS or remove the call entirely.
 */

#include "stdio.h"
#include "STD_TYPES.h"
#include "stdlib.h"
#include "time.h"

#define BOARDROWS 10
#define BOARDCOLOUMNS 10
#define SIZE 500

typedef struct pairs
{
  u8 x;
  u8 y;
}Pair; /*The pair of the board the user will use to reach his destination (i,j)*/

typedef struct node
{
  Pair data;
  struct node *next;
}Node;

Node* createNode(u8 x, u8 y);


typedef struct linklist
{
  Node* head ;
  Node* last;
}LinkedList;

LinkedList* creatLinkedList();
void displayLinkedList(LinkedList*);
u8 isEmpty (LinkedList*);
void insertLast (LinkedList* ,Node* );
LinkedList*  clone (LinkedList*);
void insertInArray (LinkedList*);
u32 sizeofLinkedList(LinkedList* l);

LinkedList* ans [SIZE];
u32 optionCounter = 0;
void printOptions();
void printDirections(LinkedList*);


static u8 map[BOARDROWS][BOARDCOLOUMNS];
static u8 visited[BOARDROWS][BOARDCOLOUMNS]={0};
u8 departureRow,departureColoumn;     /*To get the horizontal and vertical coordinates for departure */
u8 destinationRow,destinationColoumn; /*To get the horizontal and vertical coordinates for destination*/
u8 choice = -1;
//Pair** ans = NULL;
/* creating a pointer to pointers to pairs (array[array[pair]]), every sub pointer (sub-array)
is considered to be a valid route till the destination , so ans function is to hold all the valid option for
the user to go through */


void departure_location();
void arrival_location();
void fill_Random (u8 map[BOARDROWS][BOARDCOLOUMNS], u8 max);
void printBoard(u8 map[BOARDROWS][BOARDCOLOUMNS]);
u8 options();
void helper (u8	departureRow ,u8 departureColoumn,u8 destinationRow,u8 destinationColoumn);
void graph(u8	departureRow ,u8 departureColoumn,u8 destinationRow,u8 destinationColoumn, LinkedList* dir, u8 choice);

u32 main()
{

	/*initializing the board with random numbers and print it */
	fill_Random(map, 2); /*getting the board preped for the first time*/
	printBoard(map);/*printing the board*/
	choice=options(); /*to get weather the user is driving a car (1) or walking (0)*/

	departure_location();
	while( map[departureRow][departureColoumn] != choice)
	{
	//	printf("loop %d %d \n", departureRow, departureColoumn );
		printf("Wrong input, please try again!\n");
		departure_location();
	}

	arrival_location();
	while( map[destinationRow][destinationColoumn] != choice)
	{
		printf("Wrong input, please try again!\n");
		arrival_location();
	}
	//printf("here %d %d %d %d \n", departureRow , departureColoumn, destinationRow ,destinationColoumn );
	system("cls");
	printBoard(map);/*printing the board*/
	helper(departureRow,departureColoumn,destinationRow,destinationColoumn);
	printOptions(ans);
	return 0;
}

void departure_location()
{
	printf("please enter your current position \n");
	printf("please enter the row number: ");
	scanf("%d",&departureRow);
	printf("please enter the coloumn number: ");
	scanf("%d",&departureColoumn);
}
void arrival_location()
{
	u8 a,b;
	printf("please enter your destination \n");
	printf("please enter the row number: ");
	scanf("%d",&a);
	destinationRow =a;
//	printf("Entered destination row add: %d\n", destinationRow);
	printf("please enter the coloumn number: ");
	scanf("%d",&b);
	destinationColoumn =b;
//	printf("Entered destination row  add2 : %d\n", destinationRow);
//	printf("function %d %d \n", destinationRow, destinationColoumn );

}

/*The function is responsible for filling the board with a random numbers(0,1)*/
void fill_Random (u8 map[BOARDROWS][BOARDCOLOUMNS], u8 max)
{
	//srand(time(0)); /*to get the seed of the rand function used ton generate the random numbers*/
	for (u8 i=0 ; i<BOARDROWS ; i++)
		for(u8 j=0 ; j<BOARDCOLOUMNS ; j++)
			map[i][j]=rand()% max;
}

/*The function is used to display the board on the console*/
void printBoard(u8 map[BOARDROWS][BOARDCOLOUMNS])
{
	printf("    ");
	for (u8 i =0 ; i<BOARDROWS ; i++)
		printf("%d ",i);
	printf("\n");
	printf("   ");
	for (u8 i =0 ; i<BOARDROWS ; i++)
		printf("--");
	printf("\n");
	/*printing the main board here*/
	for (u8 i=0 ; i<BOARDROWS ; i++)
	{
		if (i<10)
    {
      printf("%d ",i );
  		printf("| ");
    }
    else
    {
      printf("%d",i );
  		printf("| ");
    }
		for(u8 j=0 ; j<BOARDCOLOUMNS ; j++)
			printf("%d ",map[i][j]);
		printf("\n" );
	}
}

/*This function to determine the mode the user shall be using during the program
The user do have two options to chose form :
1- The first for the walking mode , which implies the number 0 in the choice slot
2- The second for the driving mode , which implies the number 1 in the choice slot
if the user enters any other number but 0 or 1 a message appears to indicate the Invalid
mode choice and he is asked to re-enter option agian using goto optionChoice command , this
will keep running till the user enters a valid choice
*/
u8 options ()
{
	u8 option;
	Option_Choice :
	printf("\n");
	printf("0-walking.\n");
	printf("1-driving.\n\n");
	printf("please choose an option:");
	scanf("%d",&option);
	if (option == 0) /*mode for the walking*/
		printf("You are using the walking mode  \n");
	else if (option == 1) /*mode for the driving */
		printf("You are using the driving mode  \n");
	else /* the invalid mode */
	{
		printf(" Unvalid mode \n");
		goto Option_Choice;
	}
	return option;
}


void helper (u8	departureRow ,u8 departureColoumn,u8 destinationRow,u8 destinationColoumn)
{

	Node* firstNode = createNode(departureRow,departureColoumn);
  LinkedList* dir = creatLinkedList();
  insertLast(dir,firstNode);
	graph (departureRow,departureColoumn,destinationRow,destinationColoumn,dir, choice);
}


void graph(u8	departureRow ,u8 departureColoumn,u8 destinationRow,u8 destinationColoumn, LinkedList* dir , u8 choice)
{
  if (departureRow == destinationRow && departureColoumn == destinationColoumn)
    insertInArray(dir);

  else
  {
    visited[departureRow][departureColoumn]=1;
		if (departureRow-1>-1 && map[departureRow-1][departureColoumn] == choice && visited[departureRow-1][departureColoumn] == 0 )//up
		{
			LinkedList* tmp = clone(dir);
		  Node* newNode = createNode(departureRow-1,departureColoumn);
			insertLast(tmp,newNode);
			graph(departureRow-1,departureColoumn,destinationRow,destinationColoumn,tmp,choice);
		}
		if (departureRow+1<BOARDROWS && map[departureRow+1][departureColoumn] == choice && visited[departureRow+1][departureColoumn] == 0 )//down
		{
      LinkedList* tmp = clone(dir);
		  Node* newNode = createNode(departureRow+1,departureColoumn);
			insertLast(tmp,newNode);
			graph(departureRow+1,departureColoumn,destinationRow,destinationColoumn,tmp,choice);
		}
		if (departureColoumn+1<BOARDCOLOUMNS && map[departureRow][departureColoumn+1] == choice && visited[departureRow][departureColoumn+1] == 0 )//Right
		{
      LinkedList* tmp = clone(dir);
      Node* newNode = createNode(departureRow,departureColoumn+1);
      insertLast(tmp,newNode);
      graph(departureRow,departureColoumn+1,destinationRow,destinationColoumn,tmp,choice);
		}
		if (departureColoumn-1>-1 && map[departureRow][departureColoumn-1] == choice && visited[departureRow][departureColoumn-1] == 0 )//Left
		{
        LinkedList* tmp = clone(dir);
        Node* newNode = createNode(departureRow,departureColoumn-1);
        insertLast(tmp,newNode);
        graph(departureRow,departureColoumn-1,destinationRow,destinationColoumn,tmp,choice);
		 }

		visited[departureRow][departureColoumn]=0;
  }

}



												/*Node functions*/

/*The function is responsible for insetring the coordinates of any given point x,y
	into a struct of Node then return it */
Node* createNode(u8 x, u8 y)
{
  Node* newNode = (Node*)malloc (sizeof(Node));
  newNode->data.x = x;
  newNode->data.y = y;
  newNode->next =NULL;
  return newNode;
}




											/*LinkedList functions*/

/*The function is used to create a LinkedList
	actually what this function does is to create a pointer in the heap meomry
	and give it a name with a type pointer to struct of LinkedList, then it
	initiate the LinkedList -which is pointing to- two elements (head,last)
	to be equal to NULL , then after all , it returns the pointer*/
LinkedList* creatLinkedList()
{
  LinkedList* l = (LinkedList*)malloc(sizeof(LinkedList));
  l->head = NULL;
  l->last = NULL;
  return l;
}

/*This function checks weather the given LinkedList has elements or not
	the function takes a pointer to struct of LinkedList and fullfill its
	funcionality by checking weather the head of the linkedlist pointing
	to NULL or not , if no elements existed , the function shall return 1
	indicating the question (Is Empty?) answerd with positivity (1)
	otherwise, the funcion shall return (0) insicating negative answer*/
u8 isEmpty (LinkedList* l)
{
  if (l->head == NULL)
    return 1; //The list has no elements
  return 0;
}

/*The function adds the given Node to the linkedlist
	the function takes as a parameter the required linkedlist to be inserted in
	as pointer to struct of linkedlist and the required Node to be inserted
	as pointer to struct of Node, the function first checks weather the list is Empty
	or not by calling the pre-defined is Empty, (1-)if so the head and the last
	of LinkedList should be equated to the inserted Node then make their next NULL
  (2-) if the linkedlist is not Empty so the funcion insert the node as the
	new last element by making the last points at it, then make the next of the
	last points at NULL	*/
void insertLast (LinkedList* l ,Node* value )
{
  if (isEmpty(l))
    {
      l->head = value;
      l->last = value;
      l->head->next = NULL;
      l->last->next = NULL;
    }
  else
  {
    l->last->next = value;
    l->last =  value;
    l->last->next = NULL;
  }
}

/*The function prints the linkedlist element in appropriate formate (x,y)
	the funcion creates a pointer to node to traverse the whole linkedlist before
	it reaches the NULL (end of linkedlist)*/
void displayLinkedList(LinkedList* l)
{
  Node* current = l->head;
//  printf("head : (%d,%d) \n",l->head->data.x,l->head->data.y );
//  printf("last :(%d,%d) \n",l->last->data.x,l->last->data.y );
  while (current != NULL)
  {
    printf("(%d,%d) ",current->data.x,current->data.y );
    current=current->next;
  }
  printf("\n");
}

/*The function copy the whole data in the given linkedlist into another new linkedlist
  the function takes as input parameter a pointer to struct of linkedlist and first
	it create a new linkedlist using the pre-defined funcion creatLinkedList,
	the one required to clone to, then it start traversing the given linkedlist
	by a current node , by the end of traversal , all the nodes of the given linkedlist
	should by copied(cloned) succesfully to the new LinkedList, then it returned
	as a pointer to struct of LinkedList
*/
LinkedList* clone (LinkedList* l)
{
  Node* current =l->head;
  LinkedList* newList = creatLinkedList();
  while (current != NULL)
  {
    Node* newNode = createNode(current->data.x,current->data.y);
    insertLast(newList,newNode);
    current = current->next;
  }
  return newList;
}

/*This funcion insert the linkedlist into the ans array
  every time a linkedlist (pointer to struct of linkedlist) is inserted
	the global variable is increamentd to indicate new element insertion*/
void insertInArray (LinkedList* l)
{
  ans[optionCounter++]  = l;
}

u32 linkedlistSize (LinkedList* l)
{
  Node* current = l->head;
  u32 c = 0;
  while (current != NULL)
  {
    c++;
    current=current->next;
  }
  return c;
}




              				/*Ans array functions*/

/*The funcion is used to traverse the ans array
	since the array is storing indices of avaliable linkedlist (pointers)
	so every element calls the pre-defined funtion displayLinkedList by
	by passing the pointer of the elements wich is indeed a pointer to
	struct of LinkedList, there is a global variable (optionCounter)
	used to indicatde the number of the inserted elements in the ans
	array ,hence here used as loop thundershold*/
void printOptions()
{
    u8 index = 0;
    u32 min = linkedlistSize(ans[index]);
    //printf("Avaliable routes :\n\n");
    for (u8 i = 0; i<optionCounter;i++)
      {
        //displayLinkedList(ans[i]);
        //printDirections(ans[i]);
        printf("\n");
        if (linkedlistSize(ans[i]) < min)
          index = i;
      }
    printf("\nThe shortest route \n");
    displayLinkedList(ans[index]);
    printDirections(ans[index]);
}

u32 sizeofLinkedList(LinkedList* l)
{
  Node* current = l->head;
  u32 size =0;
  while (current!= NULL)
  {
    size++;
    current = current->next;
  }
  return size;
}
void printDirections(LinkedList* l)
{
  Node* current = l->head;
  printf("START-> ");
  while (current->next != NULL)
  {
    //printf("(%d,%d) ",current->data.x,current->data.y);
    //printf("(%d,%d)\n",current->next->data.x,current->next->data.y);
    if (current->data.x == current->next->data.x -1 && current->data.y == current->next->data.y ) //DOWN
      printf("DOWN-> ");

    if (current->data.x == current->next->data.x  && current->data.y == current->next->data.y +1 ) //LEFT
      printf("LEFT-> ");

    if (current->data.x == current->next->data.x+1 && current->data.y == current->next->data.y ) //DUP
      printf("UP-> ");

    if (current->data.x == current->next->data.x  && current->data.y == current->next->data.y -1) //RIGHT
      printf("RIGHT-> ");

    current = current->next;
  }
  printf(" END");
  printf("\n");
}
