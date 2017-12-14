/**
 * Implements a dictionary's functionality.
 */

#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "dictionary.h"

unsigned int count = 0;

bool loaded = false;
//we will be using a hashtable, and here we define the node structure and the number of nodes in the array/hashtable

typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

node *hashtable[HASHTABLESIZE];

//hash function taken from: https://www.reddit.com/r/cs50/comments/1x6vc8/pset6_trie_vs_hashtable/cf9nlkn
int hash_it(char* needs_hashing)
{
    unsigned int hash = 0;
    for (int i=0, n=strlen(needs_hashing); i<n; i++)
        {
            hash = (hash << 2) ^ needs_hashing[i];
        }
    return hash % HASHTABLESIZE;
}

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char *word)
{
    //remedies the incompatible integer to pointer conversion error received on line 39 when word is passed to hash_it
    char wordcpy[strlen(word) + 1];
    for (int i=0; i<strlen(word); i++)
    {
        wordcpy[i] = word[i];
    }


    //gets the hash value of the word
    int hash = hash_it(wordcpy);

    //traverses the linked list, searching for a word
    node *cursor = hashtable[hash];
    while (cursor != NULL)
    {
        if (strcasecmp(cursor->word, wordcpy) == 0)
        {
            return true;
        }

        else
        {
            cursor = cursor->next;
        }

    }

    return false;
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char *dictionary)
{
    //initialize buckets of hashtable with null pointers
    for (int i = 0; i < HASHTABLESIZE; i++)
    {
        hashtable[i] = NULL;
    }

    //open up the dictionary file
    FILE* file = fopen(dictionary, "r");
    if (file == NULL)
    {
        printf("Invalid dictionary file \n");
        return false;
    }


    //scan dictionary word by word
    int numiterations = 0;
    int maxiter = 300;
    while (true)
    {

        numiterations++;
        if (numiterations > maxiter)
        {
            printf("max iterations reached");
            return false;
        }
        // allocate memory for a node for each new word
        node* new_node = malloc(sizeof(node));

        if (new_node == NULL)
        {
            printf("unloading dictionary\n");
            unload();
            return false;
        }

        if (feof(file))
        {
            free(new_node);
            break;
        }

        //reads a word from the dictionary file and strores it in a pointer to word
        fscanf(file, "%s", new_node->word);
        new_node->next = NULL;

        //feeds the new word into our hash function, then adds it to our linked list
        int space = hash_it(new_node->word);
        node* head = hashtable[space];

        //if the linked list does not contain any words, we insert our new node as the head of the list
        if (head == NULL)
        {
            hashtable[space]= new_node;
        }

        //if the linked list does contain words, we insert our new node as the head of the list
        else
        {
            new_node->next = hashtable[space];
            hashtable[space] = new_node;
        }
        count++;
        fclose(file);
        loaded = true;


    }
    return true;

}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return count;
}

/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
bool unload(void)
{
    //free up the linked list
    for (int i=0; i<HASHTABLESIZE; i++)
    {
        node *cursor = hashtable[i];

        while (cursor != NULL)
        {
            node *temp = cursor;
            cursor = cursor->next;
            free(temp);
        }
    }
    loaded = false;
    return false;
}
