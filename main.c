/***********************************
 * DSA Zadianie 1 - Spravca pamati
 * 
 * FIIT STU 2021
 * Pavol Hradsky
************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int* header;

int *char2int (char *ptr){                                  //pretypovanie char na int
    return ((int*)ptr);
}
char *int2char (int *ptr){                                  //pretypovanie int na char
    return ((char*)ptr);
}
unsigned short *char2short(char *ptr){                      //pretypovanie char na unsigned short
    return ((unsigned short*)ptr);
}

void *memory_alloc(unsigned int size){

//definovanie velkosti rezie
    int stuff = 2 * sizeof(int);
    int freeStuff = 2 * sizeof(unsigned short);
    int fullSize = (int)size + stuff;

//zapamatanie dolezitych miest v pamati
    char *tmp = int2char(header);
    char *firstOffset = tmp + 4;    //prve volne miesto (pointer)
    tmp += *char2int(tmp + stuff / 2);
    int freeSizeBefore = *(char2int(tmp));  //volne miesto kam alokujem
    unsigned short nextFreeOffset = *(char2short(tmp + stuff/2));   //dalsi volny blok
    char *nextFreeOffset_place = tmp + stuff/2;
    unsigned short lastFreeOffset = *(char2int(tmp + stuff/2 + freeStuff/2)); //predchadzajuci volny blok
    char *lastFreeOffset_place = tmp + stuff/2 + freeStuff/2;
    char *lastFreeBlock = int2char(header);
//ak chcem alokovat viac ako mam miesta - 1. podmienka
    int betweenIsFree = 0;  //pamata si, ci som preskocil nejaky mensi volny blok

    while(*char2int(tmp) < (int) size && *char2short(tmp + stuff / 2) != 0){    //prechadza volne bloky a hlada prvy, kam sa vmesti novy blok
        betweenIsFree = 1;
        size += 0;
        lastFreeBlock = tmp;
        tmp += *char2short(tmp + stuff / 2);
    }
    if(tmp + fullSize > int2char(header) + *header){    //ak som uz za pamatou do ktorej mam pristup, vrati NULL
        return NULL;
    }

    if(*char2int(tmp) < size + stuff + freeStuff){      //ak by volne miesto ktore sa vytvori po rozpoleni volneho bloku nestacilo na reziu volneho miesta, priradi sa k uz alokovanemu
        size = *char2int(tmp);
        //printf("SIZE: %d\n", size);
        fullSize = (int) size + stuff;
    }
    //aktualizovanie predoslich a dalsich volnych blokov podla sucastneho miesta tmp
    char *nextFreeBlock = tmp + *char2short(tmp + stuff / 2);
    freeSizeBefore = *(char2int(tmp));
    nextFreeOffset = *(char2short(tmp + stuff/2));
    nextFreeOffset_place = tmp + stuff/2;
    lastFreeOffset = *(char2int(tmp + stuff/2 + freeStuff/2));
    lastFreeOffset_place = tmp + stuff/2 + freeStuff/2;

    char *toReturn = tmp;   //pointer na vratenie
    unsigned short isLast = 1;
    if(*char2int(tmp) == size && *char2short(tmp + stuff / 2) == 0){ //pamata si ci je blok posledny
        isLast = 0;
    }
//alokovanie miesta
    *(char2int(tmp)) = (int)size * (-1);
    tmp += stuff/2;
    for(int i = 0; i < size; i++){
        *tmp = -2;
        tmp += 1;
    }
    *(char2int(tmp)) = (int)size * (-1);
    tmp += stuff/2;
    //printf("%d\n", *char2int(tmp));
//nastavienie volnych offsetov (aj aktualizovanie minuleho a buduceho)

    if(*char2int(tmp) >= 0){    //ak sucastne miesto tmp nie je alokovane
        if(!isLast){ //ak je to posledne volne miesto, ak od zaciatku enbol ziaden volny blok, lastFreeOffset sa nastavi na 0, inak na velkost celej pamate
            if(betweenIsFree){
                *char2short(lastFreeOffset_place) = 0;

            } else{
                *char2int(firstOffset) = *header;
            }
        } else{ //ak nebol posledny
            *(char2int(tmp)) = freeSizeBefore - fullSize;   //zapise sa novy volny blok
            *(char2int(tmp + freeSizeBefore - (fullSize - stuff/2))) = freeSizeBefore - fullSize;
            if(nextFreeOffset == 0){    //ak je posledny, toto som uz osetril hore
                *char2short(tmp + stuff/2) = 0;
            }else{  //aktualizuju sa ukazovatel na dalsi volny blok a ukazovatel na predosly volny blok (aj v dalsom a predoslom volnom bloku)
                *char2short(tmp + stuff/2) = nextFreeOffset - fullSize;
                *char2short(nextFreeOffset_place + nextFreeOffset + freeStuff/2) = (unsigned short)((nextFreeOffset_place + nextFreeOffset - stuff/2) - tmp);
            }
            if(lastFreeOffset_place - stuff/2 - freeStuff/2 - lastFreeOffset == int2char(header)){
                *char2short(tmp + stuff/2 + freeStuff/2) = (unsigned short)(tmp - int2char(header));
                *char2short(firstOffset) = (unsigned short)(tmp - int2char(header));
            }else{
                *char2short(tmp + stuff/2 + freeStuff/2) = (unsigned short)(lastFreeOffset + fullSize);
                *char2short(nextFreeOffset_place - lastFreeOffset) = (unsigned short)(tmp - (nextFreeOffset_place - lastFreeOffset - stuff/2));
            }
        }
    }else { //ak sucastne tmp uz je alokovane (vyuzilo sa cele volne miesto), nastavia sa ukazovatele spravnym sposobom
        *char2short(lastFreeBlock + stuff / 2) = (unsigned short) (nextFreeBlock - lastFreeBlock);
        *char2short(nextFreeBlock + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeBlock - lastFreeBlock);
    }


    return toReturn; //vrati sa ukazovatel na alokovany blok

}

int memory_free(void *valid_ptr){

//definicia rezie
    int stuff = 2 * sizeof(int);
    int freeStuff = 2 * sizeof(unsigned short);


//zapamatanie dolezitych miest v pamati
    char *tmp = (char*)valid_ptr;
    int size = *char2int(tmp) * (-1);   //velkost
    int fullSize = *char2int(tmp) * (-1) + stuff;   //velkost + stuff

//Uvolni pamat, zaporne hodnotu sa zmenia na kladne a vyplni sa nulami
    *char2int(tmp) *= (-1);
    tmp += 4;
    for(int i = 0; i < size; i++){
        *tmp = 0;
        tmp += 1;
    }
    *char2int(tmp) *= (-1);
    tmp -= (fullSize - stuff/2);
//Pospaja volne bloky a aktualizuje ukazovatele
    if(*char2int(int2char(header) + 4) == *header){     //cela pamat je plna
        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - int2char(header)); //offset na predosle volne miesto sa nastavi na header
        *char2short(int2char(header) + 4) = (unsigned short) (tmp - int2char(header)); //ukazovatel na prve volne miesto sa nastavi na tmp
    }
    else if(tmp == int2char(header) + stuff && *char2int(tmp + fullSize) < 0) {    //prvy pointer - za nie je volne
        char *lastFreeOffset = (int2char(header) + stuff / 2);  //lastFreeOffset sa nastavi na zaciatok a nextFreeOffset na najblizsi volny blok od zaciatku
        char *nextFreeOffset = (int2char(header) + *char2short(lastFreeOffset));
        *char2short(tmp + stuff / 2) = (unsigned short) (*char2short(lastFreeOffset) - 8);  //aktualizuje sa ukazovatel na predosli a dalsi volny blok
        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - int2char(header));
        *char2short(lastFreeOffset) = (unsigned short) (tmp - int2char(header));    //aktualizuje sa ukazovatel predosleho a dalsieho volneho bloku na tmp
        *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);

    } else if(tmp == int2char(header) + stuff && *char2int(tmp + fullSize) > 0){    //prvy pointer - za je volne
        char *lastFreeOffset = (int2char(header) + stuff / 2);  //lastFreeOffset sa nastavi na zaciatok a nextFreeOffset na najblizsi volny blok od zaciatku
        char *nextFreeOffset = (int2char(header) + *lastFreeOffset);
        int isLast = *char2short(nextFreeOffset + stuff / 2);   //pamata si ci je volny blok posledny
        nextFreeOffset += *char2short(nextFreeOffset + stuff / 2);  //posunie sa na dalsi volny blok
        *char2short(tmp + stuff / 2) = (unsigned short) (*lastFreeOffset - 8);  //aktualizuje sa ukazovatel na predosli a dalsi volny blok
        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - int2char(header));
        *char2short(lastFreeOffset) = (unsigned short) (tmp - int2char(header));

        *char2int(tmp) += *char2int(tmp + fullSize) + stuff;    //uvolneny blok sa spoji s dalsim volnym blokom
        int newSize = *char2int(tmp);
        tmp += stuff;
        for(int i = 0; i < (newSize - stuff / 2); i++){
            *tmp = 0;
            tmp += 1;
        }
        *char2int(tmp) = newSize;
        tmp -= (newSize + stuff / 2);

        if(isLast != 0){    //nastavia sa ukazovatele (offsety) na dalsi volny blok
            *char2short(tmp + stuff / 2) = (unsigned short) (nextFreeOffset - tmp);
            *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);
        } else{
            *char2short(tmp + stuff / 2) = (unsigned short) 0;
        }


    } else if(*char2int(tmp - stuff/2) < 0 && *char2int(tmp + fullSize) < 0){       //pred za nie je volne
        char *firstFree = tmp;
        firstFree += *char2int(firstFree) + stuff;
        while(*char2int(firstFree) < 0 && (firstFree + stuff) < (int2char(header) + *header)){  //najde sa najblizsi volny blok
            firstFree += (*char2int(firstFree) * (-1) + stuff);
        }
        char *lastFreeOffset = (firstFree - *(firstFree + stuff / 2 + freeStuff / 2));  //nastavi sa last a firstFreeOffset
        char *nextFreeOffset = firstFree;
        *char2short(lastFreeOffset + stuff/2) = (unsigned short) (tmp - lastFreeOffset);    //aktualizuju sa offsety

        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - lastFreeOffset);

        *char2short(tmp + stuff / 2) = (unsigned short) (nextFreeOffset - tmp);

        *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);


    } else if(*char2int(tmp - stuff/2) > 0 && *char2int(tmp + fullSize) < 0){       //pred je volne
        char *lastFreeOffset = tmp - (*char2int(tmp - stuff / 2) + stuff);  //nasatvi sa lastfreeoffset na blok pred tmp, nextfreeoffset ako ukazovatel na najblizsi volny blok z lastfree
                                                                                    //a lastfree sa posunie na dalsi vollny blok pred nim
        char *nextFreeOffset = lastFreeOffset + *char2short(lastFreeOffset + stuff / 2);
        char *toBind = lastFreeOffset;      //toBind je blok ktory budem spajat s uvolnenym blokom
        lastFreeOffset -= *char2short(lastFreeOffset + stuff / 2 + freeStuff / 2);

        int newSize = *char2int(toBind) + stuff + *char2int(tmp);   //uvolneny blok sa spoji s predchadzajucim blokom
        tmp = toBind;
        *char2int(tmp) = newSize;
        for(int i = 0; i < newSize; i++){
            *(tmp + stuff / 2 + i) = 0;
        }
        *char2int(tmp + stuff / 2 + newSize) = newSize;

        *char2short(lastFreeOffset + stuff / 2) = (unsigned short) (tmp - lastFreeOffset);  //aktualizuju sa offsety
        *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);
        *char2short(tmp + stuff / 2) = (unsigned short) (nextFreeOffset - tmp);
        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - lastFreeOffset);


    } else if(*char2int(tmp - stuff/2) < 0 && *char2int(tmp + fullSize) >= 0){       //za je volne
        char *nextFreeOffset = tmp + *char2int(tmp) + stuff;    //nastavi sa nextFreeOffset na blok za tmp
        char *lastFreeOffset = nextFreeOffset - *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2);    //lastFreeOffset ako minuly folny blok
        char *toBind = nextFreeOffset;  //blok na spojenie
        int isLast = *char2short(nextFreeOffset + stuff / 2);
        nextFreeOffset += *char2short(nextFreeOffset + stuff / 2);  //nextFreeOffset sa posunie na dalsi volny blok

        int newSize = *char2int(tmp) + stuff + *char2int(toBind);   //volny blok sa spoji s uvolnenym blokom
        *char2int(tmp) = newSize;
        for(int i = 0; i < newSize; i++){
            *(tmp + stuff / 2 + i) = 0;
        }
        *char2int(tmp + stuff / 2 + newSize) = newSize;

        *char2short(lastFreeOffset + stuff / 2) = (unsigned short) (tmp - lastFreeOffset);  //aktualizuju sa offsety na volne bloky
        if(isLast != 0){
            *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);
            *char2short(tmp + stuff / 2) = (unsigned short) (nextFreeOffset - tmp);
        } else{
            *char2short(tmp + stuff / 2) = (unsigned short) 0;
        }
        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - lastFreeOffset);


    } else if(*char2int(tmp - stuff/2) > 0 && *char2int(tmp + fullSize) >= 0){       //pred aj za je volne
        char *lastFreeOffset = tmp - *char2int(tmp - stuff / 2) - stuff;    //najde sa lastFreeOffset a nextFreeOffset, a posunu sa este o jeden volny blok do stran
        int isEnd = *char2short(lastFreeOffset + stuff / 2);
        char *nextFreeOffset = tmp + *char2int(tmp) + stuff;
        char *toBindAfter = nextFreeOffset;
        int isLast = *char2short(nextFreeOffset + stuff / 2);
        char *toBindBefore = lastFreeOffset;
        lastFreeOffset -= *char2short(lastFreeOffset + stuff / 2 + freeStuff / 2);
        nextFreeOffset += *char2short(nextFreeOffset + stuff / 2);

        int newSize;    //volne bloky okolo sa spoja s uvolnenym blokom
        if(!isEnd){
            newSize = *char2int(toBindBefore) + stuff + *char2int(tmp);
        } else{
            newSize = *char2int(toBindBefore) + stuff + *char2int(tmp) + stuff + *char2int(toBindAfter);
        }
        tmp = toBindBefore;
        *char2int(tmp) = newSize;
        for(int i = 0; i < newSize; i++){
            *(tmp + stuff / 2 + i) = 0;
        }
        *char2int(tmp + stuff / 2 + newSize) = newSize;

        *char2short(lastFreeOffset + stuff / 2) = (unsigned short) (tmp - lastFreeOffset);      //aktualizuju sa offsety
        if(isEnd == 0){
            return 0;
        }
        if(isLast != 0){
            *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);
            *char2short(tmp + stuff / 2) = (unsigned short) (nextFreeOffset - tmp);
        } else{
            *char2short(tmp + stuff / 2) = (unsigned short) 0;
        }
        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - lastFreeOffset);
    }
    return 0;       //vrati sa 0 (uspesne uvolneny blok)
}

int memory_check(void *ptr){
    char *tmp = (char*) ptr;
    if(tmp == NULL){    //ak je pointer NULL, hned sa vrati 0
        return 0;
    }
    if(*char2int(tmp) < 0 && *(tmp + 4) == -2){     //ak je pointer mensi ako -2 (hlavicka alebo peticka alokovaneho bloku) a o 4 bajty dalej je -2 (miesto v alokovanom bloku)
        return 1;                                   //vrati 1, inak vrati 0
    }else{
        return 0;
    }
}

void memory_init(void *ptr, unsigned int size){
    header = (int*)ptr;     //globalny pointer header sa nastavi na dany pointer
    int staff = 2 * sizeof(int);    //velkost rezie
    for (int i = 0; i < (size + 16); i++){  //cely blok pamate sa vyplni 0
        *(int2char(header) + i) = 0;
    }
    *header = (int)size;    //na prve miesto sa ulozi velkost celeho bloku
    *(header + 1) = 8;      //na 4. miesto sa ulozi 8 (offset na najblizsi volny blok)
    *(header + 2) = (int)(size - (2*sizeof(int) + staff));  //na 8. miesto sa ulozi hlavicka - velkost volneho bloku
    *char2int(int2char(header) + size - sizeof(int)) = (int)(size - (2*sizeof(int) + staff));   //na poslende 4 miesta sa ulozi to iste (paticka)
    *char2short(int2char(header) + 14) = (unsigned short) 8;    //na 14. miesto sa ulozi offset na header

}

// ----------TESTY---------

//int main(void){   //rovnake bloky do pola 50
//    char region[150];
//    memory_init(region, 50);
//    char *pointers[50];
//    int i = 0;
//    do {
//        i++;
//        pointers[i] = memory_alloc(8);
//    } while(pointers[i] != NULL);
//    i = 1;
//    while(pointers[i] != NULL) {
//        memory_free(pointers[i]);
//        i++;
//    }
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}

//int main(void){   //rovkane bloky do pola 50
//    char region[150];
//    memory_init(region, 50);
//    char *pointers[50];
//    int i = 0;
//    do {
//        i++;
//        pointers[i] = memory_alloc(16);
//    } while(pointers[i] != NULL);
//    i = 1;
//    while(pointers[i] != NULL) {
//        memory_free(pointers[i]);
//        i++;
//    }
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}

//int main(void){   //rovnake bloky do pola 100
//    char region[150];
//    memory_init(region, 100);
//    char *pointers[100];
//    int i = 0;
//    do {
//        i++;
//        pointers[i] = memory_alloc(8);
//    } while(pointers[i] != NULL);
//    i = 1;
//    while(pointers[i] != NULL) {
//        memory_free(pointers[i]);
//        i++;
//    }
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}

//int main(void){   //rovnake bloky do pola 100
//    char region[150];
//    memory_init(region, 100);
//    char *pointers[100];
//    int i = 0;
//    do {
//        i++;
//        pointers[i] = memory_alloc(24);
//    } while(pointers[i] != NULL);
//    i = 1;
//    while(pointers[i] != NULL) {
//        memory_free(pointers[i]);
//        i++;
//    }
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}

//int main(void){   //rovnake bloky do pola 200
//    char region[250];
//    memory_init(region, 200);
//    char *pointers[200];
//    int i = 0;
//    do {
//        i++;
//        pointers[i] = memory_alloc(8);
//    } while(pointers[i] != NULL);
//    i = 1;
//    while(pointers[i] != NULL) {
//        memory_free(pointers[i]);
//        pointers[i] = NULL;
//        i++;
//    }
//    i = 0;
//    do {
//        i++;
//        pointers[i] = memory_alloc(16);
//    } while(pointers[i] != NULL);
//    i = 1;
//    while(pointers[i] != NULL) {
//        memory_free(pointers[i]);
//        pointers[i] = NULL;
//        i++;
//    }
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}

//int main(void){     //nahodne bloky do pola 50
//    char region[150];
//    memory_init(region, 50);
//    char *pointers[50];
//    int size;
//    int i = 0;
//    do {
//        i++;
//        size = rand() % 16 + 8;
//        pointers[i] = memory_alloc(size);
//    } while(pointers[i] != NULL);
//    i = 1;
//    while(pointers[i] != NULL) {
//        memory_free(pointers[i]);
//        i++;
//    }
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}

//int main(void){     //nahodne bloky do pola 100
//    char region[150];
//    memory_init(region, 100);
//    char *pointers[100];
//    int size;
//    int i = 0;
//    do {
//        i++;
//        size = rand() % 16 + 8;
//        pointers[i] = memory_alloc(size);
//    } while(pointers[i] != NULL);
//    i = 1;
//    while(pointers[i] != NULL) {
//        memory_free(pointers[i]);
//        i++;
//    }
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}

//int main(void){     //nahodne bloky do pola 100
//    char region[250];
//    memory_init(region, 200);
//    char *pointers[200];
//    int size;
//    int i = 0;
//    do {
//        i++;
//        size = rand() % 16 + 8;
//        pointers[i] = memory_alloc(size);
//    } while(pointers[i] != NULL);
//    i = 1;
//    while(pointers[i] != NULL) {
//        memory_free(pointers[i]);
//        i++;
//    }
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}

//int main(void){     //nahodne velke bloky do pola 5000
//    char region[5050];
//    memory_init(region, 5000);
//    char *pointers[5000];
//    int size;
//    int i = 0;
//    do {
//        i++;
//        size = rand() % 4050 + 50;
//        pointers[i] = memory_alloc(size);
//    } while(pointers[i] != NULL);
//    i = 1;
//    while(pointers[i] != NULL) {
//        memory_free(pointers[i]);
//        i++;
//    }
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}

//int main(void){     //nahodne male a velke bloky do pola 50000
//    char region[50050];
//    memory_init(region, 50000);
//    char *pointers[50000];
//    int size;
//    int i = 0;
//    do {
//        i++;
//        size = rand() % 4092 + 8;
//        pointers[i] = memory_alloc(size);
//    } while(pointers[i] != NULL);
//    i = 1;
//    while(pointers[i] != NULL) {
//        memory_free(pointers[i]);
//        i++;
//    }
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}

//int main(void){     //nahodne bloky do pola 200, striedave alokovanie a uvolnovanie
//    char region[250];
//    memory_init(region, 200);
//    char *pointers[200];
//    int size;
//    int i = 0;
//    do {
//        i++;
//        size = rand() % 16 + 8;
//        pointers[i] = memory_alloc(size);
//        if(i > 1 && memory_check(pointers[i-1])){
//            memory_free(pointers[i-1]);
//        }
//    } while(pointers[i] != NULL && i < 15);
//    memory_free(pointers[i]);
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}

//int main(void){     //nahodne bloky do pola 5000, striedave alokovanie a uvolnovanie
//    char region[5050];
//    memory_init(region, 5000);
//    char *pointers[5000];
//    int size;
//    int i = 0;
//    do {
//        i++;
//        size = rand() % 4092 + 8;
//        pointers[i] = memory_alloc(size);
//        if(i > 1 && memory_check(pointers[i-1])){
//            memory_free(pointers[i-1]);
//        }
//    } while(pointers[i] != NULL && i < 5);
//    if(memory_check(pointers[i])){
//        memory_free(pointers[i]);
//    }
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}

//int main(void){     //nahodne bloky do pola 50000, striedave alokovanie a uvolnovanie
//    char region[50050];
//    memory_init(region, 50000);
//    char *pointers[50000];
//    int size;
//    int i = 0;
//    do {
//        i++;
//        size = rand() % 49092 + 8;
//        pointers[i] = memory_alloc(size);
//        if(i > 1 && memory_check(pointers[i-1])){
//            memory_free(pointers[i-1]);
//        }
//    } while(pointers[i] != NULL && i < 5);
//    if(memory_check(pointers[i])){
//        memory_free(pointers[i]);
//    }
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}

//int main(void){     //ratanie chyby od idealneho riesenia
//    char region[5050];
//    memory_init(region, 5000);
//    char *pointers[5000];
//    int allocated = 0;
//    int allocated_count = 0;
//    int memSize = 5000;
//    int size;
//    int i = 0;
//    int freeSize;
//    do {
//        i++;
//        size = rand() % 200 + 8;
//        freeSize = *char2int(int2char(header) + *char2int(int2char(header) + 4));
//        if(freeSize < size){
//            size = 8;
//        }
//        allocated += size;
//        allocated_count++;
//        pointers[i] = memory_alloc(size);
//    } while(pointers[i] != NULL);
//    i = 1;
//    while(pointers[i] != NULL) {
//        memory_free(pointers[i]);
//        i++;
//    }
//    printf("velkost uspesne alokovanej pamate je %d v %d blokoch na pamat velkosti %d\n", allocated, allocated_count, memSize);
//    printf("To je %.2lf%%\n", (double) allocated / (double) memSize * 100);
//    if(*char2int(region + 8) == *char2int(region) - 16){
//        printf("Successful Test\n");
//    }
//
//    return 0;
//}