/*
 * Binární vyhledávací strom — rekurzivní varianta
 *
 * S využitím datových typů ze souboru btree.h a připravených koster funkcí
 * implementujte binární vyhledávací strom pomocí rekurze.
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializace stromu.
 *
 * Uživatel musí zajistit, že inicializace se nebude opakovaně volat nad
 * inicializovaným stromem. V opačném případě může dojít k úniku paměti (memory
 * leak). Protože neinicializovaný ukazatel má nedefinovanou hodnotu, není
 * možné toto detekovat ve funkci. 
 */
void bst_init(bst_node_t **tree) {
    // Setting it to NULL means the BST has no nodes yet
    *tree = NULL;
}

/*
 * Vyhledání uzlu v stromu.
 *
 * V případě úspěchu vrátí funkce hodnotu true a do proměnné value zapíše
 * hodnotu daného uzlu. V opačném případě funkce vrátí hodnotu false a proměnná
 * value zůstává nezměněná.
 * 
 * Funkci implementujte rekurzivně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
    // Check if the current node is not null.
    if (tree) {
        // Check if the current node's key matches the search key.
        if (tree->key == key) {
            // If it matches, store the value in the provided address and return true.
            *value = tree->value;
            return true;
        }
        // Recursively search in the left and right subtrees.
        // If the key is found in either subtree, return true.
        if (bst_search(tree->left, key, value) || bst_search(tree->right, key, value)) {
            return true;
        }
    }
    // If the key is not found in the tree or the tree is empty, return false.
    return false;
}

/*
 * Vložení uzlu do stromu.
 *
 * Pokud uzel se zadaným klíče už ve stromu existuje, nahraďte jeho hodnotu.
 * Jinak vložte nový listový uzel.
 *
 * Výsledný strom musí splňovat podmínku vyhledávacího stromu — levý podstrom
 * uzlu obsahuje jenom menší klíče, pravý větší. 
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
    // Check if the current node (root or subtree) is NULL, indicating an insertion point.
    if (*tree == NULL) {
        // Dynamically allocate memory for the new node.
        *tree = (bst_node_t *)malloc(sizeof(struct bst_node));
        // Set the new node's key and value.
        (*tree)->key = key;
        (*tree)->value = value;
        // Initialize the left and right children of the new node to NULL.
        (*tree)->left = NULL;
        (*tree)->right = NULL;
    } else if (key < (*tree)->key) {
        // If the key is less than the current node's key, recurse on the left subtree.
        bst_insert(&(*tree)->left, key, value);
    } else if (key > (*tree)->key) {
        // If the key is greater than the current node's key, recurse on the right subtree.
        bst_insert(&(*tree)->right, key, value);
    } else {
        // If the key already exists in the tree, update the node's value.
        (*tree)->value = value;
    }
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 * 
 * Klíč a hodnota uzlu target budou nahrazeny klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 * 
 * Tato pomocná funkce bude využitá při implementaci funkce bst_delete.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
    // Check if the right child of the current node is NULL,
    // indicating that the current node is the rightmost node.
    if ((*tree)->right == NULL) {
        // Replace the target node's key and value with that of the rightmost node.
        target->key = (*tree)->key;
        target->value = (*tree)->value;
        // Free the memory allocated for the rightmost node and set it to NULL.
        free((*tree));
        (*tree) = NULL;
    } else {
        // If the right child exists, recursively call the function to find the rightmost node.
        bst_replace_by_rightmost(target, &(*tree)->right);
    }
}

/*
 * Odstranění uzlu ze stromu.
 *
 * Pokud uzel se zadaným klíčem neexistuje, funkce nic nedělá.
 * Pokud má odstraněný uzel jeden podstrom, zdědí ho rodič odstraněného uzlu.
 * Pokud má odstraněný uzel oba podstromy, je nahrazený nejpravějším uzlem
 * levého podstromu. Nejpravější uzel nemusí být listem.
 * 
 * Funkce korektně uvolní všechny alokované zdroje odstraněného uzlu.
 * 
 * Funkci implementujte rekurzivně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key) {
    // Check if the current node's key matches the key to be deleted.
    if ((*tree)->key == key) {
        //Node with only a left child or no child.
        if ((*tree)->right == NULL) {
            if ((*tree)->left != NULL) {
                // Node has only a left child.
                bst_node_t *tmp = (*tree)->left;
                (*tree)->key = (*tree)->left->key;
                (*tree)->value = (*tree)->left->value;
                (*tree)->right = (*tree)->left->right;
                (*tree)->left = (*tree)->left->left;
                free(tmp);
                tmp = NULL;
            } else {
                // Node is a leaf.
                free(*tree);
                *tree = NULL;
            }
        } else {
            //Node with only a right child.
            if ((*tree)->left == NULL) {
                bst_node_t *tmp = (*tree)->right;
                (*tree)->key = (*tree)->right->key;
                (*tree)->value = (*tree)->right->value;
                (*tree)->left = (*tree)->right->left;
                (*tree)->right = (*tree)->right->right;
                free(tmp);
                tmp = NULL;
            } else {
                //Node with two children.
                bst_replace_by_rightmost(*tree, &(*tree)->left);
            }
        }
    } else if (key < (*tree)->key && (*tree)->left != NULL) {
        // If the key to be deleted is less than the current node's key, recurse on the left subtree.
        bst_delete(&(*tree)->left, key);
    } else if (key > (*tree)->key && (*tree)->right != NULL) {
        // If the key to be deleted is greater than the current node's key, recurse on the right subtree.
        bst_delete(&(*tree)->right, key);
    } else if (*tree == NULL) {
        // If the tree is empty, simply return.
        return;
    }
}

/*
 * Zrušení celého stromu.
 * 
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po 
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených 
 * uzlů.
 * 
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree) {
    // Check if the current node (root or subtree) is NULL.
    // If it is, there's nothing to free, so return immediately.
    if ((*tree) == NULL) {
        return;
    }
    // Recursively call bst_dispose on the left subtree.
    bst_dispose(&(*tree)->left);
    // Recursively call bst_dispose on the right subtree.
    bst_dispose(&(*tree)->right);
    // free the current node itself.
    free((*tree));
    (*tree) = NULL;
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items) {
    if(tree == NULL){ return; }
    bst_add_node_to_items(tree, items);
    bst_preorder(tree->left, items);
    bst_preorder(tree->right, items);

}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items) {
    if(tree == NULL){ return; }
    bst_inorder(tree->left, items);
    bst_add_node_to_items(tree, items);
    bst_inorder(tree->right, items);
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items) {
    if(tree == NULL){ return; }
    bst_postorder(tree->left, items);
    bst_postorder(tree->right, items);
    bst_add_node_to_items(tree, items);
}
