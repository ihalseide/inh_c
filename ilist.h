struct IListNode
{
	struct IList * previous;
	struct IList * next;
	int data;
};

struct IList
{
	struct IList * first;
	struct IList * middle;
	struct IList * last;
	int length;
};

void ilist_index_insert(struct IList * plist, struct IListNode value)
{

}

void ilist_prepend(struct IList * plist, struct IListNode * value)
{

}

bool ilist_append(struct IList * plist, struct IListNode * value)
{
	/* Update the new node */
	value->previous = plist->last;
	value->next = NULL;

	/* Update the list */
	plist->length += 1;
	plist->last->next = &new;
	plist->last = &new;

	ilist_update_middle(plist);
}

void ilist_update_middle(struct IList * plist)
{
	if (plist->length % 2 == 0)
		plist->middle = &(plist->next);
}
