
 #include "prototypes.h"                                                                               │
│  2                                                                                                       │
│  3 void createPokemon(Pokemon* p, const char* name, int hp, int attack, int defense) {                   │
│  4     // p->name yerine (*p).name kullanılıyor                                                          │
│  5     strcpy((*p).name, name);                                                                          │
│  6     (*p).hp = hp;                                                                                     │
│  7     (*p).attack = attack;                                                                             │
│  8     (*p).defense = defense;                                                                           │
│  9 }                                                                                                     │
│ 10                                                                                                       │
│ 11 void displayPokemon(const Pokemon* p) {                                                               │
│ 12     printf("Pokemon: %s, HP: %d, Atk: %d, Def: %d\n", (*p).name, (*p).hp, (*p).attack, (*p).defense); │
│ 13 }                                              