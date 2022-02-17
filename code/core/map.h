/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

#include <cstdlib>

// Objects to be added to a Map should contain a node
template<class T> struct MapNode {
	MapNode(void) { next = (T*)-1; prev = (T*)-1; }
	const char* key;
	size_t key_hash;
	T*     next;
	T*     prev;
};


// A key/value store for objects of type T
//   T is the type of object that will be stored in the map
//   O is the offset of the Node within the object
//   S is the size of the hash bucket for the map
template<class T, size_t O, size_t S = 32> class Map {
public:
	Map(void);
	T* Get    (const char* key);
	T* Add    (const char* key, T* object);
	T* Remove (const char* key);

private:
	T* buckets[S];

	// Private method to return pointer to node within object
	MapNode<T>* _node(T* object) const { return (MapNode<T>*)((char*)object + O); }
};


template<class T, size_t O, size_t S> Map<T,O,S>::Map(void)
{
	memset(&buckets, 0, sizeof(buckets));
}


// Add object to map
// If the object is already in a map then the operation fails and nullptr is returned
// If the object is added successfully then the object pointer is returned
// If the key already exists in the map then the new object replaces the existing object and the existing object pointer is returned
template<class T, size_t O, size_t S> T* Map<T,O,S>::Add(const char* key, T* object)
{
	// If this object is already in a map then fail
	if (_node(object)->next != (T*)-1) return nullptr;

	// Select bucket based on key hash
	// Each bucket is a linked list containing objects sorted by their key hash
	size_t key_hash = std::hash<std::string>{}(key);
	size_t index = key_hash % S;

	// Sort by hash to find the insertion point
	T* prev = nullptr;
	T* next = buckets[index];
	while (next) {
		if (key_hash >= _node(next)->key_hash) break;
		prev = next;
		next = _node(next)->next;
	}

	// If successful we will return a pointer to the inserted object
	T* return_ptr = object;

	// If an object with this hash already exists then discard it
	if (next && (key_hash == _node(next)->key_hash))
	{
		// Unlink node
		MapNode* n = _node(next);
		if (n->next) _node(n->next)->prev = n->prev;
		if (n->prev) _node(n->prev)->next = n->next;
		else         buckets[index] = n->next;

		// Return pointer to the object just removed
		return_ptr = next;

		// Update the insertion point
		next = n->next;

		// Clear node links to indicate that the object is no longer in a map
		n->next = n->prev = (T*)-1;
	}

	// Fill out node details
	MapNode* n = _node(object);
	n->key = key;
	n->key_hash = key_hash;

	// Link to the next object
	n->next = next;
	if (n->next) _node(n->next)->prev = object;

	// Link to the previous object
	n->prev = prev;
	if (n->prev) _node(n->prev)->next = object;
	else         buckets[index] = object;

	return return_ptr;
}


template<class T, size_t O, size_t S> T* Map<T,O,S>::Get(const char* key)
{
	// Select bucket based on key hash
	size_t key_hash = std::hash<std::string>{}(key);
	size_t index = key_hash % S;

	// Walk bucket to find the object
	T* next = buckets[index];
	while (next)
	{
		if (key_hash >= _node(next)->key_hash) break;
		next = _node(next)->next;
	}

	// If we found the matching key hash then return its associated object
	if (next && (key_hash == _node(next)->key_hash)) return next;
	else return nullptr;
}


template<class T, size_t O, size_t S> T* Map<T,O,S>::Remove(const char* key)
{
	// Select bucket based on key hash
	size_t key_hash = std::hash<std::string>{}(key);
	size_t index = key_hash % S;

	// Walk bucket to find the object
	T* next = buckets[index];
	while (next)
	{
		// If we found the matching key hash then remove the object
		if (key_hash == _node(next)->key_hash)
		{
			// Remove node
			MapNode* n = _node(next);
			if (n->next) _node(n->next)->prev = n->prev;
			if (n->prev) _node(n->prev)->next = n->next;
			else         buckets[index] = n->next;

			// Clear node links to indicate that its no longer in a map
			n->next = n->prev = (T*)-1;

			return next;
		}

		if (key_hash > _node(next)->key_hash) break;
		next = _node(next)->next;
	}

	return nullptr;
}
