/*
 * Tabulka s rozptýlenými položkami
 *
 * S využitím datových typů ze souboru hashtable.h a připravených koster
 * funkcí implementujte tabulku s rozptýlenými položkami s explicitně
 * zretězenými synonymy.
 *
 * Při implementaci uvažujte velikost tabulky HT_SIZE.
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptylovací funkce která přidělí zadanému klíči index z intervalu
 * <0,HT_SIZE-1>. Ideální rozptylovací funkce by měla rozprostírat klíče
 * rovnoměrně po všech indexech. Zamyslete sa nad kvalitou zvolené funkce.
 */
int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) {
    result += key[i];
  }
  return (result % HT_SIZE);
}

/*
 * Inicializace tabulky — zavolá sa před prvním použitím tabulky.
 */
void ht_init(ht_table_t *table) {
    unsigned index = 0;
    while(index < HT_SIZE){
        // Set the current slot in the hash table to NULL.
        (*table)[index++] = NULL;
        // The index is incremented after the assignment is done.
    }
}
/*
 * Vyhledání prvku v tabulce.
 *
 * V případě úspěchu vrací ukazatel na nalezený prvek; v opačném případě vrací
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {
    // Retrieve the initial item from the hash table using the hash of the key.
    // 'get_hash' is assumed to be a function that converts the key into a hash value.
    ht_item_t *item = (*table)[get_hash(key)];

    // Loop as long as 'item' is not NULL (i.e., end of chain not reached).
    while (item) {
        // Check if the current item's key matches the search key.
        if (!strcmp(key, item->key)) {
            // If the keys match, return the current item.
            return item;
        }

        // Move to the next item in the chain.
        // 'item->next' points to the next item in the linked list.
        item = item->next;
    }

    // If the key was not found in the hash table, return NULL.
    return NULL;
}

/*
 * Vložení nového prvku do tabulky.
 *
 * Pokud prvek s daným klíčem už v tabulce existuje, nahraďte jeho hodnotu.
 *
 * Při implementaci využijte funkci ht_search. Pri vkládání prvku do seznamu
 * synonym zvolte nejefektivnější možnost a vložte prvek na začátek seznamu.
 */
void ht_insert(ht_table_t *table, char *key, float value) {
    // First, try to find if the key already exists in the table.
    ht_item_t *item_find = ht_search(table, key);

    // If the key is found, update its value and return.
    if(item_find != NULL){
        item_find->value = value;
        return;
    }

    // If the key is not found, create a new item.
    ht_item_t *new_item = (ht_item_t *)malloc(sizeof(ht_item_t));

    // Check if memory allocation was successful.
    if (!new_item) {
        return;
    }

    // Initialize the new item with the provided key and value.
    new_item->key = key;
    new_item->value = value;
    new_item->next = NULL; // Next pointer is initially NULL.

    // Retrieve the head of the chain for the hashed key.
    item_find = (*table)[get_hash(key)];

    // If the head exists, insert the new item at the beginning of the chain.
    if (item_find){
        new_item->next = item_find;
    }

    // Set the new item as the new head of the chain.
    (*table)[get_hash(key)] = new_item;
}

/*
 * Získání hodnoty z tabulky.
 *
 * V případě úspěchu vrací funkce ukazatel na hodnotu prvku, v opačném
 * případě hodnotu NULL.
 *
 * Při implementaci využijte funkci ht_search.
 */
float *ht_get(ht_table_t *table, char *key) {
    // Use ht_search to find the item associated with the given key.
    ht_item_t *item_find = ht_search(table, key);

    // Check if the item was found.
    if (item_find != NULL) {
        return &item_find->value;
    }
    return NULL;
}
/*
 * Smazání prvku z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje přiřazené k danému prvku.
 * Pokud prvek neexistuje, funkce nedělá nic.
 *
 * Při implementaci NEPOUŽÍVEJTE funkci ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {
    // Retrieve the head of the list for the hashed key.
    ht_item_t *item_find = (*table)[get_hash(key)];

    // Create an array to hold pointers for current and previous items.
    ht_item_t *items[3] = {[0] = NULL, [1] = NULL};

    // Iterate through the chain of items.
    while(item_find != NULL){
        // Check if the current item's key is not the key to be deleted.
        if (strcmp(key, item_find->key)) {
            items[0] = item_find;
            item_find = item_find->next;
            // Store the next item (if deletion occurs, this will be linked to the previous item).
            items[1] = item_find ? item_find->next : NULL;
        } else {
            // If the current item is the one to delete, free its memory.
            free(item_find);
            item_find = NULL;
            // Link the previous item to the next item, effectively removing the current item from the list.
            if (items[0]) {
                items[0]->next = items[1];
            }
            break;
        }
    }
}

/*
 * Smazání všech prvků z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje a uvede tabulku do stavu po 
 * inicializaci.
 */
void ht_delete_all(ht_table_t *table) {
    // Initialize an index to iterate over the hash table.
    unsigned index = 0;

    // Declare a pointer to hold the current item.
    ht_item_t *item;

    // Loop over each slot in the hash table.
    while (index < HT_SIZE) {
        // Check if the current slot is not empty.
        if ((*table)[index] != NULL) {
            // Point to the next item in the chain.
            item = (*table)[index]->next;
            free((*table)[index]);
            (*table)[index] = item;
            // Continue to the next iteration to check if there are more items in the same slot.
            continue;
        }
        // Increment the index to move to the next slot in the hash table.
        index++;
    }
}
