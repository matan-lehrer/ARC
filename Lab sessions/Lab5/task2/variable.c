#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "variable.h"

var *addVar(var *var_list, var *data)
{  
  var *var_list_temp = locationVar(data->name,var_list);
  if(var_list_temp == NULL)
  {
    data->next = var_list;
    return data;
  }
  else
  {
    strcpy(var_list_temp->value, data->value);
    
    free(data->name);
    free(data->value);
    free(data);
    
    return var_list;
  }
}

void printEnv(var *var_list)
{
  while(var_list!=NULL)
  {
    printf("NAME: %s VALUE: %s\n", var_list->name, var_list->value);
    var_list = var_list->next;
  }
}

var *removeVar(char* name, var *var_list)
{

  var *iter1 = var_list, *iter2 = NULL;
  
  while ( iter1 != NULL ) 
  {
    
    if ( strcmp ( name, iter1->name ) == 0 ) 
    {
      if ( iter2 == NULL ) /*first*/
      {
	
	free ( iter1->name );
	free ( iter1->value );
	iter2 = iter1->next;
	free ( iter1 );
	
	return iter2;
      }
      else
      {
	
	iter2->next = iter1->next;

	free ( iter1->name );
	free ( iter1->value );
	free ( iter1 );
	
	return var_list;
      }
    }
    
    iter2 = iter1;
    iter1 = iter1->next;
  }
  
  printf("name: %s is not exist\n", name);
  return var_list;
}

var *locationVar(char* name, var *var_list)
{
  while(var_list != NULL)
  {
    if(strcmp(var_list->name, name)==0)
    {
      return var_list;
    }
    var_list = var_list->next;
  }
  return NULL;
}

void list_free(var *var_list)
{
  if(var_list!=NULL)
  {
    list_free(var_list->next);
    free(var_list->name);
    free(var_list->value);
    free(var_list);
  }
}