/*
 * Použití binárních vyhledávacích stromů.
 *
 * S využitím Vámi implementovaného binárního vyhledávacího stromu (soubory ../iter/btree.c a ../rec/btree.c)
 * implementujte funkci letter_count. Výstupní strom může být značně degradovaný (až na úroveň lineárního seznamu) 
 * a tedy implementujte i druhou funkci (bst_balance), která strom, na požadavek uživatele, vybalancuje.
 * Funkce jsou na sobě nezávislé a tedy automaticky NEVOLEJTE bst_balance v letter_count.
 * 
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>


/**
 * Vypočítání frekvence výskytů znaků ve vstupním řetězci.
 * 
 * Funkce inicilializuje strom a následně zjistí počet výskytů znaků a-z (case insensitive), znaku 
 * mezery ' ', a ostatních znaků (ve stromu reprezentováno znakem podtržítka '_'). Výstup je v 
 * uložen ve stromu.
 * 
 * Například pro vstupní řetězec: "abBccc_ 123 *" bude strom po běhu funkce obsahovat:
 * 
 * key | value
 * 'a'     1
 * 'b'     2
 * 'c'     3
 * ' '     2
 * '_'     5
 * 
 * Pro implementaci si můžete v tomto souboru nadefinovat vlastní pomocné funkce.
*/
void letter_count(bst_node_t **tree, char *input) {
    // Array to keep track of character counts. Initialized to 0.
    int value[256] = {[255] = 0};
    // Initialize the BST.
    bst_init(tree);
    // Loop through each character in the input string.
    while (*input != '\0') {
        // Determine the character to be counted.
        char in = (*input <= 'z' && *input >= 'a') || *input == ' ' ? *input :
                  (*input <= 'Z' && *input >= 'A') ? *input + ' ' : '_';
        bst_insert(tree, in, ++value[(int)in]);
        // Move to the next character in the input string.
        input++;
    }
}



/**
 * Vyvážení stromu.
 * 
 * Vyvážený binární vyhledávací strom je takový binární strom, kde hloubka podstromů libovolného uzlu se od sebe liší maximálně o jedna.
 * 
 * Předpokládejte, že strom je alespoň inicializován. K získání uzlů stromu využijte vhodnou verzi vámi naimplmentovaného průchodu stromem.
 * Následně můžete například vytvořit nový strom, kde pořadím vkládaných prvků zajistíte vyváženost.
 *  
 * Pro implementaci si můžete v tomto souboru nadefinovat vlastní pomocné funkce. Není nutné, aby funkce fungovala *in situ* (in-place).
*/
int compare(const void *a, const void *b) {
    return (*(char *)a - *(char *)b);
}

void bst_balance(bst_node_t **tree) {
    // Array to keep track of character counts.
    int value[256] = {[255] = 0};

    // Variable to keep track of the number of unique characters in the tree.
    unsigned size = 0;

    // Dynamically allocate memory for storing characters.
    char *chars = (char *)malloc(sizeof(char) + 2);

    // Iterate over the ASCII characters (up to 126).
    for (char i = 0; i < 126; ++i) {
        // Check if the character is in the tree and get its frequency.
        if (bst_search(*tree, i, &value[(int)i])) {
            // If the character is found, add it to the chars array and increase its size.
            chars[size++] = i;
            chars = (char *)realloc(chars, sizeof(char) * size + 2);
        }
    }
    // Null-terminate the chars array.
    chars[size] = '\0';

    // Sort the characters in the chars array.
    qsort(chars, size, sizeof(char), compare);

    // Create a new BST for balancing.
    bst_node_t **new_tree = (bst_node_t **)malloc(sizeof(bst_node_t *));
    bst_init(new_tree);

    // Insert the middle character from the sorted array into the new tree.
    bst_insert(new_tree, chars[size / 2], value[(int)chars[size / 2]]);

    // Balance the tree by inserting characters from the sorted array into the new tree.
    for (int i = 0; i + (size / 2) <= size; ++i) {
        // Insert characters to the left of the middle character.
        if ((int)(size / 2) - i >= 0) {
            bst_insert(new_tree, chars[size / 2 - i], value[(int)chars[size / 2 - i]]);
        }
        // Insert characters to the right of the middle character, excluding the middle character itself.
        if (!((int)(i + size / 2) == size)) {
            bst_insert(new_tree, chars[size / 2 + i], value[(int)chars[size / 2 + i]]);
        }
    }

    // Free the memory allocated for the chars array.
    free(chars);

    // Dispose of the original tree.
    bst_dispose(tree);

    // Replace the original tree with the new balanced tree.
    *tree = *new_tree;

    // Free the memory allocated for the new_tree pointer.
    free(new_tree);
}