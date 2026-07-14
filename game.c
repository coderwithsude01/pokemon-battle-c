#include <stdio.h>
#include <string.h>
#include "include/prototypes.h"

#define TYPES_FILE   "data/types.txt"
#define MOVES_FILE   "data/moves.txt"
#define POKEMON_FILE "data/pokemon.txt"





static int hasAlive(Player *p)
{
    for (int i = 0; i < 6; i++) {
        if ((*p).Pokemons[i].currentHP > 0) return 1;
    }
    return 0;
}

static int nextAliveIndex(const Player *p)
{
    for (int i = 0; i < 6; i++) {
        if (p->Pokemons[i].currentHP > 0) return i + 1; 
    }
    return 1; 
}


void game(Player *p1, Player *p2, Type Types[18])
{
    while (hasAlive(p1) && hasAlive(p2)) {
        playRound(p1, p2, Types);
    }

    if (hasAlive(p1)) {
        printf("\n%s wins!\n", (*p1).name);
    } else {
        printf("\n%s wins!\n", (*p2).name);
    }
}



void playRound(Player *p1, Player *p2, Type Types[18])
{
  

   int p1Move = 1, p2Move = 1;
    int index;

int p1Action, p2Action;
int ch;

do {
    printf("\n%s:\n1 - Attack\n2 - Change Pokemon\nSelect: ", p1->name);

    if (scanf("%d", &p1Action) != 1) {
        printf("Error: Please enter a NUMBER (1 or 2)!\n");
        while ((ch = getchar()) != '\n' && ch != EOF); 
        p1Action = 0;
        continue;
    }

    if (p1Action != 1 && p1Action != 2)
        printf("Error: Only 1 or 2!\n");

} while (p1Action != 1 && p1Action != 2);


do {
    printf("\n%s:\n1 - Attack\n2 - Change Pokemon\nSelect: ", p2->name);

    if (scanf("%d", &p2Action) != 1) {
        printf("Error: Please enter a NUMBER (1 or 2)!\n");
        while ((ch = getchar()) != '\n' && ch != EOF); 
        p2Action = 0;
        continue;
    }

    if (p2Action != 1 && p2Action != 2)
        printf("Error: Only 1 or 2!\n");

} while (p2Action != 1 && p2Action != 2);

    if (p1Action == 1) {
        Pokemon *p = &((*p1).Pokemons[(*p1).currentIndex - 1]);

        printf("\n1 - %s  2 - %s\n3 - %s  4 - %s\n",
               (*p).moves[0].name, (*p).moves[1].name,
               (*p).moves[2].name, (*p).moves[3].name);

        printf("Please select a move: ");
        scanf("%d", &p1Move);
    } else {
        printf("\nAvailable Pokemons (HP > 0):\n");
        for (int i = 0; i < 6; i++) {
            if ((*p1).Pokemons[i].currentHP > 0) {
                printf("%d - %s  ", i + 1, (*p1).Pokemons[i].name);
            }
        }

        printf("\nPlease select a Pokemon to switch: ");
        scanf("%d", &index);
        (*p1).currentIndex = index;
    }

    if (p2Action == 1) {
        Pokemon *p = &((*p2).Pokemons[(*p2).currentIndex - 1]);

        printf("\n1 - %s  2 - %s\n3 - %s  4 - %s\n",
               (*p).moves[0].name, (*p).moves[1].name,
               (*p).moves[2].name, (*p).moves[3].name);

        printf("Please select a move: ");
        scanf("%d", &p2Move);
    } else {
        printf("\nAvailable Pokemons (HP > 0):\n");
        for (int i = 0; i < 6; i++) {
            if ((*p2).Pokemons[i].currentHP > 0) {
                printf("%d - %s  ", i + 1, (*p2).Pokemons[i].name);
            }
        }

        printf("\nPlease select a Pokemon to switch: ");
        scanf("%d", &index);
        (*p2).currentIndex = index;
    }

    applyDamage(p1, p2, Types, p1Action, p1Move, p2Action, p2Move);
}





void applyDamage(Player *p1, Player *p2, Type Types[18],
                 int p1Action, int p1MoveChoice,
                 int p2Action, int p2MoveChoice)
{
    Pokemon *p1Poke = &((*p1).Pokemons[(*p1).currentIndex - 1]);
    Pokemon *p2Poke = &((*p2).Pokemons[(*p2).currentIndex - 1]);

    int p1CanAttack = (p1Action == 1) && ((*p1Poke).currentHP > 0);
    int p2CanAttack = (p2Action == 1) && ((*p2Poke).currentHP > 0);

    int p1First = ((*p1Poke).speed >= (*p2Poke).speed);

    for (int t = 0; t < 2; t++) {

        Pokemon *atkPoke;
        Pokemon *defPoke;
        int canAttack;
        int moveChoice;

        if ((t == 0 && p1First) || (t == 1 && !p1First)) {
            atkPoke   = p1Poke;
            defPoke   = p2Poke;
            canAttack = p1CanAttack;
            moveChoice = p1MoveChoice;
        } else {
            atkPoke   = p2Poke;
            defPoke   = p1Poke;
            canAttack = p2CanAttack;
            moveChoice = p2MoveChoice;
        }

        if (!canAttack) continue;
        if ((*atkPoke).currentHP <= 0) continue;
        if ((*defPoke).currentHP <= 0) continue;

        if (moveChoice < 1) moveChoice = 1;
        if (moveChoice > 4) moveChoice = 4;
        Move *atkMove = &((*atkPoke).moves[moveChoice - 1]);

        float atkStat, defStat;
        if ((*atkMove).category == PHYSICAL) {
            atkStat = (float)(*atkPoke).attack;
            defStat = (float)(*defPoke).defense;
        } else {
            atkStat = (float)(*atkPoke).spAtk;
            defStat = (float)(*defPoke).spDef;
        }
        if (defStat <= 0.0f) defStat = 1.0f;

        float type1 = 1.0f;
        float type2 = 1.0f;

        int moveTypeIndex = -1;
        for (int i = 0; i < 18; i++) {
            if (strcmp(Types[i].name, (*atkMove).type.name) == 0) {
                moveTypeIndex = i;
                break;
            }
        }

        if (moveTypeIndex != -1) {

            for (int j = 0; j < 19; j++) {
                if (strcmp(Types[moveTypeIndex].effects[j].defName, (*defPoke).types[0].name) == 0) {
                    type1 = Types[moveTypeIndex].effects[j].multiplier;
                    break;
                }
            }

            if (strcmp((*defPoke).types[1].name, "None") != 0) {
                for (int j = 0; j < 19; j++) {
                    if (strcmp(Types[moveTypeIndex].effects[j].defName, (*defPoke).types[1].name) == 0) {
                        type2 = Types[moveTypeIndex].effects[j].multiplier;
                        break;
                    }
                }
            }
        }

        
        float stab = 1.0f;
        if (strcmp((*atkMove).type.name, (*atkPoke).types[0].name) == 0 ||
            strcmp((*atkMove).type.name, (*atkPoke).types[1].name) == 0) {
            stab = 1.5f;
        }

        float dmgF = (float)(*atkMove).power * (atkStat / defStat) * type1 * type2 * stab;
        int dmg = (int)(dmgF + 0.5f);
        if (dmg < 1) dmg = 1;

        (*defPoke).currentHP -= dmg;

printf("\n%s used %s! Damage: %d\n", (*atkPoke).name, (*atkMove).name, dmg);

if ((*defPoke).currentHP <= 0) {                 
    (*defPoke).currentHP = 0;
    printf("%s HP: %d\n", (*defPoke).name, (*defPoke).currentHP);
    printf("%s fainted!\n", (*defPoke).name);

    if (defPoke == p1Poke) {
        p1->currentIndex = nextAliveIndex(p1);
    } else {
        p2->currentIndex = nextAliveIndex(p2);
    }

    break; 
} else {
    printf("%s HP: %d\n", (*defPoke).name, (*defPoke).currentHP);
}
    }
}
