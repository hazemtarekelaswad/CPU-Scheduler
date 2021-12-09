typedef struct node {
  int pid;    // process id from input file
  int S_pid;  // process id in system
  int priority; // low value -> high pirority
  int remaining_time;
  struct node_HPF* next; // pointer to next node in queue
} node_HPF;

typedef struct HPF_Queue
{ 
  node* head;  // head of queue
  node* tail;  // tail of queue
} HPF_Queue;

// Function to check is empty 
bool isEmpty(node** head) 
{ 
    return (*head) == NULL; 
}

//insert in queue
void Enqueue (HPF_Queue* x,node* y)
{
  if (isEmpty(x))  // if queue was empty
  {
	//head and tail pointing to the only node in the queue
    x->head = y;
    x->tail = y;
    return;   
  }
  
  x->tail->next = y;  //the next to the tail is y
  x->tail = y;  	 // y is new tail of queue
  y->next = NULL;   // the new next to the tail is nullptr
  return;
} 

//Remove from Queue
void Dequeue (HPF_Queue* x,node* y)
{
    if (HPF_isempty(x))       // if queue is empty no nodes will dequeue 
		return;

    node* search;
    node* temp;

    search = x->head;   // search for the node to be dequeued

    if (search == y)    // if it is the header 
    {
	  search->head = x; 	//set search to the specified node
      x->head = (x->head)->next;	//modify the header
	  
	 return;			//search will be deleted
    }

    while (search->next != y && search != NULL)      // find previous the required node
    {
      search = search->next;
    }
    
    if (search->next == x->tail)   // checking if is tail
    {
      x->tail = search;    // search becomes new tail
      search->next=NULL;    //tail's next should be nullptr
      return;
    }
    
    search->next=(search->next)->next;  // general 
    return;
}

//Get the node with high Pirority

node* Get_PirorityNode (HPF_Queue* x)
{
  if (isempty(x))       // if queue is empty return nullptr
	return NULL;

    node* temp;
    int pirority_number;

    temp = x->head;   // start from the header
    pirority_number = temp->priority; // pirority_number will hold smallest running time in queue
 
    while (temp != NULL)      // loops over entire queue to obtain correct value for pirority_number
    {
      if (temp->priority < pirority_number)
		pirority_number = temp->priority;

      temp = temp->next;   
    }

    temp = x->head;  		//pointing to node containing pirority_number

    while (temp != NULL)
    {
      if (temp->priority == pirority_number)
		return temp;

      temp = temp->next;
    } 
}


