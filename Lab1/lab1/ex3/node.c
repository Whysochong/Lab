/*************************************
* Lab 1 Exercise 2
* Name:
* Student No:
* Lab Group:
*************************************/
 
#include "node.h" 
 
#include <ctype.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
 
// Add in your implementation below to the respective functions 
// Feel free to add any headers you deem fit (although you do not need to) 
long sum_helper(node *nde, int result);
int len_helper(node *nde, int length);
 
 
// Traverses list and returns the sum of the data values 
// of every node in the list. 
long sum_list(list *lst) { 
    return sum_helper(lst->head, 0);
} 
long sum_helper(node *nde, int result) {
    if (nde == NULL) {
        return result;
    } else {
        return sum_helper(nde->next, result + nde->data);
    }
}


 
// Traverses list and returns the number of data values in the list. 
int list_len(list *lst) { 
    return len_helper(lst->head, 0);
} 
int len_helper(node *nde, int length) {
    if (nde == NULL) {
        return length;
    } else {
        return len_helper(nde->next, length + 1);
    }
}
 
// Inserts a new node with data value at index (counting from head
// starting at 0).
// Note: index is guaranteed to be valid.
void insert_node_at(list *lst, int index, int data) { 

    node *node_to_add = (node*)malloc(sizeof(node)); 
  
    if (index == 0) { 
        node_to_add->data = data; 
        node_to_add->next = lst->head; 
        lst->head = node_to_add; 
        return; 
    } 

    if (lst->head == NULL) { 
        node_to_add->data = data; 
        node_to_add->next = NULL; 
        lst->head = node_to_add; 
        return; 
    } 
    
    node *curr_head = lst->head;

    // iterate through specified index
    for (int i = 0; i < index-1; i++) { 
        curr_head = curr_head->next; 
    } 
    
    node *next_node = curr_head->next; 
    node_to_add->data = data; 
    node_to_add->next = next_node; 
    curr_head->next = node_to_add;
    return; 
}

// Deletes node at index (counting from head starting from 0).
// Note: index is guarenteed to be valid.
void delete_node_at(list *lst, int index) { 

    node* curr_head = lst->head; 
    
    if (index == 0) { 
        node* delete_node = curr_head; 
        lst->head = lst->head->next; 
        free(delete_node); 
        return; 
    } 

    // iterate through till specified index
    for (int i = 0; i < index-1; i++) { 
        curr_head = curr_head->next; 
    } 
    
    node* delete_node = curr_head->next; 
    curr_head->next = delete_node->next; 
    
    free(delete_node);
    return; 
}
// Search list by the given element.
// If element not present, return -1 else return the index. If lst is empty return -2.
int search_list(list *lst, int element) { 

    // no list
    if (lst->head == NULL) { 
        return -2; 
    } 

    node *curr_head = lst->head; 
    int index = 0; 
    
    while (curr_head != NULL) { 
        if (curr_head->data == element) { 
            return index; 
        } 
        index++; 
        curr_head = curr_head->next; 
    } 
    // element not in list
    return -1; 
}

// Reverses the list with the last node becoming the first node. 
void reverse_list(list *lst) { 
    node *previous = NULL; 
    node *curr_head = lst->head; 
    
    while (curr_head != NULL) { 
        node *ch = curr_head-> next; 
        curr_head->next = previous; 
        previous = curr_head; 
        curr_head = ch; 
    } 

    lst->head = previous; 
    return; 
}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst) { 
    node *curr_head = lst->head; 

    while (curr_head != NULL) { 
        node *temp = curr_head; 
        curr_head = curr_head->next; 
        free(temp); 
    } 
    lst->head = NULL;
}

// Traverses list and applies func on data values of 
// all elements in the list. 
void map(list *lst, int (*func)(int)) { 
    node *curr_head = lst->head; 

    while (curr_head != NULL) { 
        curr_head->data = func(curr_head->data); 
        curr_head = curr_head->next; 
    } 
} 