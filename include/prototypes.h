#ifndef PROTOTYPES_H
#define PROTOTYPES_H
#include "structs.h"
#include <stdbool.h>
#include "constants.h"



/* INITIALIZATION */
void initialize(Type Types[18], Move Moves[486], Pokemon Pokemons[1015],
                Player *Player1, Player *Player2);

void initializeTypes(Type Types[18]);
void initializeMoves(Move Moves[486], Type Types[18]);
void initializePokemons(Pokemon Pokemons[1015], Type Types[18], Move Moves[486]);

/* GAME */
void game(Player *Player1, Player *Player2, Type Types[18]);
void round(Player *Player1, Player *Player2, Type Types[18]);
void applyDamage(Player *Player1, Player *Pl
    


                                         
     
    #endif 