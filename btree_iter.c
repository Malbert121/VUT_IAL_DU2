/*
 * Binární vyhledávací strom — iterativní varianta
 *
 * S využitím datových typů ze souboru btree.h, zásobníku ze souboru stack.h 
 * a připravených koster funkcí implementujte binární vyhledávací 
 * strom bez použití rekurze.
 */

#include "../btree.h"
#include "stack.h"
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
    *tree = NULL;
}

/*
 * Vyhledání uzlu v stromu.
 *
 * V případě úspěchu vrátí funkce hodnotu true a do proměnné value zapíše
 * hodnotu daného uzlu. V opačném případě funkce vrátí hodnotu false a proměnná
 * value zůstává nezměněná.
 * 
 * Funkci implementujte iterativně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
    // Temporary pointer to keep track of the original root of the tree.
    bst_node_t *tmp = tree;

    // Iterate through the tree as long as the current node is not NULL.
    while (tree != NULL) {
        // Check if the current node's key matches the search key.
        if (tree->key == key) {
            // If the key is found and value is not NULL, store the value in the provided address.
            if (value != NULL) {
                *value = tree->value;
            }
            // Reset the tree pointer to the original root before returning.
            tree = tmp;
            return true;
        }
        // Move to the left or right child depending on the value of the key.
        tree = (key < tree->key) ? tree->left : tree->right;
    }

    // Reset the tree pointer to the original root before returning.
    tree = tmp;
    // Return false if the key is not found in the tree.
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
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
    bst_node_t *newNode = (bst_node_t *)malloc(sizeof(struct bst_node));
    // Set the key and value of the new node.
    newNode->key = key;
    newNode->value = value;
    // Initialize the left and right children of the new node to NULL.
    newNode->left = NULL;
    newNode->right = NULL;

    // If the tree is empty, set the new node as the root and return.
    if ((*tree) == NULL) {
       (*tree) = newNode;
       return;
    }

    // Temporary pointer to traverse the tree without losing the original root.
    bst_node_t *tmp = *tree;
    // Iterate through the tree to find the correct position for the new node.
    while (tmp != NULL) {
    // If a node with the same key is found, update its value and free the new node.
    if ((*tree)->key == key) {
        (*tree)->value = value;
        free(newNode);
        break;
    }

    // Navigate to the left or right child based on the key comparison.
    if (key < (*tree)->key) {
        // If the left child is NULL, insert the new node here.
        if ((*tree)->left == NULL) {
            (*tree)->left = newNode;
            break;
        } else {
            // Move to the left child for further comparison.
            (*tree) = (*tree)->left;
        }
    } else {
        // If the right child is NULL, insert the new node here.
        if ((*tree)->right == NULL) {
            (*tree)->right = newNode;
            break;
        } else {
            // Move to the right child for further comparison.
            (*tree) = (*tree)->right;
        }
    }
}
    // Reset the tree pointer to the original root of the tree.
    *tree = tmp;
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 * 
 * Klíč a hodnota uzlu target budou nahrazené klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 * 
 * Tato pomocná funkce bude využita při implementaci funkce bst_delete.
 *
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
    // Pointers to keep track of the current node and its parent during traversal.
    bst_node_t *cur = *tree; // Current node, starting at the root of the subtree.
    bst_node_t *par = NULL;  // Parent of the current node.
    // Loop indefinitely (will break out of the loop internally).
    while (1) {
        // Check if the right child of the current node is NULL,
        // indicating that the current node is the rightmost node.
        if (cur->right == NULL) {
            // Replace the target node's key and value with those of the rightmost node.
            target->key = cur->key;
            target->value = cur->value;
            // Link the parent of the rightmost node to the left child of the rightmost node.
            // This step removes the rightmost node from the tree.
            if (par != NULL) {
                par->right = cur->left;
            }
            // Free the memory of the rightmost node and set the pointer to NULL.
            free(cur);
            cur = NULL;
            // Break out of the loop as the replacement is done.
            break;
        }

        // Move to the right child of the current node and update the parent.
        par = cur;
        cur = cur->right;
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
 * Funkci implementujte iterativně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key) {
    // Pointers to keep track of the current node and its parent during traversal.
    bst_node_t *cur = *tree; // Current node, starting at the root of the tree.
    bst_node_t *par = NULL;  // Parent of the current node.

    // Iterate through the tree as long as the current node is not NULL.
    while (cur) {
        // Check if the current node's key matches the key to be deleted.
        if (key == cur->key) {
            // Case 1: Node with only right child or no child.
            if (cur->right == NULL) {
                // If parent is NULL, it means cur is the root node.
                if (par == NULL) {
                    *tree = cur->left;
                } else {
                    // Link the parent node to the left child of the current node.
                    par->left = cur->left;
                }
                // Free the memory of the current node and set it to NULL.
                free(cur);
                cur = NULL;
            } else {
                // Case 2: Node with only left child.
                if (cur->left == NULL) {
                    // If parent is NULL, it means cur is the root node.
                    if (par == NULL) {
                        *tree = cur->right;
                    } else {
                        // Link the parent node to the right child of the current node.
                        par->right = cur->right;
                    }
                    // Free the memory of the current node.
                    free(cur);
                    cur = NULL;
                } else {
                    // Case 3: Node with two children.
                    // Replace current node with the rightmost node in its left subtree.
                    bst_replace_by_rightmost(cur, &(cur->left));
                }
            }
            break; // Node found and handled; exit the loop.
        }
        // Traverse the tree to find the node to be deleted.
        else if (key < cur->key) {
            par = cur;
            cur = cur->left;
        } else {
            par = cur;
            cur = cur->right;
        }
    }
}

/*
 * Zrušení celého stromu.
 * 
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po 
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených 
 * uzlů.
 * 
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití 
 * vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree) {
    if (*tree == NULL) return;
    stack_bst_t stack;
    stack_bst_init(&stack);
    do {
        if(*tree == NULL){
            if(!stack_bst_empty(&stack)){
                *tree = stack_bst_top(&stack);
                stack_bst_pop(&stack);
            }
        } else{
            if((*tree)->right != NULL){
                stack_bst_push(&stack, (*tree)->right);
            }
            bst_node_t *tmp = *tree;
            *tree = (*tree)->left;
            free(tmp);
        }

    } while ((*tree != NULL) || (!stack_bst_empty(&stack)));
}

/*
 * Pomocná funkce pro iterativní preorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu.
 * Nad zpracovanými uzly zavolá bst_add_node_to_items a uloží je do zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití 
 * vlastních pomocných funkcí.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit, bst_items_t *items) {
    while (tree != NULL){
        stack_bst_push(to_visit, tree);
        bst_add_node_to_items(tree, items);
        tree = tree->left;
    }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_preorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items) {
    stack_bst_t stack;
    stack_bst_init(&stack);
    bst_leftmost_preorder(tree, &stack, items);
    while (!stack_bst_empty(&stack)){
        tree = stack_bst_top(&stack);
        stack_bst_pop(&stack);
        bst_leftmost_preorder(tree->right, &stack, items);
    }
}

/*
 * Pomocná funkce pro iterativní inorder.
 * 
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití 
 * vlastních pomocných funkcí.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {
    while (tree != NULL){
        stack_bst_push(to_visit, tree);
        tree = tree->left;
    }
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_inorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items) {
    stack_bst_t stack;
    stack_bst_init(&stack);
    bst_leftmost_inorder(tree, &stack);
    while (!stack_bst_empty(&stack)){
        tree = stack_bst_top(&stack);
        stack_bst_pop(&stack);
        bst_add_node_to_items(tree, items);
        bst_leftmost_inorder(tree->right, &stack);
    }
}

/*
 * Pomocná funkce pro iterativní postorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů. Do zásobníku bool hodnot ukládá informaci, že uzel
 * byl navštíven poprvé.
 *
 * Funkci implementujte iterativně pomocí zásobníku uzlů a bool hodnot a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,stack_bool_t *first_visit) {
    while (tree != NULL){
        stack_bst_push(to_visit, tree);
        stack_bool_push(first_visit, true);
        tree = tree->left;
    }
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_postorder a
 * zásobníku uzlů a bool hodnot a bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items) {
    bool fromLeft;
    stack_bst_t bst_stack;
    stack_bool_t bool_stack;
    stack_bst_init(&bst_stack);
    stack_bool_init(&bool_stack);
    bst_leftmost_postorder(tree, &bst_stack, &bool_stack);
    while (!stack_bst_empty(&bst_stack)){
        tree = stack_bst_top(&bst_stack);
        fromLeft = stack_bool_top(&bool_stack);
        stack_bool_pop(&bool_stack);
        if(fromLeft){
            stack_bool_push(&bool_stack, false);
            bst_leftmost_postorder(tree->right, &bst_stack, &bool_stack);
        }else{
            stack_bst_pop(&bst_stack);
            bst_add_node_to_items(tree, items);
        }
    }
}
