#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/prototypes.h"

#define TYPES_FILE   "data/types.txt"
#define MOVES_FILE   "data/moves.txt"
#define POKEMON_FILE "data/pokemon.txt"


/* Amaç: rand() ile seçerken aynı index tekrar seçilmesin diye kontrol */
static int isDuplicate(const int arr[], int size, int val)
{
    /* arr: daha önce seçilen değerler
       size: kaç tanesi dolu
       val: yeni seçilen değer */
    for (int i = 0; i < size; i++) {
        if (arr[i] == val) return 1; /* tekrar varsa */
    }
    return 0; /* tekrar yok */
}

/* Amaç: type ismini (ör: "Fire") Types[] içinde arayıp index döndürmek */
static int typeIndex(Type Types[18], const char *name)
{
    for (int i = 0; i < 18; i++) {
        if (strcmp(Types[i].name, name) == 0) return i;
    }
    return -1; /* bulunamadı */
}

/* Amaç: ikinci tipi olmayan pokemonlar için "None" type oluşturmak
   PDF: None type'ın bütün multiplierları 1 olmalı */
   
// static Type buildNoneType(Type Types[18])
// {
//     Type noneType;
//     strcpy(noneType.name, "None");

//     /* None -> her defender için multiplier = 1 */
//     for (int j = 0; j < 18; j++) {
//         strcpy(noneType.effects[j].atkName, "None");      /* saldıran type adı */
//         strcpy(noneType.effects[j].defName, Types[j].name); /* savunan type adı */
//         noneType.effects[j].multiplier = 1.0f;
//     }

//     return noneType;
// }



/* ------------------ 2.2.2 INITIALIZE ------------------ */
/* PDF: önce Types, sonra Moves, sonra Pokemons doldurulacak.
   Sonra Player1 ve Player2 oluşturulup 6’şar pokemon rastgele atanacak. */
void initialize(Type Types[18], Move Moves[486], Pokemon Pokemons[1015],
                Player *p1, Player *p2)
{
    /* rand() her çalıştırmada farklı sonuç versin */
    srand((unsigned)time(NULL));

    /* PDF sırası */
    initializeTypes(Types);
    initializeMoves(Moves, Types);
    initializePokemons(Pokemons, Types, Moves);

    /* oyuncu isimleri */
    strcpy((*p1).name, "Player1");
    strcpy((*p2).name, "Player2");

    /* seçilen pokemon indexlerini tutacağız (unique kontrol için) */
    int pick1[6];
    int pick2[6];

    for (int i = 0; i < 6; i++) {
        pick1[i] = -1;
        pick2[i] = -1;
    }

    /* Player1: 0..1014 arası 6 farklı pokemon seç */
    for (int i = 0; i < 6; i++) {
        int r;
        do {
            r = rand() % 1015;
        } while (isDuplicate(pick1, i, r));

        pick1[i] = r;
        (*p1).Pokemons[i] = Pokemons[r];
    }

    /* Player2: kendi içinde unique + Player1 ile de aynı olmasın */
    for (int i = 0; i < 6; i++) {
        int r;
        do {
            r = rand() % 1015;
        } while (isDuplicate(pick2, i, r) || isDuplicate(pick1, 6, r));

        pick2[i] = r;
        (*p2).Pokemons[i] = Pokemons[r];
    }

    /* PDF: currentIndex başlangıç 1 */
    (*p1).currentIndex = 1;
    (*p2).currentIndex = 1;
}

/* ------------------ 2.2.3 INITIALIZETYPES ------------------ */
/* PDF: types.txt 19 satırlık bloklar:
   1 satır attacker type adı
   18 satır defender + multiplier */
void initializeTypes(Type Types[18])
{
    FILE *fp = fopen(TYPES_FILE, "r");

    if (fp == NULL) {
        printf("types.txt didn't open!\n");
        exit(1);
    }

    char line[256];

    for (int i = 0; i < 18; i++) {

        /* attacker satırı oku (ör: Fire) */
        if (fgets(line, sizeof(line), fp) == NULL) {
            printf("types.txt satir sayisi eksik.\n");
            exit(1);
        }

        /* bazen dosyada boş satır olursa geç */
        while (line[0] == '\n' || line[0] == '\r') {
            if (fgets(line, sizeof(line), fp) == NULL) {
                printf("types.txt satir sayisi eksik.\n");
                exit(1);
            }
        }

        /* attacker type adını al */
        char atk[20];
        sscanf(line, "%19s", atk);

        /* Types[i] bu attacker type */
        strcpy(Types[i].name, atk);

        /* attacker için 18 defender satırını doldur */
        for (int j = 0; j < 18; j++) {

            if (fgets(line, sizeof(line), fp) == NULL) {
                printf("types.txt defender satiri eksik.\n");
                exit(1);
            }

            /* satır örn: Grass 2.0 */
            char def[20];
            float mult;
            sscanf(line, "%19s %f", def, &mult);

            /* Fire -> Grass = 2.0 gibi effect kaydı */
            strcpy(Types[i].effects[j].atkName, atk);
            strcpy(Types[i].effects[j].defName, def);
            Types[i].effects[j].multiplier = mult;
        }

        /* ---------- PDF: NONE TYPE (19. EFFECT) ---------- */
        strcpy(Types[i].effects[18].atkName, atk);
        strcpy(Types[i].effects[18].defName, "None");
        Types[i].effects[18].multiplier = 1.0f;
    }
    
    fclose(fp);
}



/* ------------------ 2.2.4 INITIALIZEMOVES ------------------ */
/* PDF: moves.txt her satır:
   name type category power */
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




 















// /* ------- helpers (kısa) ------- */
// static int usedInt(const int arr[], int n, int x){
//     for(int i=0;i<n;i++) if(arr[i]==x) return 1;
//     return 0;
// }

// static int findTypeIndex(Type Types[TYPE_COUNT], const char *name){
//     for(int i=0;i<TYPE_COUNT;i++){
//         if(strcmp(Types[i].name, name)==0) return i;
//     }
//     return -1;
// }

// /* ------- 2.2.2 initialize ------- */
// void initialize(Type Types[18], Move Moves[486], Pokemon Pokemons[1015],
//                 Player *Player1, Player *Player2)
// {
//     srand((unsigned)time(NULL));

//     initializeTypes(Types);
//     initializeMoves(Moves, Types);
//     initializePokemons(Pokemons, Types, Moves);

//     strcpy(Player1->name, "Player1");
//     strcpy(Player2->name, "Player2");

//     int pick1[6], pick2[6];
//     for(int i=0;i<6;i++){ pick1[i] = -1; pick2[i] = -1; }

//     /* Player1: 6 unique pokemon */
//     for(int i=0;i<6;i++){
//         int idx;
//         do { idx = rand() % POKEMON_COUNT; } while(usedInt(pick1, i, idx));
//         pick1[i] = idx;
//         Player1->Pokemons[i] = Pokemons[idx];
//     }

//     /* Player2: 6 unique pokemon (kendi içinde) */
//     for(int i=0;i<6;i++){
//         int idx;
//         do { idx = rand() % POKEMON_COUNT; } while(usedInt(pick2, i, idx));
//         pick2[i] = idx;
//         Player2->Pokemons[i] = Pokemons[idx];
//     }

//     Player1->currentIndex = 1; /* PDF böyle istiyor */
//     Player2->currentIndex = 1;
// }

// /* ------- 2.2.3 initializeTypes ------- */
// /* types.txt: 19 satırlık blok: 1 attacker + 18 (defender multiplier) */
// void initializeTypes(Type Types[18])
// {
//     FILE *fp = fopen(TYPES_FILE, "r");
//     if(!fp){ printf("ERROR: %s\n", TYPES_FILE); exit(1); }

//     char line[256];

//     for(int i=0;i<TYPE_COUNT;i++){
//         /* attacker adı */
//         if(!fgets(line, sizeof(line), fp)){ printf("EOF types\n"); exit(1); }
//         while(line[0]=='\n' || line[0]=='\r'){
//             if(!fgets(line, sizeof(line), fp)){ printf("EOF types\n"); exit(1); }
//         }

//         char atk[20];
//         sscanf(line, "%19s", atk);
//         strcpy(Types[i].name, atk);

//         /* 18 defender satırı */
//         for(int j=0;j<TYPE_COUNT;j++){
//             if(!fgets(line, sizeof(line), fp)){ printf("EOF types\n"); exit(1); }

//             char def[20];
//             float mult;
//             sscanf(line, "%19s %f", def, &mult);

//             strcpy(Types[i].effects[j].atkName, atk);
//             strcpy(Types[i].effects[j].defName, def);
//             Types[i].effects[j].multiplier = mult;
//         }
//     }

//     fclose(fp);
// }

// /* ------- 2.2.4 initializeMoves ------- */
// /* moves.txt: name type category power */
// void initializeMoves(Move Moves[486], Type Types[18])
// {
//     FILE *fp = fopen(MOVES_FILE, "r");
//     if(!fp){ printf("ERROR: %s\n", MOVES_FILE); exit(1); }

//     char line[256];

//     for(int i=0;i<MOVE_COUNT;i++){
//         if(!fgets(line, sizeof(line), fp)){ printf("EOF moves\n"); exit(1); }

//         char mname[30], tname[20], cname[20];
//         int power;

//         sscanf(line, "%29s %19s %19s %d", mname, tname, cname, &power);

//         strcpy(Moves[i].name, mname);
//         Moves[i].power = power;

//         int tIdx = findTypeIndex(Types, tname);
//         if(tIdx == -1){ printf("Move type not found: %s\n", tname); exit(1); }
//         Moves[i].type = Types[tIdx];

//         if(strcmp(cname,"Physical")==0 || strcmp(cname,"PHYSICAL")==0) Moves[i].category = PHYSICAL;
//         else Moves[i].category = SPECIAL; /* Special/SPECIAL varsayımı */
//     }

//     fclose(fp);
// }

// /* ------- 2.2.5 initializePokemons ------- */
// /* pokemon.txt: name type1 type2 maxHP attack defense spAtk spDef speed */
// /* type2 "-" ise "None" */
// /* moves: 4 unique rand (0..485) */
// void initializePokemons(Pokemon Pokemons[1015], Type Types[18], Move Moves[486])
// {
//     FILE *fp = fopen(POKEMON_FILE, "r");
//     if(!fp){ printf("ERROR: %s\n", POKEMON_FILE); exit(1); }

//     char line[256];

//     for(int i=0;i<POKEMON_COUNT;i++){
//         if(!fgets(line, sizeof(line), fp)){ printf("EOF pokemon\n"); exit(1); }

//         char pname[30], t1[20], t2[20];
//         int maxHP, atk, def, spAtk, spDef, speed;

//         sscanf(line, "%29s %19s %19s %d %d %d %d %d %d",
//                pname, t1, t2, &maxHP, &atk, &def, &spAtk, &spDef, &speed);

//         strcpy(Pokemons[i].name, pname);
//         Pokemons[i].maxHP = maxHP;
//         Pokemons[i].currentHP = maxHP;
//         Pokemons[i].attack = atk;
//         Pokemons[i].defense = def;
//         Pokemons[i].spAtk = spAtk;
//         Pokemons[i].spDef = spDef;
//         Pokemons[i].speed = speed;

//         int idx1 = findTypeIndex(Types, t1);
//         if(idx1 == -1){ printf("Pokemon type1 not found: %s\n", t1); exit(1); }
//         Pokemons[i].types[0] = Types[idx1];

//         if(strcmp(t2, "-") == 0){
//             strcpy(Pokemons[i].types[1].name, "None");
//         } else {
//             int idx2 = findTypeIndex(Types, t2);
//             if(idx2 == -1){ printf("Pokemon type2 not found: %s\n", t2); exit(1); }
//             Pokemons[i].types[1] = Types[idx2];
//         }

//         int pick[4];
//         for(int k=0;k<4;k++) pick[k] = -1;

//         for(int k=0;k<4;k++){
//             int midx;
//             do { midx = rand() % MOVE_COUNT; } while(usedInt(pick, k, midx));
//             pick[k] = midx;
//             Pokemons[i].moves[k] = Moves[midx];
//         }
//     }

//     fclose(fp);
// }