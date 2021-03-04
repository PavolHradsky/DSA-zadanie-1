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
unsigned short *char2short(char *ptr){
    return ((unsigned short*)ptr);
}
char *short2char(unsigned short *ptr){
    return ((char*)ptr);
}

//void *memory_alloc(unsigned int size){
//    //stuff definition
//    int stuff = 2 * sizeof(int);
//    int freeStuff = 2 * sizeof(unsigned short);
//    int fullSize = (int)size + stuff;
//
//    //zapamatanie dolezitych miest v pamati
//    char *tmp = int2char(header);
//    char *firstOffset = tmp + 4;
//    char *firstFree = (firstOffset) + *char2int((tmp+4));
//    char *lastFreeOffset = firstFree;
//    char *nextFreeOffset;
//    tmp += *char2int(firstFree);
//
//    while(*char2int(tmp) < (int) size && *char2short(tmp + stuff / 2) != 0){
//        size += 0;
//        lastFreeOffset = tmp;
//        tmp += *char2short(tmp + stuff / 2);
//    }
//    if(tmp + fullSize > int2char(header) + *header){
//        //printf("tmp+fullSeze: %x, \nheader+*header: %x\n", (int)tmp + fullSize, (int)int2char(header) + *header);
//        printf("Velka pamat\n");
//        return NULL;
//    }
//    if(*char2int(tmp) < size + stuff + freeStuff){
//        size = *char2int(tmp);
//        //printf("SIZE: %d\n", size);
//        fullSize = (int) size + stuff;
//    }
//    int oldSize = *char2int(tmp);
//    int isLast = *char2short(tmp + stuff / 2);
//    if(!isLast){
//        nextFreeOffset = tmp + *char2short(tmp + stuff / 2);
//    }
//    char *toReturn = tmp;
//
//    //alokovanie miesta
//    *(char2int(tmp)) = (int)size * (-1);
//    tmp += stuff/2;
//    for(int i = 0; i < size; i++){
//        *tmp = -2;
//        tmp += 1;
//    }
//    *(char2int(tmp)) = (int)size * (-1);
//    tmp += stuff/2;
//    //printf("%d\n", *char2int(tmp));
//
//
//
//    //update volnych miest
//    if(tmp == 0 && tmp < int2char(header) + *header){
//        *char2int(tmp) = oldSize - fullSize;
//        *char2int(tmp + oldSize - fullSize + stuff / 2) = oldSize - fullSize;
//    }
//    if(!isLast){
//        *char2short(lastFreeOffset + stuff / 2) = (unsigned short) (tmp - lastFreeOffset);
//        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - lastFreeOffset);
//        *char2short(tmp + stuff / 2) = (unsigned short) (nextFreeOffset - tmp);
//        *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);
//
//    } else{
//        *char2short(tmp + stuff / 2) = (unsigned short) (tmp - lastFreeOffset);
//        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - lastFreeOffset);
//        *char2short(lastFreeOffset + stuff / 2) = (unsigned short) 0;
//    }
//    return toReturn;
//
//
//}

void *memory_alloc(unsigned int size){

//stuff definition
    int stuff = 2 * sizeof(int);
    int freeStuff = 2 * sizeof(unsigned short);
    int fullSize = (int)size + stuff;

//zapamatanie dolezitych miest v pamati
    char *tmp = int2char(header);
    char *firstOffset = tmp + 4;
    char *firstFree = (firstOffset) + *char2int((tmp+4));
//dalsie dolezite hodnoty
    tmp += *char2int(tmp + stuff / 2);
    int freeSizeBefore = *(char2int(tmp));
    unsigned short nextFreeOffset = *(char2short(tmp + stuff/2));
    char *nextFreeOffset_place = tmp + stuff/2;
    unsigned short lastFreeOffset = *(char2int(tmp + stuff/2 + freeStuff/2));
    char *lastFreeOffset_place = tmp + stuff/2 + freeStuff/2;
    char *lastFreeBlock = int2char(header);
//ak chcem alokovat viac ako mam miesta - 1. podmienka
    int betweenIsFree = 0;

    while(*char2int(tmp) < (int) size && *char2short(tmp + stuff / 2) != 0){
        betweenIsFree = 1;
        size += 0;
        lastFreeBlock = tmp;
        tmp += *char2short(tmp + stuff / 2);
    }
    if(tmp + fullSize > int2char(header) + *header){
        //printf("tmp+fullSeze: %x, \nheader+*header: %x\n", (int)tmp + fullSize, (int)int2char(header) + *header);
        //printf("Velka pamat\n");
        return NULL;
    }

    if(*char2int(tmp) < size + stuff + freeStuff){
        size = *char2int(tmp);
        //printf("SIZE: %d\n", size);
        fullSize = (int) size + stuff;
    }
    char *nextFreeBlock = tmp + *char2short(tmp + stuff / 2);
    freeSizeBefore = *(char2int(tmp));
    nextFreeOffset = *(char2short(tmp + stuff/2));
    nextFreeOffset_place = tmp + stuff/2;
    lastFreeOffset = *(char2int(tmp + stuff/2 + freeStuff/2));
    lastFreeOffset_place = tmp + stuff/2 + freeStuff/2;

    char *toReturn = tmp;
    unsigned short isLast = 1;
    if(*char2int(tmp) == size && *char2short(tmp + stuff / 2) == 0){
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

    if(*char2int(tmp) >= 0){
        if(!isLast){
            if(betweenIsFree){
                *char2short(lastFreeOffset_place) = 0;

            } else{
                *char2int(firstOffset) = *header;
            }
        } else{
            *(char2int(tmp)) = freeSizeBefore - fullSize;
            *(char2int(tmp + freeSizeBefore - (fullSize - stuff/2))) = freeSizeBefore - fullSize;
            if(nextFreeOffset == 0){
                *char2short(tmp + stuff/2) = 0;
            }else{
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
    }else {
        *char2short(lastFreeBlock + stuff / 2) = (unsigned short) (nextFreeBlock - lastFreeBlock);
        *char2short(nextFreeBlock + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeBlock - lastFreeBlock);
    }


    return toReturn;

}

int memory_free(void *valid_ptr){

//stuff definition
    int stuff = 2 * sizeof(int);
    int freeStuff = 2 * sizeof(unsigned short);


//zapamatanie dolezitych miest v pamati
    char *tmp = (char*)valid_ptr;
    int size = *char2int(tmp) * (-1);
    int fullSize = *char2int(tmp) * (-1) + stuff;

//Uvolni pamat
    *char2int(tmp) *= (-1);
    tmp += 4;
    for(int i = 0; i < size; i++){
        *tmp = 0;
        tmp += 1;
    }
    *char2int(tmp) *= (-1);
    tmp -= (fullSize - stuff/2);
//Pospaja volne bloky a aktualizuje ukazovatele
    if(*char2int(int2char(header) + 4) == *header){
        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - int2char(header));
        *char2short(int2char(header) + 4) = (unsigned short) (tmp - int2char(header));
    }
    else if(tmp == int2char(header) + stuff && *char2int(tmp + fullSize) < 0) {    //prvy pointer - za nie je volne
        //printf("Case 0\n");

        char *lastFreeOffset = (int2char(header) + stuff / 2);
        char *nextFreeOffset = (int2char(header) + *lastFreeOffset);
        *char2short(tmp + stuff / 2) = (unsigned short) (*lastFreeOffset - 8);
        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - int2char(header));
        *char2short(lastFreeOffset) = (unsigned short) (tmp - int2char(header));
        *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);

    } else if(tmp == int2char(header) + stuff && *char2int(tmp + fullSize) > 0){    //prvy pointer - za je volne
        //printf("Case 0_v1\n");

        char *lastFreeOffset = (int2char(header) + stuff / 2);
        char *nextFreeOffset = (int2char(header) + *lastFreeOffset);
        int isLast = *char2short(nextFreeOffset + stuff / 2);
        nextFreeOffset += *char2short(nextFreeOffset + stuff / 2);
        *char2short(tmp + stuff / 2) = (unsigned short) (*lastFreeOffset - 8);
        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - int2char(header));
        *char2short(lastFreeOffset) = (unsigned short) (tmp - int2char(header));

        *char2int(tmp) += *char2int(tmp + fullSize) + stuff;
        int newSize = *char2int(tmp);
        tmp += stuff;
        for(int i = 0; i < (newSize - stuff / 2); i++){
            *tmp = 0;
            tmp += 1;
        }
        *char2int(tmp) = newSize;
        tmp -= (newSize + stuff / 2);
        //printf("%d\n", *char2int(tmp));
        if(isLast != 0){
            *char2short(tmp + stuff / 2) = (unsigned short) (nextFreeOffset - tmp);
            *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);
        } else{
            *char2short(tmp + stuff / 2) = (unsigned short) 0;
        }


    } else if(*char2int(tmp - stuff/2) < 0 && *char2int(tmp + fullSize) < 0){       //pred za nie je volne
        //printf("Case 1\n");
        char *firstFree = tmp;
        firstFree += *char2int(firstFree) + stuff;
        while(*char2int(firstFree) < 0 && (firstFree + stuff) < (int2char(header) + *header)){
            firstFree += (*char2int(firstFree) * (-1) + stuff);
        }
        char *lastFreeOffset = (firstFree - *(firstFree + stuff / 2 + freeStuff / 2));
        char *nextFreeOffset = firstFree;
        //printf("%d\n", *char2short(lastFreeOffset));
        //printf("%d\n", *char2short(nextFreeOffset));
        *char2short(lastFreeOffset + stuff/2) = (unsigned short) (tmp - lastFreeOffset);

        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - lastFreeOffset);

        *char2short(tmp + stuff / 2) = (unsigned short) (nextFreeOffset - tmp);

        *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);


    } else if(*char2int(tmp - stuff/2) > 0 && *char2int(tmp + fullSize) < 0){       //pred je volne
        //printf("Case 2\n");

        char *lastFreeOffset = tmp - (*char2int(tmp - stuff / 2) + stuff);
        char *nextFreeOffset = lastFreeOffset + *char2short(lastFreeOffset + stuff / 2);
        char *toBind = lastFreeOffset;
        lastFreeOffset -= *char2short(lastFreeOffset + stuff / 2 + freeStuff / 2);

        int newSize = *char2int(toBind) + stuff + *char2int(tmp);
        tmp = toBind;
        *char2int(tmp) = newSize;
        for(int i = 0; i < newSize; i++){
            *(tmp + stuff / 2 + i) = 0;
        }
        *char2int(tmp + stuff / 2 + newSize) = newSize;

        *char2short(lastFreeOffset + stuff / 2) = (unsigned short) (tmp - lastFreeOffset);
        *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);
        *char2short(tmp + stuff / 2) = (unsigned short) (nextFreeOffset - tmp);
        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - lastFreeOffset);


    } else if(*char2int(tmp - stuff/2) < 0 && *char2int(tmp + fullSize) > 0){       //za je volne
        //printf("Case 3\n");
        char *nextFreeOffset = tmp + *char2int(tmp) + stuff;
        char *lastFreeOffset = nextFreeOffset - *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2);
        char *toBind = nextFreeOffset;
        int isLast = *char2short(nextFreeOffset + stuff / 2);
        nextFreeOffset += *char2short(nextFreeOffset + stuff / 2);

        int newSize = *char2int(tmp) + stuff + *char2int(toBind);
        *char2int(tmp) = newSize;
        for(int i = 0; i < newSize; i++){
            *(tmp + stuff / 2 + i) = 0;
        }
        *char2int(tmp + stuff / 2 + newSize) = newSize;

        *char2short(lastFreeOffset + stuff / 2) = (unsigned short) (tmp - lastFreeOffset);
        if(isLast != 0){
            *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);
            *char2short(tmp + stuff / 2) = (unsigned short) (nextFreeOffset - tmp);
        } else{
            *char2short(tmp + stuff / 2) = (unsigned short) 0;
        }
        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - lastFreeOffset);


    } else if(*char2int(tmp - stuff/2) > 0 && *char2int(tmp + fullSize) > 0){       //pred aj za je volne
        //printf("Case 4\n");
        char *lastFreeOffset = tmp - *char2int(tmp - stuff / 2) - stuff;
        int isEnd = *char2short(lastFreeOffset + stuff / 2);
        char *nextFreeOffset = tmp + *char2int(tmp) + stuff;
        char *toBindAfter = nextFreeOffset;
        int isLast = *char2short(nextFreeOffset + stuff / 2);
        //char *lastFreeOffset = tmp - *char2int(tmp - stuff / 2) - stuff;
        char *toBindBefore = lastFreeOffset;
        //printf("%d\n", *char2int(lastFreeOffset));
        //printf("%d\n", *char2int(nextFreeOffset));
        lastFreeOffset -= *char2short(lastFreeOffset + stuff / 2 + freeStuff / 2);
        nextFreeOffset += *char2short(nextFreeOffset + stuff / 2);
        //printf("%d\n", *char2int(lastFreeOffset));
        //printf("%d\n", *char2int(nextFreeOffset));

        int newSize;
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

        *char2short(lastFreeOffset + stuff / 2) = (unsigned short) (tmp - lastFreeOffset);
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
    return 0;
}

int memory_check(void *ptr){
    char *tmp = (char*) ptr;
    if(*char2int(tmp) < 0 && *(tmp + 4) == -2){
        return 1;
    }else{
        return 0;
    }
}

void memory_init(void *ptr, unsigned int size){
    header = (int*)ptr;
    int staff = 2 * sizeof(int);
    for (int i = 0; i < size; i++){
        *(int2char(header) + i) = 0;
    }
    *header = (int)size;
    *(header + 1) = 8;
    *(header + 2) = (int)(size - (2*sizeof(int) + staff));
    *char2int(int2char(header) + size - sizeof(int)) = (int)(size - (2*sizeof(int) + staff));
    *char2short(int2char(header) + 14) = (unsigned short) 8;

}

void z1_testovac(char *region, char **pointer, int minBlock, int maxBlock, int minMemory, int maxMemory, int testFragDefrag) {
    unsigned int allocated = 0;
    unsigned int mallocated = 0;
    unsigned int allocated_count = 0;
    unsigned int mallocated_count = 0;
    unsigned int i = 0;
    int random_memory = 0;
    int random = 0;
    memset(region, 0, 100000);
    random_memory = (rand() % (maxMemory-minMemory+1)) + minMemory;
    memory_init(region + 500, random_memory);
    if (testFragDefrag) {
        do {
            pointer[i] = memory_alloc(8);
            if (pointer[i])
                i++;
        } while (pointer[i]);
        for (int j = 0; j < i; j++) {
            if (memory_check(pointer[j])) {
                memory_free(pointer[j]);
            }
            else {
                printf("Error: Wrong memory check.\n");
            }
        }
    }
    i = 0;
    while (allocated <= random_memory-minBlock) {
        random = (rand() % (maxBlock-minBlock+1)) + minBlock;
        if (allocated + random > random_memory)
            continue;
        allocated += random;
        allocated_count++;
        pointer[i] = memory_alloc(random);
        if (pointer[i]) {
            i++;
            mallocated_count++;
            mallocated += random;
        }
    }
    for (int j = 0; j < i; j++) {
        if (memory_check(pointer[j])) {
            memory_free(pointer[j]);
        }
        else {
            printf("Error: Wrong memory check.\n");
        }
    }
    memset(region + 500, 0, random_memory);
    for (int j = 0; j < 100000; j++) {
        if (region[j] != 0) {
            region[j] = 0;
            printf("Error: Modified memory outside the managed region. index: %d\n",j-500);
        }
    }
    float result = ((float)mallocated_count / allocated_count) * 100;
    float result_bytes = ((float)mallocated / allocated) * 100;
    printf("Memory size of %d bytes: allocated %.2f%% blocks (%.2f%% bytes).\n", random_memory, result, result_bytes);
}


int main() {
    char region[100000];
    char* pointer[13000];
    z1_testovac(region, pointer, 8, 24, 50, 100, 1);
    z1_testovac(region, pointer, 8, 1000, 10000, 20000, 0);
    z1_testovac(region, pointer, 8, 35000, 50000, 60000, 0);

    return 0;
}


//int main(void){
//
//
//    char region[100];
//    memory_init(region, 100);
//
//
//    char *pointer = memory_alloc(20);
//    char *pointer1 = memory_alloc(12);
//    memory_free(pointer);
//    char *pointer2 = memory_alloc(10);
//
//    char *pointer3 = memory_alloc(4);
//    char *pointer4 = memory_alloc(10);
//    char *pointer5 = memory_alloc(4);
//
//
//    if(memory_check(pointer2)){
//        printf("pointer 2\n");
//    }
//    if(memory_check(pointer)){
//        printf("pointer\n");
//    }
//    printf("%d\n", *char2int(pointer2));
//    memory_free(pointer4);
//    memory_free(pointer1);
//
//    memory_free(pointer2);
//    memory_free(pointer3);
//    memory_free(pointer5);
//
//
//    return 0;
//}