#include <stdlib.h>

/*
 * Library for Doubly-Linked Lists
 * By Izak Halseide
 */

/* Boolean type definition */
typedef enum {false, true} bool;

/*
 * Structures for the Doubly-Linked List...
 */

struct LinkedNode2
{
	struct LinkedNode2 * previous;
	struct LinkedNode2 * next;
	void*data_pointer;
};

struct LinkedList2
{
	struct LinkedNode2 * head;
	struct LinkedNode2 * tail;
	int length;
};

/*
 * Begin function prototypes...
 */

struct LinkedNode2 * linkedlist2_node_allocate (void * data);

bool linkedlist2_index_in_bounds (struct LinkedList2 *, int index);

void linkedlist2_node_hide (struct LinkedNode2 * node);

void linkedlist2_node_free (struct LinkedNode2 *);

void linkedlist2_free (struct LinkedList2 *);

void linkedlist2_prepend (struct LinkedList2 *, void * data);

void linkedlist2_insert (struct LinkedList2 *, void *, int index);

struct LinkedNode2 * linkedlist2_index_get (struct LinkedList2 *, int index);

void linkedlist2_index_remove(struct LinkedList2 *, int index);

void linkedlist2_update_length (struct LinkedList2 *);

void linkedlist2_print (struct LinkedList2 *);

struct LinkedList2 * linkedlist2_copy (struct LinkedList2 *);

void linkedlist2_sort (struct LinkedList2 *);

void linkedlist2_reverse (struct LinkedList2 *);

void linkedlist2_index_swap (struct LinkedList2 *, int index1,int index2);

void linkedlist2_node_swap (struct LinkedNode2 *, struct LinkedNode2 *);

struct LinkedList2 * linkedlist2_copy_array (void * array, int length);

struct LinkedList2 * linkedlist2_list_allocate (int start_length);

/*
 * Begin function implementations...
 */

struct LinkedNode2 * linkedlist2_node_allocate
(void * data_pointer)
{
	struct Linked2Node*node = malloc(sizeof(struct Linked2Node));
	node->data_pointer = data_pointer;
	return node;
}

void linkedlist2_reverse
(struct LinkedList2 * list)
{
	struct LinkedNode2 * head = list->head;
	if(head! = NULL)
	{
		list->head = list->tail;
		list->tail = head;
		while(head! = NULL)
		{
			struct LinkedNode2*next = head->next;
			head->next = previous;
			head->previous = next;
			head = next;
		}
	}
}

bool linkedlist2_index_in_bounds
(struct LinkedList2 * list, int index)
{
	return(0 <= index) && (index < list->length);
}

struct LinkedNode2 * linkedlist2_index_get
(struct LinkedList2 * list,int index)
{
	if(linkedlist2_index_in_bounds(list,index))
	{
		int n;
		struct LinkedNode2*element = list->head;
		for(n = 0;
				n<index;
				n++)
		{
			element = element->next;
		}
		return element;
	}
	else
	{
		return NULL;
	}
}
void linkedlist2_node_hide
(struct LinkedNode2*node)
{
	if(node->previous == NULL && node->next != NULL)
	{
		node->next->previous = NULL;
	}
	else if(node->previous != NULL && node->next != NULL)
	{
		node->next->previous = node->previous;
		node->previous->next = node->next;
	}
	else if(node->previous != NULL && node->next == NULL)
	{
		node->previous->next = NULL;
	}
}

void linkedlist2_node_remove
(struct LinkedNode2*node)
{
	linkedlist2_node_hide(node);
	linkedlist2_node_free(node);
}

void linkedlist2_node_free
(struct LinkedNode2*node)
{
	free(node);
}

void linkedlist2_append
(struct LinkedList2 * list, void * data)
{
	struct LinkedNode2 * new = linkedlist2_node_alloc(data);
	list->tail->next = new;
	new->previous = list->tail;
	new->next = NULL;
	list->length++;
	list->tail = new;
}

void linkedlist2_prepend
(struct LinkedList2*list,void*data)
{
	struct LinkedNode2*new = linkedlist2_node_alloc(data);
	list->head->prev = new;
	new->previous = NULL;
	new->next = list->head;
	list->length++;
	list->head = new;
}

void linkedlist2_node_swap
(struct LinkedNode2 * node1,struct LinkedNode2 * node2)
{
	void*node1_data = node1->data_pointer;
	node1->data_pointer = node2->data_pointer;
	node2->data_pointer = node1_data;
}

void linkedlist2_index_swap
(struct LinkedList2 * list,int index1,int index2)
{
	struct LinkedNode2 * node1 = linkedlist2_index_get(list,index1);
	struct LinkedNode2 * node2 = linkedlist2_index_get(list,index2);
	linkedlist2_node_swap(node1,node2);
}

struct LinkedList2 * linkedlist2_list_allocate
(int length)
{
	struct LinkedList2 * list = malloc(sizeof(struct LinkedList2));
	struct LinkedNode2 * prev = linkedlist2_node_allocate(NULL);
	struct LinkedNode2 * element;
	list->head = prev;
	int i;
	for(i = 0; i < length; i++)
	{
		element = linkedlist2_node_allocate(NULL);
		prev->next = element;
		element->previous = prev;
		prev = element;
	}
	element->next = NULL;
	list->length = length;
	return list;
}

