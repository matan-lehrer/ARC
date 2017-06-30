#include "variable.h"

void addVar(var** var_list, char *name, char *value){
    if (*var_list == NULL){
        var* var_toAdd = malloc(sizeof(var));

        var_toAdd->name = name;
        var_toAdd->value = value;

        var_toAdd->next = NULL;
        *var_list = var_toAdd;
    }
    else{
        if(replaceValueByName(var_list, name, value) == -1){
            var *var_toAdd = malloc(sizeof(var));
            var_toAdd->name = name;
            var_toAdd->value = value;
            var_toAdd->next = NULL;
            var* list = *var_list;
            while (list->next != NULL){
                list = list -> next;
            }
            list -> next = var_toAdd;
        }

    }
}
void removeVar(var** var_list, const char* name){
    if (*var_list == NULL){
        perror("can't delete");
    }
    var* tmp_list = *var_list;
    if (strcmp(tmp_list->name, name)  == 0){
        var *tmp = tmp_list;
        *var_list = tmp_list -> next;
        free (tmp->name);
        free (tmp->value);
        free (tmp);
        return;
    }

    while (tmp_list->next != NULL){
        var *tmp = tmp_list -> next;
        if(strcmp(tmp->name, name)  == 0) {
            tmp_list -> next = tmp_list -> next -> next;

            free (tmp->name);
            free (tmp->value);
            free (tmp);
            return;
        }
        tmp_list = tmp_list->next;
    }

    perror("can't delete");
}
void printVar(var** var_list){
    var *tmp_list = *var_list;
    while (tmp_list != NULL) {
        printf("[%s]\t %s \t\n", tmp_list->name, tmp_list -> value);
        tmp_list = tmp_list->next;
    }
}
void freeVarList(var** var_list) {
    var *tmp_list = *var_list;
    while (tmp_list != NULL) {
        var *tmp = tmp_list;
        tmp_list = tmp_list->next;

        free(tmp->name);
        free(tmp->value);
        free(tmp);
    }
}
char* findValueByName(var ** var_list, const char* name){
    var *tmp_list = *var_list;
    while (tmp_list != NULL) {
        if(strcmp(tmp_list->name, name) == 0){
            return tmp_list->value;
        }
        tmp_list = tmp_list->next;
    }
    perror("cant find varible");
    exit(-1);
}

int replaceValueByName(var ** var_list, const char* name, const char* value){
    var *tmp_list = *var_list;
    while (tmp_list != NULL) {
        if(strcmp(tmp_list->name, name) == 0){
            strcpy(tmp_list->value, value);
            return 1;
        }
        tmp_list = tmp_list->next;
    }
    return -1;
}
