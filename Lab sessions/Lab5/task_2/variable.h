
typedef struct var var;

struct var {
  char *name;
  char *value;
  var *next;
}; 

var *addVar(var *var_list, var *data);
void printEnv(var *var_list);
var *removeVar(char* name, var *var_list);
var *locationVar(char* name, var *var_list);
void list_free(var *var_list);
