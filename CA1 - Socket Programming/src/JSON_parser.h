#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "cJSON.h"
#include "consts.h"

typedef struct ingredient
{
    char* name;
    int count;
} ingredient;

typedef struct food
{
    char* name;
    ingredient *ingredients;
    int ingredient_count;
} food;

typedef struct Recipes
{
    food* foods;
    int food_counter;
}Recipes;

char *read_file(const char *filename);
char **parse_food_names();
char **parse_ingridients();
void parse_food(cJSON *foodJson, food *food_struct);
Recipes get_foods();