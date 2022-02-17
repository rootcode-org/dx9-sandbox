/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

// Objects to be added to a Tree should contain a node
template<class T> struct TreeNode {
	TreeNode(void) { first_child = last_child = nullptr; parent = next_sibling = prev_sibling = (T*)-1; }
	T* parent;
	T* next_sibling;
	T* prev_sibling;
	T* first_child;
	T* last_child;
};


// A hierarchy of objects of type T
//   T is the type of object that will be stored in the map
//   O is the offset of the Node within the object
template<class T, size_t O> class Tree {
public:
	Tree(void);

	// Insertion methods
	void AddChild     (T* object, T* parent);		// Add object into tree as a child of specified parent
	void InsertBefore (T* object, T* next);			// Insert object into tree preceding the specified object

	// Removal methods
	void Remove (T* object);						// Remove object from tree

	// Iteration methods
	T* IterateFirst (void);							// Reset iterator and return first object in tree
	T* IterateNext  (void);							// Increment iterator and return next object in tree

	// Traversal methods
	T* GetParent      (const T* object) const;		// Return parent of specified object
	T* GetNextSibling (const T* object) const;		// Return next sibling of specified object
	T* GetPrevSibling (const T* object) const;		// Return preceding sibling of specified object
	T* GetFirstChild  (const T* object) const;		// Return first child of specified object
	T* GetLastChild   (const T* object) const;		// Return last child of specified object

private:
	TreeNode<T> root;		// Root node of tree
	T*          next;		// Pointer to next object to be iterated
	int         count;		// Number of objects in tree

	// Private method to return pointer to node within object
	TreeNode<T>* _node (T* object) const { return (TreeNode<T>*) ((char*)object + O); }

	// Private method to return pointer to next object in tree
	T* _next (T* object) const;
};


template<class T, size_t O> Tree<T,O>::Tree(void)
{
	next = nullptr;
	count = 0;
}


template<class T, size_t O> void Tree<T,O>::AddChild (T* object, T* parent)
{
	// If a parent is not specified then the root node is the parent
	TreeNode<T>* p;
	if (parent) p = _node(parent);
	else        p = &root;

	// Add the object to the head of the parent's child list
	TreeNode<T>* n = _node(object);
	n->parent       = parent;
	n->prev_sibling = nullptr;
	n->next_sibling = p->first_child;
	p->first_child  = object;
	if (n->next_sibling) _node(n->next_sibling)->prev_sibling = object;
	else                 p->last_child = object;

	count++;
}


// Insert a sibling object into the tree
template<class T, size_t O> void Tree<T,O>::InsertBefore (T* object, T* next)
{
	if (!next) return;

	// The parent of the next object will be this objects parent
	TreeNode<T>* p;
	T* parent = _node(next)->parent;
	if (parent) p = _node(parent);
	else        p = &root;

	TreeNode<T>* n = _node(object);
	n->parent = parent;

	// Link this object to the next object
	n->next_sibling = next;

	// Link this object to the previous object
	if (n->next_sibling) n->prev_sibling = _node(next)->prev_sibling;
	else                 n->prev_sibling = p->last_child;

	// Link next object to this one
	if (n->next_sibling) _node(n->next_sibling)->prev_sibling = object;
	else                 p->last_child = object;

	// Link previous object to this one
	if (n->prev_sibling) _node(n->prev_sibling)->next_sibling = object;
	else                 p->first_child = object;

	count++;
}


template<class T, size_t O> void Tree<T,O>::Remove (T* object)
{
	// If this is the next iterator object then advance the iterator
	if (object == next)
	{
		IterateNext();
	}

	// Determine the parent object
	TreeNode<T>* p;
	T* parent = _node(object)->parent;
	if (parent) p = _node(parent);
	else        p = &root;

	// Unlink from the next object
	TreeNode<T>* n = _node(object);
	if (n->next_sibling) _node(n->next_sibling)->prev_sibling = n->prev_sibling;
	else                 p->last_child = n->prev_sibling;

	// Unlink from the previous object
	if (n->prev_sibling) _node(n->prev_sibling)->next_sibling = n->next_sibling;
	else                 p->first_child = n->next_sibling;

	count--;

	// Clear node links to indicate that the object is no longer in a tree
	// N.B. we don't clear the child links as the object may simply be being removed
	// to be re-inserted elsewhere and we want to preserve the child list
	n->next_sibling = n->prev_sibling = n->parent = (T*) -1;
}


// Reset iterator and return first object in tree
template<class T, size_t O> T* Tree<T,O>::IterateFirst (void)
{
	next = nullptr;
	if (root.first_child)
	{
		next = _next(root.first_child);
	}
	return root.first_child;
}


// Increment iterator and return next object in tree
template<class T, size_t O> T* Tree<T,O>::IterateNext (void)
{
	T* current = next;
	if (current)
	{
		next = _next(current);
	}
	return current;
}


// Private function to determine next object in tree traversal
template<class T, size_t O> T* Tree<T,O>::_next (T* object) const
{
	TreeNode<T>* n = _node(object);

	// If the object has a child, then that's next
	if (n->first_child) return n->first_child;

	// If there's no child then advance to the next sibling
	if (n->next_sibling) return n->next_sibling;

	// If there's no sibling then step back up to the parent
	while (n->parent)
	{
		// Get the parent
		n = _node(n->parent);

		// If the parent has a sibling then that's next
		if (n->next_sibling) return n->next_sibling;
	}

	// If we get here, the iteration is complete
	return nullptr;
}


template<class T, size_t O> T* Tree<T,O>::GetParent (const T* object) const
{
	return _node(object)->parent;
}


template<class T, size_t O> T* Tree<T,O>::GetNextSibling (const T* object) const
{
	return _node(object)->next_sibling;
}


template<class T, size_t O> T* Tree<T,O>::GetPrevSibling (const T* object) const
{
	return _node(object)->prev_sibling;
}


template<class T, size_t O> T* Tree<T,O>::GetFirstChild  (const T* object) const
{
	return _node(object)->first_child;
}


template<class T, size_t O> T* Tree<T,O>::GetLastChild  (const T* object) const
{
	return _node(object)->last_child;
}
