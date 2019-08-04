/*
 
 Jake Seawell
 03/10/19
 
 This is a spellChecker function that uses a dictionary file,
 a hashMap to store each disctionary word, and a levenshtein
 algorithm to calculate how far away each dictionary entry is
 from a user inputted word. The spellChecker is case-insensitive,
 and will continue to run until the user enters "quit".
 
*/


#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 
 Levenshtein Distance Algorithm obtained from:
 https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
 
*/
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshtein(char *s1, char *s2) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int column[s1len+1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    return(column[s1len]);
}



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
            //cast character into lowercase letter
            c = tolower(c);
            
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
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
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    //asset file is not NULL
    assert (file != NULL);
    
    //get first word from file
    char* next = nextWord(file);
    
    //while file is not empty
    while (next != NULL){
        //put word in map, with a value of 1000
        hashMapPut(map, next, 1000);
	
        //get next word from file
        next = nextWord(file);
    }
    
    //print map
    //hashMapPrint(map);
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
    //create new map w/ 1000 buckets
    HashMap* map = hashMapNew(1000);
    
    //open dictionary -> set to file
    FILE* file = fopen("dictionary.txt", "r");
    
    //start timer
    clock_t timer = clock();
    
    //load dictionary
    loadDictionary(file, map);
    
    //stop timer
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    
    //close file (dictionary)
    fclose(file);
    
    //user input
    char inputBuffer[256];
    char inputLC[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        
        //get user input for word to search
        scanf("%s", inputLC);
        //clear input buffer
        while ((getchar()) != '\n');
 
        //convert input to all lowercase
        for (int i = 0; i < sizeof(inputLC)/sizeof(inputLC[0]); i++){
            inputBuffer[i] = tolower(inputLC[i]);
        }
        
        // Implement the spell checker code here..
        
        //if user types quit
        if (strcmp(inputBuffer, "quit") == 0)
        {
            //quit
            quit = 1;
        }
        //if user types anything else
        else
        {
            //create current pointer
            HashLink* current = NULL;
            //levenshtein # (initialized)
            int lev = 1000;
            //how many suggested words (initialized)
            int j = 0;
            //array that holds suggested words (initialized)
            char* strArray[5];
            //bool to track if word is found (spelled correctly)
            int found = 0;

            
            
/**
 * Searches the hashMap for correct spelling. If found, it will state such.
 * If not found, it will look for words "num" letters away from correct spelling.
 * This function can be called with multiple "num" values until suggested array is full.
 
 * @param num (Levenshtein #, or number of letters away from correct spelling)
 * @return None
 */
void suggestedNum(int num){
    //for each bucket in table
    for (int i = 0; i < map->capacity; i++){
        
        //current pointer to link at index
        current = map->table[i];
        
        //while spot is not empty
        while (current != NULL && found == 0) {
            
            //find levenshtein #
            lev = levenshtein(inputBuffer, current->key);
    
            //change value to lev #
            current->value = lev;
        
            //if lev is zero
            if (lev == 0){
                
                //print and exit search
                printf("%s is spelled correctly.\n", inputBuffer);
                found = 1;
            }
            //otherwise
            else {
                //if lev is num letters away from correct spelling
                if (lev == num && j < 5){
                    //add it to suggested array
                    strArray[j] = current->key;
                    //increment j (count of sugg. array)
                    j++;
                }
            }
            //traverse bucket
            current = current->next;
        }
    }
 }

            
        //start looking at 1 letter away from correct spelling
		int start = 1;
		
        //while sugg. array is not full and correct spelling not found
        while (j < 5 && found != 1){
			//check spelling
            suggestedNum(start);
			//increment num to look for other words
            start++;
		}

            //if word was not found
            if (found == 0){
                printf("%s is spelled incorrectly.\n", inputBuffer);
			
                //if no words were within 2 letters
                if (j < 1)
                    printf("I'm sorry, but we could not find any suggested words. Try again.\n");
                //otherwise, print list of suggested words
                else{
                    printf("Did you mean:\n");
                    for (int i = 0; i < j; i++){
                        printf("%s?\n", strArray[i]);
                    }
                    //if less than five words were found
                    
                    //right now, this line should never run, because we are looping through dict
                    //until all 5 suggested words are found
                    if (j < 5)
                        printf("I'm sorry, but we could only find %d suggested words. Try again.\n", j);
                }
            }
            
        }
        //uncomment to print map (if necessary)
        //hashMapPrint(map);
    }
    //delete map
    hashMapDelete(map);
    return 0;
}
