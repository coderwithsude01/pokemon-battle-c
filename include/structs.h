#ifndef STRUCTS_H
#define STRUCTS_H

typedef enum {
    PHYSICAL,
    SPECIAL
} Category;

typedef struct {
    char atkName[20];
    char defName[20];
    float multiplier;
} TypeEffect;

typedef struct {
    char name[20];
    TypeEffect effects[18];
}Type;

typedef struct{
    char name[30];
    Type type;
     Category category;
     int power;
}Move;

typedef struct{
    char name[30];
    Type types[2];
    int maxHP;
    int currentHP;
    int attack;
    int defense;
    int spAtk;
    int spDef;
    int speed;
    Move moves[4];
}Pokemon;

typedef struct{
    char name[50];
    Pokemon Pokemons[6];
    int currentIndex;
}Player;

#endif

