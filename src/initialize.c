#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/prototypes.h"

#define TYPES_FILE   "data/types.txt"
#define MOVES_FILE   "data/moves.txt"
#define POKEMON_FILE "data/pokemon.txt"


static int isDuplicate(const int arr[], int size, int val)
{
    
    for (int i = 0; i < size; i++) {
        if (arr[i] == val) return 1; 
    }
    return 0; 
}

static int typeIndex(Type Types[18], const char *name)
{
    for (int i = 0; i < 18; i++) {
        if (strcmp(Types[i].name, name) == 0) return i;
    }
    return -1; 
}





void initialize(Type Types[18], Move Moves[486], Pokemon Pokemons[1015],
                Player *p1, Player *p2)
{
    srand((unsigned)time(NULL));

    initializeTypes(Types);
    initializeMoves(Moves, Types);
    initializePokemons(Pokemons, Types, Moves);

    strcpy((*p1).name, "Player1");
    strcpy((*p2).name, "Player2");

    int pick1[6];
    int pick2[6];

    for (int i = 0; i < 6; i++) {
        pick1[i] = -1;
        pick2[i] = -1;
    }

    for (int i = 0; i < 6; i++) {
        int r;
        do {
            r = rand() % 1015;
        } while (isDuplicate(pick1, i, r));

        pick1[i] = r;
        (*p1).Pokemons[i] = Pokemons[r];
    }

    for (int i = 0; i < 6; i++) {
        int r;
        do {
            r = rand() % 1015;
        } while (isDuplicate(pick2, i, r) || isDuplicate(pick1, 6, r));

        pick2[i] = r;
        (*p2).Pokemons[i] = Pokemons[r];
    }

    (*p1).currentIndex = 1;
    (*p2).currentIndex = 1;
}


void initializeTypes(Type Types[18])
{
    FILE *fp = fopen(TYPES_FILE, "r");

    if (fp == NULL) {
        printf("types.txt didn't open!\n");
        exit(1);
    }

    char line[256];

    for (int i = 0; i < 18; i++) {

        if (fgets(line, sizeof(line), fp) == NULL) {
            printf("types.txt satir sayisi eksik.\n");
            exit(1);
        }

        while (line[0] == '\n' || line[0] == '\r') {
            if (fgets(line, sizeof(line), fp) == NULL) {
                printf("types.txt satir sayisi eksik.\n");
                exit(1);
            }
        }

        char atk[20];
        sscanf(line, "%19s", atk);

        strcpy(Types[i].name, atk);

        for (int j = 0; j < 18; j++) {

            if (fgets(line, sizeof(line), fp) == NULL) {
                printf("types.txt defender satiri eksik.\n");
                exit(1);
            }

            char def[20];
            float mult;
            sscanf(line, "%19s %f", def, &mult);

            strcpy(Types[i].effects[j].atkName, atk);
            strcpy(Types[i].effects[j].defName, def);
            Types[i].effects[j].multiplier = mult;
        }

        strcpy(Types[i].effects[18].atkName, atk);
        strcpy(Types[i].effects[18].defName, "None");
        Types[i].effects[18].multiplier = 1.0f;
    }
    
    fclose(fp);
}




void initializeMoves(Move Moves[486], Type Types[18])
{
    FILE *fp = fopen(MOVES_FILE, "r");
    if (fp == NULL) {
        printf("moves.txt didn't open!\n");
        exit(1);
    }

    char line[256];

    for (int i = 0; i < 486; i++) {

        /* 1 satır = 1 move */
        if (fgets(line, sizeof(line), fp) == NULL) {
            printf("moves.txt satir sayisi eksik.\n");
            exit(1);
        }

        /* satırdan 4 bilgi alacağız */
        char moveName[30];
        char typeName[20];
        char categoryName[20];
        int power;

        /* örn: FlameBurst Fire Special 70 */
        sscanf(line, "%29s %19s %19s %d", moveName, typeName, categoryName, &power);

        /* name ve power */
        strcpy(Moves[i].name, moveName);
        Moves[i].power = power;

        /* typeName string -> Type struct */
        int typePos = typeIndex(Types, typeName);
        if (typePos == -1) {
            printf("moves.txt icinde bilinmeyen type var: %s\n", typeName);
            exit(1);
        }
        Moves[i].type = Types[typePos];

        /* category string -> enum */
        if (strcmp(categoryName, "Physical") == 0 || strcmp(categoryName, "PHYSICAL") == 0)
            Moves[i].category = PHYSICAL;
        else
            Moves[i].category = SPECIAL;
    }

    fclose(fp);
}

 
    void initializePokemons(Pokemon Pokemons[1015], Type Types[18], Move Moves[486])
{
    FILE *fp = fopen(POKEMON_FILE, "r");
    if (fp == NULL) {
        printf("pokemon.txt didn't open!\n");
        exit(1);
    }

    char line[256];

    for (int i = 0; i < 1015; i++) {

        if (fgets(line, sizeof(line), fp) == NULL) {
            printf("pokemon.txt satir sayisi eksik.\n");
            exit(1);
        }

        char pname[30], t1[20], t2[20];
        int maxHP, attack, defense, spAtk, spDef, speed;

        sscanf(line, "%29s %19s %19s %d %d %d %d %d %d",
               pname, t1, t2, &maxHP, &attack, &defense, &spAtk, &spDef, &speed);

        strcpy(Pokemons[i].name, pname);

        Pokemons[i].maxHP = maxHP;
        Pokemons[i].currentHP = maxHP;
        Pokemons[i].attack = attack;
        Pokemons[i].defense = defense;
        Pokemons[i].spAtk = spAtk;
        Pokemons[i].spDef = spDef;
        Pokemons[i].speed = speed;

        int index1 = typeIndex(Types, t1);
        if (index1 == -1) {
            printf("pokemon.txt unknown type1: %s\n", t1);
            exit(1);
        }
        Pokemons[i].types[0] = Types[index1];

        if (strcmp(t2, "-") == 0) {

            strcpy(Pokemons[i].types[1].name, "None");

            for (int j = 0; j < 19; j++) {
                strcpy(Pokemons[i].types[1].effects[j].atkName, "");
                strcpy(Pokemons[i].types[1].effects[j].defName, "");
                Pokemons[i].types[1].effects[j].multiplier = 1.0f;
            }

        } else {

            int index2 = typeIndex(Types, t2);
            if (index2 == -1) {
                printf("pokemon.txt bilinmeyen type2: %s\n", t2);
                exit(1);
            }
            Pokemons[i].types[1] = Types[index2];
        }

        int usedMoveIndex[4];
        for (int k = 0; k < 4; k++) usedMoveIndex[k] = -1;

        for (int k = 0; k < 4; k++) {
            int r;
            do {
                r = rand() % 486;
            } while (isDuplicate(usedMoveIndex, k, r));

            usedMoveIndex[k] = r;
            Pokemons[i].moves[k] = Moves[r];
        }
    }

    fclose(fp);
}
