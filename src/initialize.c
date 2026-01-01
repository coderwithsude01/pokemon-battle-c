#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/prototypes.h"

#define TYPE_COUNT 18
#define MOVE_COUNT 486
#define POKEMON_COUNT 1015

#define TYPES_FILE   "types.txt"
#define MOVES_FILE   "moves.txt"
#define POKEMON_FILE "pokemon.txt"

/* ------- helpers (kısa) ------- */
static int usedInt(const int arr[], int n, int x){
    for(int i=0;i<n;i++) if(arr[i]==x) return 1;
    return 0;
}

static int findTypeIndex(Type Types[TYPE_COUNT], const char *name){
    for(int i=0;i<TYPE_COUNT;i++){
        if(strcmp(Types[i].name, name)==0) return i;
    }
    return -1;
}

/* ------- 2.2.2 initialize ------- */
void initialize(Type Types[18], Move Moves[486], Pokemon Pokemons[1015],
                Player *Player1, Player *Player2)
{
    srand((unsigned)time(NULL));

    initializeTypes(Types);
    initializeMoves(Moves, Types);
    initializePokemons(Pokemons, Types, Moves);

    strcpy(Player1->name, "Player1");
    strcpy(Player2->name, "Player2");

    int pick1[6], pick2[6];
    for(int i=0;i<6;i++){ pick1[i] = -1; pick2[i] = -1; }

    /* Player1: 6 unique pokemon */
    for(int i=0;i<6;i++){
        int idx;
        do { idx = rand() % POKEMON_COUNT; } while(usedInt(pick1, i, idx));
        pick1[i] = idx;
        Player1->Pokemons[i] = Pokemons[idx];
    }

    /* Player2: 6 unique pokemon (kendi içinde) */
    for(int i=0;i<6;i++){
        int idx;
        do { idx = rand() % POKEMON_COUNT; } while(usedInt(pick2, i, idx));
        pick2[i] = idx;
        Player2->Pokemons[i] = Pokemons[idx];
    }

    Player1->currentIndex = 1; /* PDF böyle istiyor */
    Player2->currentIndex = 1;
}

/* ------- 2.2.3 initializeTypes ------- */
/* types.txt: 19 satırlık blok: 1 attacker + 18 (defender multiplier) */
void initializeTypes(Type Types[18])
{
    FILE *fp = fopen(TYPES_FILE, "r");
    if(!fp){ printf("ERROR: %s\n", TYPES_FILE); exit(1); }

    char line[256];

    for(int i=0;i<TYPE_COUNT;i++){
        /* attacker adı */
        if(!fgets(line, sizeof(line), fp)){ printf("EOF types\n"); exit(1); }
        while(line[0]=='\n' || line[0]=='\r'){
            if(!fgets(line, sizeof(line), fp)){ printf("EOF types\n"); exit(1); }
        }

        char atk[20];
        sscanf(line, "%19s", atk);
        strcpy(Types[i].name, atk);

        /* 18 defender satırı */
        for(int j=0;j<TYPE_COUNT;j++){
            if(!fgets(line, sizeof(line), fp)){ printf("EOF types\n"); exit(1); }

            char def[20];
            float mult;
            sscanf(line, "%19s %f", def, &mult);

            strcpy(Types[i].effects[j].atkName, atk);
            strcpy(Types[i].effects[j].defName, def);
            Types[i].effects[j].multiplier = mult;
        }
    }

    fclose(fp);
}

/* ------- 2.2.4 initializeMoves ------- */
/* moves.txt: name type category power */
void initializeMoves(Move Moves[486], Type Types[18])
{
    FILE *fp = fopen(MOVES_FILE, "r");
    if(!fp){ printf("ERROR: %s\n", MOVES_FILE); exit(1); }

    char line[256];

    for(int i=0;i<MOVE_COUNT;i++){
        if(!fgets(line, sizeof(line), fp)){ printf("EOF moves\n"); exit(1); }

        char mname[30], tname[20], cname[20];
        int power;

        sscanf(line, "%29s %19s %19s %d", mname, tname, cname, &power);

        strcpy(Moves[i].name, mname);
        Moves[i].power = power;

        int tIdx = findTypeIndex(Types, tname);
        if(tIdx == -1){ printf("Move type not found: %s\n", tname); exit(1); }
        Moves[i].type = Types[tIdx];

        if(strcmp(cname,"Physical")==0 || strcmp(cname,"PHYSICAL")==0) Moves[i].category = PHYSICAL;
        else Moves[i].category = SPECIAL; /* Special/SPECIAL varsayımı */
    }

    fclose(fp);
}

/* ------- 2.2.5 initializePokemons ------- */
/* pokemon.txt: name type1 type2 maxHP attack defense spAtk spDef speed */
/* type2 "-" ise "None" */
/* moves: 4 unique rand (0..485) */
void initializePokemons(Pokemon Pokemons[1015], Type Types[18], Move Moves[486])
{
    FILE *fp = fopen(POKEMON_FILE, "r");
    if(!fp){ printf("ERROR: %s\n", POKEMON_FILE); exit(1); }

    char line[256];

    for(int i=0;i<POKEMON_COUNT;i++){
        if(!fgets(line, sizeof(line), fp)){ printf("EOF pokemon\n"); exit(1); }

        char pname[30], t1[20], t2[20];
        int maxHP, atk, def, spAtk, spDef, speed;

        sscanf(line, "%29s %19s %19s %d %d %d %d %d %d",
               pname, t1, t2, &maxHP, &atk, &def, &spAtk, &spDef, &speed);

        strcpy(Pokemons[i].name, pname);
        Pokemons[i].maxHP = maxHP;
        Pokemons[i].currentHP = maxHP;
        Pokemons[i].attack = atk;
        Pokemons[i].defense = def;
        Pokemons[i].spAtk = spAtk;
        Pokemons[i].spDef = spDef;
        Pokemons[i].speed = speed;

        int idx1 = findTypeIndex(Types, t1);
        if(idx1 == -1){ printf("Pokemon type1 not found: %s\n", t1); exit(1); }
        Pokemons[i].types[0] = Types[idx1];

        if(strcmp(t2, "-") == 0){
            strcpy(Pokemons[i].types[1].name, "None");
        } else {
            int idx2 = findTypeIndex(Types, t2);
            if(idx2 == -1){ printf("Pokemon type2 not found: %s\n", t2); exit(1); }
            Pokemons[i].types[1] = Types[idx2];
        }

        int pick[4];
        for(int k=0;k<4;k++) pick[k] = -1;

        for(int k=0;k<4;k++){
            int midx;
            do { midx = rand() % MOVE_COUNT; } while(usedInt(pick, k, midx));
            pick[k] = midx;
            Pokemons[i].moves[k] = Moves[midx];
        }
    }

    fclose(fp);
}