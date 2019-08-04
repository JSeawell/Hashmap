/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Jake Seawell
 * Date: 03/10/19
 
 This is a hashMap linked-list implementation. The user can create a map,
 put links into it, remove them, get or update a value from the map, check
 if it contains a certain key, or even print the map and contents. It also
 will resize itself it the table_load ever reaches its threshhold (defined
 in the .h file).
 
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}


/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * //@param link
 */
static void hashLinkDelete(HashLink* link)
{   
    //free key
    free(link->key);
    
    //free link
    free(link);   
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * //@param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    //assert there is a map
    assert(map != NULL);
    
    //create temp and current pointer
    HashLink* temp = NULL;
    HashLink* current = NULL;
    
    //for each bucket
    for (int i = 0; i < map->capacity; i++)
    {
        //current points to 1st bucket
        current = map->table[i];
        
        //while current is not NULL
        while (current != NULL){
            
            //temp points to next
            temp = current->next;
            
            //delete current link
	    hashLinkDelete(current);
            
            //move to next
            current = temp;
        }
    }
}


/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    if(map != NULL){
        hashMapCleanUp(map);
        free(map);
    }
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 *
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    //assert there is a map
    assert(map != NULL);
    
    //hash key to find index
    int idx = hashFunction1(key) % map->capacity;
    
    //create current pointer to link at index
    HashLink* current = map->table[idx];
    
    //if bucket is empty, return null
    if (current == NULL){
        //hashLinkDelete(current);
        return NULL;
    }
    //if bucket is not empty
    else {
        //while the key does not match, and we are not at the end of the list
        while (current != NULL && strcmp(current->key, key) != 0){
            //traverse the list to the next link
            current = current->next;
        }
        //if the key matches
        if (current != NULL && strcmp(current->key, key) == 0){
            //create a pointer to the value at that key, and return it
            int* ptrToValue = &(current->value);
            //hashLinkDelete(current);
            return ptrToValue;
        }
        //otherwise, we're at the end of the list and the key was not found
        //return null
        else {
        
            return NULL;
        }
    }
}



/**
 * Resizes the hash table to have a number of buckets equal to the given
 * capacity (double of the old capacity). After allocating the new table,
 * all of the links need to rehashed into it because the capacity has changed.
 *
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 *
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    //assert there is a map
    assert(map != NULL);
    
    //create new table with double the capacity of old map;
    HashMap* newMap = hashMapNew(map->capacity * 2);
    
    //current pointer in old map
    HashLink* oldMapCurrent = NULL;
    
    //for each bucket
    for (int i = 0; i < map->capacity; i++)
    {
        //current points to 1st bucket
        oldMapCurrent = map->table[i];
        
        //while current is not NULL
        while (oldMapCurrent != NULL){
            
            //put key in new map
            hashMapPut(newMap, oldMapCurrent->key, oldMapCurrent->value);
            
            oldMapCurrent = oldMapCurrent->next;
        }
    }
    
    //map points to new table
    HashLink** tempTable = map->table;
    map->table = newMap->table;
    newMap->table = tempTable;
    
    //change capacity
    map->capacity = newMap->capacity;
    
    //delete and free original map (now pointed to by newMap ptr)
    free(newMap);
    //hashMapDelete(newMap);
}


/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 *
 * //@param map
 * //@param key
 * //@param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    //assert map exists
    assert(map != NULL);
    
    //If capacity reached, resize map
    if(hashMapTableLoad(map) >= MAX_TABLE_LOAD){
        resizeTable(map, 2 * map->capacity);
    }
    //hash key to find index
    int newIdx = hashFunction1(key) % map->capacity;
    
    //current pointer
    HashLink* current = NULL;
    
    //current pointer to link at index
    current = map->table[newIdx];
    
    //if spot is empty
    if (current == NULL){
        
        //create a new hash link with key and value and null next, and put in new index
        HashLink*  newLink = hashLinkNew(key, value, NULL);
        map->table[newIdx] = newLink;
        map->size++;
        
        return;
    }
    //if spot is not empty
    else {
        
        //if keys match, update value and return
        if (strcmp(current->key, key) == 0){
            current->value = value;
            
            return;
        }
        
        //while next spot is not empty
        while (current->next != NULL){
            
            //traverse down the list until next is empty spot
            current = current->next;
            
            //if keys match, update value and return
            if (strcmp(current->key, key) == 0){
                current->value = value;
                
                return;
            }
        }
        //next spot is empty
        HashLink*  newLink = hashLinkNew(key, value, NULL);
        current->next = newLink;
        map->size++;
        
        return;
    }
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    //assert map exists
    assert(map != NULL);
    
    //hash key to index
    int newIdx = hashFunction1(key) % map->capacity;
    
    //create current pointer
    HashLink* current = NULL;
    
    //for each bucket in table
    for (int i = 0; i < map->capacity; i++){
        
        //current pointer to link at index
        current = map->table[newIdx];
        
        //if spot is empty
        if (current == NULL){
            //hashLinkDelete(current);
            return;
        }
        //while spot is not empty
        else {
            
            //if keys match, remove link, return
            if (strcmp(current->key, key) == 0){
                
                HashLink* temp = current->next;
                
                map->table[newIdx] = temp;
                
                //delete and free current
                hashLinkDelete(current);
                current = NULL;
                
                //free temp
                temp = NULL;
                //hashLinkDelete(temp);
                
                map->size--;
                
                return;
            }
            
            //while next spot is not empty
            while (current->next != NULL){
                
                //if next key matches, remove next link, update pointers, return
                if (strcmp(current->next->key, key) == 0){
                
                    //hold current->next's next
                    HashLink* temp = current->next->next;
                    
                    //delete and free next
                    hashLinkDelete(current->next);
                    
                    //assign next to temp
                    current->next = temp;
                    
                    //free temp
                    temp = NULL;
                    //hashLinkDelete(temp);
                    
                    map->size--;
                    
                    return;
                }
                
                //traverse down the list until next is empty spot
                current = current->next;
            }
        }
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 *
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    //assert map exists
    assert(map != NULL);
    
    //hash key to find index
    int idx = hashFunction1(key) % map->capacity;
    
    //create current pointer to link at index
    HashLink* current = map->table[idx];
    
    //if bucket is empty, return 0
    if (current == NULL){
        //hashLinkDelete(current);
        return 0;
    }
    
    //if bucket is not empty
    else {
        while (current != NULL){
            //if key matches
            if (strcmp(current->key, key) == 0){
                return 1;
            }
            else
                current = current->next;
        }
        //otherwise, we're at the end of the list and the key was not found
        return 0;
    }
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    
    //assert map exists
    assert(map != NULL);
    
    return map->size;
}
            
/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    //assert map exists
    assert(map != NULL);
    
    return map->capacity;
}
            
/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    //assert map exists
    assert(map != NULL);
    
    //count variable
    int countEmpties = 0;
    
    //create current link pointer
    HashLink* current = NULL;
    
    //for each index of map
    for (int i = 0; i < map->capacity; i++){
        
        //current points to bucket
        current = map->table[i];
        
        //if bucket is empty
        if (current == NULL){
            //count it
            countEmpties++;
        }
    }
    //hashLinkDelete(current);
    return countEmpties;
}
            
/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    //assert map exists
    assert(map != NULL);
    
    //number of links
    float num = hashMapSize(map);
    
    //number of buckets
    float denom = map->capacity;
    
    //ratio
    return num/denom;
}
            
/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
    //assert map exists
    assert(map != NULL);
    
    //create current link pointer
    HashLink* current = NULL;
    
    printf("\n");
    
    //for each index bucket
    for (int i = 0; i < map->capacity; i++){
        
        printf("Bucket #%d: \n", i+1);
	printf("___________\n");       
 
        //current points to bucket
        current = map->table[i];
        
        //if spot is empty
        if (current == NULL)
            printf("Empty\n");
        //else
        else{
            while(current != NULL){
                
                //print contents
                printf("%s : %d\n", current->key, current->value);
            
                //go to next spot
                current = current->next;
            }
        }
	printf("\n");
    }
}
