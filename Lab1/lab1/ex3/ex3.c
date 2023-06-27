/*************************************
* Lab 1 Exercise 3
* Name:
* Student No:
* Lab Group:
*************************************/

#include <stdio.h>
#include <stdlib.h>

#include "function_pointers.h"
#include "node.h"

// The runner is empty now! Modify it to fulfill the requirements of the
// exercise. You can use ex2.c as a template

// DO NOT initialize the func_list array in this file. All initialization
// logic for func_list should go into function_pointers.c.

// Macros
#define SUM_LIST 0
#define INSERT_AT 1
#define DELETE_AT 2
#define SEARCH_LIST 3
#define REVERSE_LIST 4
#define RESET_LIST 5
#define LIST_LEN 6
#define MAP 7

void print_index(int index);


int main(int argc, char *argv[]) { 
    if (argc != 2) { 
        fprintf(stderr, "Error: expecting 1 argument, %d found\n", argc - 1); 
        exit(1); 
    } 
 
    char *filename = argv[1];  
    FILE *fp = fopen(argv[1], "r");  
  
    if (!fp) {  
        fprintf(stderr, "Error: unable to instren file %s/n", filename);  
        exit(1);  
    }  
 
    // Initialize new list 
    list *lst = (list *)malloc(sizeof(list)); 
    lst->head = NULL; 
 
    // Update the array of function pointers  
    // DO NOT REMOVE THIS CALL  
    // (You may leave the function empty if you do not need it)  
    update_functions();  
  
    int instr;  
    while (fscanf(fp, "%d", &instr) == 1) {  
        int index, data, element, func_idx, len, ind; 
        long result;
        switch (instr) {  
            case SUM_LIST:  
                result = sum_list(lst);  
                printf("%ld\n", result);
                break;  
            case DELETE_AT:  
                fscanf(fp, "%d", &index); 
                delete_node_at(lst, index);  
                break;  
            case SEARCH_LIST:  
                fscanf(fp, "%d", &element);  
                ind = search_list(lst, element);
                print_index(ind);
                break;  
            case REVERSE_LIST:  
                reverse_list(lst);  
                break;  
            case RESET_LIST:  
                reset_list(lst);  
                break;  
            case LIST_LEN:  
                len = list_len(lst); 
                printf("%d\n", len);
                break;
            case INSERT_AT:
                fscanf(fp, "%d %d", &index, &data); 
                insert_node_at(lst, index, data);
                break;
            case MAP:  
                fscanf(fp, "%d", &func_idx);  
                map(lst, func_list[func_idx]);  
                break;  
        }         
    }  
 
    reset_list(lst); 
    free(lst); 
    fclose(fp);     
}

//Print index
void print_index(int index)
{
    if(index == -2)
        printf("{}\n");
    else
        printf("{%d}\n", index);
}

