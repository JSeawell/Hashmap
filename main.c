/*
 
 Jake Seawell
 03/10/19
 
 This is a concordance function to count how many times each word
 occurs in a certain text file. It will load a file from the user,
 put each word into a hashMap bucket, and then keep a count of how
 many times the word occurs.
 
*/


#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
  
   while (1) 
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')  
        {
            //makes each character from file lowercase
            c = tolower(c);
	    if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            // .............
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Prints the concordance of the given file and performance information. Uses
 * the file input1.txt by default or a file name specified as a command line
 * argument.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    //default filename - user can specify if other is wanted
    const char* fileName = "input1.txt";
    
    if (argc > 1)
    {
        fileName = argv[1];
    }
    printf("Opening file: %s\n", fileName);
    
    //start timer
    clock_t timer = clock();
   
    // --- Concordance code begins here ---
    // Be sure to free the word after you are done with it here.
     
    //pointer to file
    FILE *a_file;
    //count variable
    int count = 0;
    //create new hash map with 10 buckets
    HashMap* map = hashMapNew(10);
    
    //open file
    a_file = fopen(fileName, "r");
    
    //next gets first word of file
    char* next = nextWord(a_file);

    //assert file is not NULL
    assert (a_file != NULL);

    
    //until the end of the file
    while (next != NULL){
	
        //reset count
        count = 0;
	
        //if map already contains next word
        if (hashMapContainsKey(map, next) == 1){
            //get the current count of word
            count = *hashMapGet(map, next);
        }
	
        //update count by adding 1 to current count
        hashMapPut(map, next, count + 1);
	
        //Uncomment this to print the whole file
        //printf("%s ", next);
	
        //move to next word in file
        next = nextWord(a_file);
    }
    
    //close file
    fclose(a_file);

    //print map
	hashMapPrint(map);
    
    //free next ptr
    free(next);
    
    // --- Concordance code ends here ---
    
    //display results
    timer = clock() - timer;
    printf("\nRan in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    printf("Empty buckets: %d\n", hashMapEmptyBuckets(map));
    printf("Number of links: %d\n", hashMapSize(map));
    printf("Number of buckets: %d\n", hashMapCapacity(map));
    printf("Table load: %f\n", hashMapTableLoad(map));
    
    //delete maps
    hashMapDelete(map);
    return 0;
}

