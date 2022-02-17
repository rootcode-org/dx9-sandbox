/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

// Objects to be added to a Stack should contain a node
template<class T> struct StackNode {
	StackNode(void) { next = (T*)-1; }
	T* next;
};

// A threadsafe stack of objects of type T
//   T is the type of object that will be stored in the stack
//   O is the offset of the Node within the object
template<class T, size_t O> class Stack {
public:
	Stack(void);
	bool Push (T* object);
	T*   Pop  (void);

private:
	T*  head;

	// Private method to return pointer to node within object
	StackNode<T>* _node(T* object) const {return (StackNode<T>*) ((char*)object + O);}
};


// Declare private function for atomic compare and exchange
void* _stackInterlockedCompareAndExchangePointer(void* volatile* destination, void* exchange, void* comperand);


template<class T, size_t O> Stack<T,O>::Stack(void)
{
	head = nullptr;
}


template<class T, size_t O> bool Stack<T,O>::Push(T* object)
{
	// If this object is already in a stack then fail
	StackNode<T>* new_node = _node(object);
	if (new_node->next != (T*)-1) return false;

	// Atomically add the object to the head of the list
	T* original_head;
	do {
		original_head = head;
		new_node->next = original_head;
	} while (_stackInterlockedCompareAndExchangePointer((void* volatile*) &head, object, original_head) != original_head);

	return true;
}


template<class T, size_t O> T* Stack<T,O>::Pop(void)
{
	// Atomically remove the object from the head of the list
	T* new_head;
	T* original_head;
	do {
		if (head == nullptr) return nullptr;
		original_head = head;
		new_head = _node(original_head)->next;
	} while (_stackInterlockedCompareAndExchangePointer((void* volatile*) &head, new_head, original_head) != original_head);

	// Clear node link to indicate that the object no longer in a stack
	_node(original_head)->next = (T*)-1;

	// Return the removed object
	return original_head;
}
