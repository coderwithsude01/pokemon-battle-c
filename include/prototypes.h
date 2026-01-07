#ifndef PROTOTYPES_H
#define PROTOTYPES_H

#include "structs.h"

void initialize(Type Types[18], Move Moves[486], Pokemon Pokemons[1015],
                Player *p1, Player *p2);

void initializeTypes(Type Types[18]);
void initializeMoves(Move Moves[486], Type Types[18]);
void initializePokemons(Pokemon Pokemons[1015], Type Types[18], Move Moves[486]);

void game(Player *p1, Player *p2, Type Types[18]);
void playRound(Player *p1, Player *p2, Type Types[18]);

void applyDamage(Player *p1, Player *p2, Type Types[18],
                 int p1Action, int p1MoveChoice,
                 int p2Action, int p2MoveChoice);


#endif