/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

#include <cstdlib>

// Objects to be added to a List should contain a node
template<class T> struct ListNode {
	ListNode(void) { next = (T*)-1; prev = (T*)-1; }
	bool IsInList(void) { return (next != (T*)-1); }
	T* next;
	T* prev;
};


// A linked list of objects of type T
//   T is the type of object that will be stored in the map
//   O is the offset of the Node within the object
template<class T, size_t O> class List {
public:
	// Initialization
	List (void);
	void Clear(void);							// Clear all entries from list at once

	// Insertion functions
	void InsertBefore (T* object, T* next);		// Insert an object into the list prior to the specified next object
	void InsertHead   (T* object);				// Insert an object at the head of the list
	void InsertTail   (T* object);				// Insert an object at the tail of the list

	// Removal functions
	void Remove     (T* object);				// Remove an object from the list
	T*   RemoveHead (void);						// Remove and return the object at the head of the list
	T*   RemoveTail (void);						// Remove and return the object at the tail of the list

	// Iteration functions
	T* IterateFirst (void);						// Reset the iterator and return the first object in the list
	T* IterateNext  (void);						// Increment the iterator and return the next object in the list

	// Traversal functions
	T* GetHead (void) const;					// Return the object at the head of the list
	T* GetTail (void) const;					// Return the object at the tail of the list
	T* GetNext (T* object) const;				// Return the object after the specified object
	T* GetPrev (T* object) const;				// Return the object preceding the specifed object

	// Query functions
	int GetLength (void) const;					// Return the number of objects in the list

	// Sort functions
	void InsertSort (int (*compare)(const T*, const T*));								// Sort the list via an insertion method (use for small lists only, no memory allocation required)
	void QuickSort  (int (__cdecl *compare)(const T**, const T**));						// Sort the list via a quicksort method (use for large lists, allocates temporary memory for the sort)
	void BucketSort (void** buckets, int num_buckets, unsigned (*getvalue)(const T*));	// Sort the list via a bucket sort method (sorts on 32-bit value only, user supplies temporary memory)

private:
	T*  head;
	T*  tail;
	T*  iterator;
	int list_length;

	// Private method to return pointer to node within object
	ListNode<T>* _node (T* object) const {return (ListNode<T>*) ((char*)object + O);}
};


template<class T, size_t O> List<T,O>::List(void)
{
	Clear();
}


template<class T, size_t O> void List<T, O>::Clear(void)
{
	head = nullptr;
	tail = nullptr;
	iterator = nullptr;
	list_length = 0;
}


// Return the number of nodes in the list
template<class T, size_t O> int List<T,O>::GetLength (void) const
{
	return list_length;
}


// Reset iterator and return first object in list
template<class T, size_t O> T* List<T,O>::IterateFirst (void)
{
	iterator = nullptr;
	if (head)
	{
		iterator = _node(head)->next;
	}
	return head;
}


// Increment iterator and return next object in list
template<class T, size_t O> T* List<T,O>::IterateNext (void)
{
	T* current = iterator;
	if (current != nullptr)
	{
		iterator = _node(current)->next;
	}
	return current;
}


// Return first object in list
template<class T, size_t O> T* List<T,O>::GetHead (void) const
{
	return head;
}


// Return last object in list
template<class T, size_t O> T* List<T,O>::GetTail (void) const
{
	return tail;
}


// Return object following specified object
template<class T, size_t O> T* List<T,O>::GetNext (T* object) const
{
	return _node(object)->next;
}


// Return object preceding specified object
template<class T, size_t O> T* List<T,O>::GetPrev (T* object) const
{
	return _node(object)->prev;
}


// Insert an object into the list prior to a specified object
template<class T, size_t O> void List<T,O>::InsertBefore (T* object, T* next)
{
	ListNode<T>* n = _node(object);

	// Link to the next object
	n->next = next;
	if (n->next)
	{
		n->prev = _node(n->next)->prev;
		_node(n->next)->prev = object;
	}
	else
	{
		n->prev = tail;
		tail = object;
	}

	// Link to the previous object
	if (n->prev) _node(n->prev)->next = object;
	else         head = object;

	list_length++;
}


// Insert an object at the head of the list
template<class T, size_t O> void List<T,O>::InsertHead (T* object)
{
	ListNode<T>* n = _node(object);

	// Insert this object at the list head
	n->next = head;
	n->prev = nullptr;
	head = object;

	// Update next object
	if (n->next) _node(n->next)->prev = object;
	else         tail = object;

	list_length++;
}


// Insert an object at the tail of the list
template<class T, size_t O> void List<T,O>::InsertTail (T* object)
{
	ListNode<T>* n = _node(object);

	// Insert this object at the list tail
	n->next = nullptr;
	n->prev = tail;
	tail = object;

	// Update previous object
	if (n->prev) _node(n->prev)->next = object;
	else         head = object;

	list_length++;
}


// Remove an object from the list
template<class T, size_t O> void List<T,O>::Remove (T* object)
{
	ListNode<T>* n = _node(object);

	// If this is the next iterator object then advance the iterator
	if (object == iterator)
	{
		IterateNext();
	}

	// Unlink from the next object
	if (n->next) _node(n->next)->prev = n->prev;
	else         tail = n->prev;

	// Unlink from the previous object
	if (n->prev) _node(n->prev)->next = n->next;
	else         head = n->next;

	// Clear node links to indicate that the object is no longer in a list
	n->next = n->prev = (T*) -1;

	list_length--;
}


// Remove and return the object at the head of the list
template<class T, size_t O> T* List<T,O>::RemoveHead (void)
{
	T* object = head;
	if (object != nullptr)
	{
		Remove (object);
	}
	return object;
}


// Remove and return the object at the tail of the list
template<class T, size_t O> T* List<T,O>::RemoveTail (void)
{
	T* object = tail;
	if (object != nullptr)
	{
		Remove (object);
	}
	return object;
}


/*
 Insertion sort

 An insertion sort is a simple but slow sorting algorithm. We start by establishing the full
 list as a range. We then walk the range, find the highest value object, and move this to after
 the range. Thus the range shrinks by one object. We repeat this process until the range is a
 single object, at which point the list is sorted.  For example;

 [ 3, 1, 4, 0, 2 ]			// A list of 5 objects.  The object with value 4 is the highest.

 [ 3, 1, 0, 2 ] 4			// We remove the 4 from the list and move it to the end of the range

 [ 1, 0, 2 ] 3, 4			// and so on...

 [ 1, 0 ] 2, 3, 4 

 [ 0 ] 1, 2, 3, 4			// Now we're done and the list is sorted

*/
template<class T, size_t O> void List<T,O>::InsertSort (int (*compare)(const T*, const T*))
{
	// Exit if the list is empty
	if (!head) return;

	// This tracks the last item in the range
	T* last = tail;

	// Repeat sort until the range has shrunk to a single item
	while (head != last)
	{
		// Find the highest value object in the range
		T* max = head;
		T* cur = _node(head)->next;
		T* end = _node(last)->next;
		while (cur != end)
		{
			if (compare (cur,max) > 0)
			{
				max = cur;
			}
			cur = _node(cur)->next;
		}

		// Move the highest value item to after the range
		if (max == last)
		{
			last = _node(last)->prev;
		}
		else
		{
			Remove(max);
			Insert(max, _node(last)->next);
		}
	}
}


/*
 Quick sort

 A quick sort is a moderately fast general purpose sort algorithm.
 
 We quick sort the linked list as follows;
 1. Create an array of pointers, one for each object in the list
 2. Call the standard qsort function on the array
 3. Re-link the list using the sorted pointers

 Note that the compare function is passed a pointer to a pointer to the objects to compare and hence must be
 dereferenced appropriately.
*/
template<class T, size_t O> void List<T,O>::QuickSort (int (__cdecl *compare)(const T**, const T**))
{
	// Exit if there's nothing to sort
	if (list_length < 2) return;

	// Allocate an array of pointers, one for each object in the list
	void** array = new void*[list_length];

	// Fill the array with pointers to the objects in the list
	int i = 0;
	for (T* t = head; t; t = _node(t)->next)
	{
		array[i++] = t;
	}

	// Sort the array with the standard qsort function
	qsort (array, list_length, sizeof(void*), reinterpret_cast<int (__cdecl *)(const void *,const void *)>(compare));

	// Re-link the sorted array into a sorted list
	T* p = (T*) array[0];
	head = p;
	_node(p)->prev = nullptr;
	for (int j = 1; j < list_length; j++)
	{
		T* q = (T*) array[j];
		_node(p)->next = q;
		_node(q)->prev = p;
		p = q;
	}
	tail = p;
	_node(p)->next = nullptr;

	// Tidy up
	delete array;
}


/*
 Bucket Sort

 A bucket sort is a fast algorithm that can be used to sort list objects according to a 32-bit value. The
 user must supply a callback function to provide the 32-bit value for each object.

 We start by initializing an array of void pointers. Each pointer represents a bucket. For each object
 in the list we determine the bucket to place it in according to the high bits of its value (the number
 of bits used depends on the number of buckets). Once the bucket is selected we sort the object into the
 bucket using a simple linear search. Once all the objects are in a bucket we simply link the tail of
 each non-empty bucket to the head of the next to get a fully sorted list.

 The performance of the bucket sort is influenced by the number of buckets supplied. Too few buckets will
 require many linear searches within each bucket, degrading performance. However this must be balanced
 against the memory requirements of the buckets. Each bucket is a void pointer, and the number of buckets
 must be a power of 2. Hence for example, 256 buckets require 1KB, and 8192 buckets require 32KB.
*/
#if 0
template<class T, size_t O> void List<T,O>::BucketSort (void** buckets, int num_buckets, unsigned (*getvalue)(const T*))
{
	// Ensure the number of buckets is a power of 2
	if (!ispow2(num_buckets))
	{
		num_buckets = 1 << msb32(num_buckets);			// Round down to next power of 2
	}

	// Clear the buckets
	for (int i = 0; i < num_buckets; i++) buckets[i] = 0;

	// Sort each list entry into it's appropriate bucket
	// Note we use the iterator functions to walk the list as these allow the current object to be modified.
	for (T* t = IterateFirst(); t; t = IterateNext())
	{
		// Determine which bucket the item belongs in
		unsigned value = getvalue(t);
		unsigned index = value >> (32-log2(num_buckets));

		// Sort the item within its bucket
		T* p = (T*) buckets[index];
		T* q = nullptr;
		while (p && (value > getvalue(p)))
		{
			q = p;
			p = _node(p)->next;
		}

		// Insert the item into the bucket
		_node(t)->next = p;
		_node(t)->prev = q;
		if (q) _node(q)->next = t;
		else   buckets[index] = t;
		if (p) _node(p)->prev = t;
	}

	// Now link all the non-empty buckets together to get a fully sorted list
	head = nullptr;
	tail = nullptr;
	for (int i = 0; i < num_buckets; i++)
	{
		if (buckets[i] != nullptr)
		{
			T* object = (T*) buckets[i];

			// Link the start of the bucket to the previous tail
			if (!tail)
			{
				head = object;
			}
			else
			{
				_node(tail)->next   = object;
				_node(object)->prev = tail;
			}

			// Find the end of the bucket
			while (object)
			{
				tail = object;
				object = _node(object)->next;
			}
		}
	}
}
#endif

#if 0
///////////////////////////////////////////////////////////////////////////////////////////////////
// Condensed version of list class
#include <stddef.h>
template<class T> struct ListNode { ListNode(void) { next = (T*)-1; prev = (T*)-1; } bool IsInList(void) { return (next != (T*)-1); } T* next; T* prev; };
template<class T, size_t O> class List {
public:
	List(void)                                  {Clear();}
	Clear             (void)                    {head = nullptr; tail = nullptr; iterator = nullptr; list_length = 0;}
	void InsertBefore (T* object, T* next)		{ListNode<T>* n = _node(object); n->next = next; if (n->next) node->prev = _node(next)->prev; else node->prev = tail; if (n->next) _node(n->next)->prev = object; else tail = object; if (n->prev) _node(n->prev)->next = object; else head = object; list_length++;}
	void InsertHead   (T* object)				{ListNode<T>* n = _node(object); n->next = head; n->prev = 0; if (n->next) _node(n->next)->prev = object; else tail = object; head = object; list_length++;}
	void InsertTail   (T* object)				{ListNode<T>* n = _node(object); n->next = 0; n->prev = tail; tail = object; if (n->prev) _node(n->prev)->next = object; else head = object; list_length++;}
	void Remove       (T* object)				{ListNode<T>* n = _node(object); if (object == iterator) IterateNext(); if (n->next) _node(n->next)->prev = n->prev; else tail = n->prev; if (n->prev) _node(n->prev)->next = n->next; else head = n->next; n->next = n->prev = (T*) -1; list_length--;}
	T*   RemoveHead   (void)					{T* object = head; if (object != nullptr) Remove (object); return object;}
	T*   RemoveTail   (void)					{T* object = tail; if (object != nullptr) Remove (object); return object;}
	T*   IterateFirst (void)					{iterator = nullptr; if (head) iterator = _node(head)->next; return head;}
	T*   IterateNext  (void)					{T* current = iterator; if (current) iterator = _node(current)->next; return current;}
	T*   GetHead      (void) const				{return head;}
	T*   GetTail      (void) const				{return tail;}
	T*   GetNext      (T* object) const			{return _node(object)->next;}
	T*   GetPrev      (T* object) const			{return _node(object)->prev;}
	int  GetLength    (void) const				{return list_length;}
private:
	T* head; T* tail; T* iterator; int list_length;
	ListNode<T>* _node (T* object) {return (ListNode<T>*) ((char*) object + O);};
};
///////////////////////////////////////////////////////////////////////////////////////////////////
#endif
