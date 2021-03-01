/***********************************
 * DSA Zadianie 1 - Spravca pamati
 * 
 * FIIT STU 2021
 * Pavol Hradsky
************************************/

#include <stdio.h>

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

void freeSpotUpdate(){                                       //hladanie najblizsieho volneho miesta v pamati
    int stuff = 2*sizeof(int);
    char *tmp = int2char(header);                           //vytvorenie docastneho pointeru, s ktorym sa budem hybat po pamati

    tmp += stuff/2;
    int *freeSpot = char2int(tmp);
    printf("freeSpot: %d\n", *freeSpot);

    //tmp += *freeSpot;
    tmp += stuff/2;

    while(*(char2int(tmp)) < 0){
        printf("tmpNow: %d\n", *(char2int(tmp)));
        tmp += (*(char2int(tmp)) * (-1)) + stuff;
    }
    printf("tmp-freeSpot: %d\n", tmp - int2char(freeSpot));

    int newSize = (int)(tmp - int2char(freeSpot));

    if(*freeSpot > newSize){
        *char2int(tmp + stuff/2) = *freeSpot;
    }

    *freeSpot = newSize;
    printf("freeSpotNow: %d\n", *freeSpot);


}

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
//ak chcem alokovat viac ako mam miesta - 1. podmienka
    if(tmp + fullSize > int2char(header) + *header){
        printf("tmp+fullSeze: %x, \nheader+*header: %x\n", (int)tmp + fullSize, (int)int2char(header) + *header);
        printf("Velka pamat\n");
        return NULL;
    }
    char *toReturn = tmp;
//alokovanie miesta
    *(char2int(tmp)) = (int)size * (-1);
    tmp += stuff/2;
    for(int i = 0; i < size; i++){
        *tmp = -2;
        tmp += 1;
    }
    *(char2int(tmp)) = (int)size * (-1);
    tmp += stuff/2;
//nastavienie volnych offsetov (aj aktualizovanie minuleho a buduceho)
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

    return toReturn;

}

//void *memory_alloc(unsigned int size){
//    int stuff = 2 * sizeof(int);                            //velkost hlavicky a paticky
//    char *tmp = int2char(header);                           //vytvorenie pointera, s ktorym sa budem hybat po pamati
//    tmp += sizeof(int);                                     //posun na miesto, kde ukladam najblizsie volne miesto
//    printf("nextFree in alloc: %d\n", *(char2int(tmp)));
//    tmp += *(char2int(tmp));                                            //posun na najblizsie volne miesto
//    printf("tmpAfterJMP: %d\n", *(char2int(tmp)));
//
//    int currentSize = *(char2int(tmp));                     //ulozenie velkosti volneho bloku
//
//    if ((tmp + stuff + size) > (int2char(header) + *header - stuff/2)){
//        return NULL;
//    }
//
//    if(*char2int(tmp) < size){
//        tmp += *char2int(tmp + stuff/2);
//    }
//
//    if(currentSize < stuff + size){
//        printf("CURRENT SIZE JE MENSIA AKO STUFF\n");
//        size = currentSize;
//    }
//
//    char *result = tmp;
//    *char2int(tmp) = (int)(size * (-1));                    //ulozenie novej velkosti v zapornom tvare (alokovana)
////    printf("tmp now: %d\n", *(char2int(tmp)));
//    //*(char2int(tmp)) = (int)size + stuff/3 * 2;
//    //*(char2int(tmp)) = 0;
//    tmp += stuff/2;                                         //posun o hlavicku
//    for(int i = 0; i < size; i++){                          //zapis size-krat -2 (miesto pouzitelne pre pouzivatela)
//        *(tmp + i) = -2;
//    }
//    tmp += size;                                            //posun na koniec alokovaneho miesta
//    *char2int(tmp) = (int)size * (-1);                      //zapisanie paticky v rovnakom tvare ako hlavicka
//
//
//
//    tmp += stuff/2;                                         //posun o paticku
//    *char2int(tmp) = (int)(currentSize - (size + stuff));   //vytvorenie (zapisanie) noveho volneho miesta
//    //    printf("tmp now: %d\n", *(char2int(tmp)));
//    tmp += stuff/2;
//    *(char2short(tmp)) = (unsigned int)NULL;
//    tmp += stuff/4;
//    *(char2short(tmp)) = (unsigned short)(tmp - int2char(header + 1));
//    tmp -= stuff/4;
//    tmp -= stuff/2;
//    tmp += *(char2int(tmp));                                //posun o toto volne miesto
//    tmp += stuff;                                         //posun o hlavicku a smernik na volny blok
//    *char2int(tmp) = (int)(currentSize - (size + stuff));   //update paticky volneho miesta
////    printf("tmp now: %d\n", *(char2int(tmp)));
//
//    freeSpotUpdate();                                        //zavolanie funkcie, ktora najde najblizsie volne miesto od zaciatku a aktualizuje udaj na 4. mieste v pamati
//
////    printf("tmp now: %d\n", *tmp);
//    return result;
//}

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
    if(tmp == int2char(header) + stuff && *char2int(tmp + fullSize) < 0) {    //prvy pointer - za nie je volne
        printf("Case 0\n");

        char *lastFreeOffset = (int2char(header) + stuff / 2);
        char *nextFreeOffset = (int2char(header) + *lastFreeOffset);
        *char2short(tmp + stuff / 2) = (unsigned short) (*lastFreeOffset - 8);
        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - int2char(header));
        *char2short(lastFreeOffset) = (unsigned short) (tmp - int2char(header));
        *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);

    } else if(tmp == int2char(header) + stuff && *char2int(tmp + fullSize) > 0){    //prvy pointer - za je volne
        printf("Case 0_v1\n");

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
        printf("%d\n", *char2int(tmp));
        if(isLast != 0){
            *char2short(tmp + stuff / 2) = (unsigned short) (nextFreeOffset - tmp);
            *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);
        } else{
            *char2short(tmp + stuff / 2) = (unsigned short) 0;
        }


    } else if(*char2int(tmp - stuff/2) < 0 && *char2int(tmp + fullSize) < 0){       //pred za nie je volne
        printf("Case 1\n");
        char *firstFree = tmp;
        firstFree += *char2int(firstFree) + stuff;
        while(*char2int(firstFree) < 0 && (firstFree + stuff) < (int2char(header) + *header)){
            firstFree += (*char2int(firstFree) * (-1) + stuff);
        }
        char *lastFreeOffset = (firstFree - *(firstFree + stuff / 2 + freeStuff / 2));
        char *nextFreeOffset = firstFree;
        printf("%d\n", *char2short(lastFreeOffset));
        printf("%d\n", *char2short(nextFreeOffset));
        *char2short(lastFreeOffset + stuff/2) = (unsigned short) (tmp - lastFreeOffset);

        *char2short(tmp + stuff / 2 + freeStuff / 2) = (unsigned short) (tmp - lastFreeOffset);

        *char2short(tmp + stuff / 2) = (unsigned short) (nextFreeOffset - tmp);

        *char2short(nextFreeOffset + stuff / 2 + freeStuff / 2) = (unsigned short) (nextFreeOffset - tmp);


    } else if(*char2int(tmp - stuff/2) > 0 && *char2int(tmp + fullSize) < 0){       //pred je volne
        printf("Case 2\n");

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
        printf("Case 3\n");
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
        printf("Case 4\n");

        char *nextFreeOffset = tmp + *char2int(tmp) + stuff;
        char *toBindAfter = nextFreeOffset;
        int isLast = *char2short(nextFreeOffset + stuff / 2);
        char *lastFreeOffset = tmp - *char2int(tmp - stuff / 2) - stuff;
        char *toBindBefore = lastFreeOffset;
        printf("%d\n", *char2int(lastFreeOffset));
        printf("%d\n", *char2int(nextFreeOffset));
        lastFreeOffset -= *char2short(lastFreeOffset + stuff / 2 + freeStuff / 2);
        nextFreeOffset += *char2short(nextFreeOffset + stuff / 2);
        printf("%d\n", *char2int(lastFreeOffset));
        printf("%d\n", *char2int(nextFreeOffset));


        int newSize = *char2int(toBindBefore) + stuff + *char2int(tmp) + stuff + *char2int(toBindAfter);
        tmp = toBindBefore;
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
    }
}


//int memory_free(void *valid_ptr){
//    printf("--------FREE--------\n");
//
//    int stuff = 3 * sizeof(int);
//    char *tmp = valid_ptr;
//    int size = *(char2int(tmp)) * (-1);
//    printf("%d\n", size);
//    printf("%d\n", *(char2int(tmp - stuff/3)));
//    printf("%d\n", *(char2int(tmp + size + stuff)));
//    if(*(char2int(tmp - stuff/3)) < 0 && *(char2int(tmp + size + stuff)) < 0){
//        printf("Case 0\n");
//        *(char2int(tmp)) *= (-1);
//        tmp += stuff/3 * 2;
//        for(int i = 0; i < size; i++){
//            *(tmp + i) = 0;
//        }
//        tmp += size;
//        *(char2int(tmp)) *= (-1);
//    }
//
//    freeSpotUpdate();
//}

//void *memory_alloc(unsigned int size){
//    char *tmp = header + sizeof(int);
//    tmp = tmp + (*tmp);
//    int lastSize;
//    int staff = 2;
//
//    if(*tmp > 0){
//        lastSize = *tmp;
//        if ((tmp + size + staff) > header + *header){
//            return NULL;
//        }
//        *tmp = size * (-1);
//        for(int i = 0; i < size; i++){
//            tmp++;
//            *tmp = -1;
//        }
//        tmp++;
//        *tmp = size * -1;
//        tmp++;
//        *tmp = lastSize - size - staff;
//        tmp += *tmp + staff/2;
//        *tmp = lastSize - size - staff;
//        *(header + sizeof(int)) += (size + staff);
//
//        return (tmp - size - staff);
//    }else{
//        return NULL;
//    }
//}
//
//int memory_free(void *valid_ptr){
//    char *tmp = valid_ptr;
//    char *pointer_free = (header + sizeof(int));
//    *tmp *= (-1);
//    int size = *tmp;
//    int staff = 2;
//
//    if((*(tmp - staff) == -1 || (tmp - sizeof(int) == pointer_free)) && *(tmp + size + staff) < -1){
//        printf("Case 1");
//        for(int i = 1; i < (size); i++){
//            *(tmp + i) = 0;
//        }
//        *(tmp + size) = size;
//        *(pointer_free) = tmp-pointer_free;
//    }else if((*(tmp - staff) == -1 || (tmp - sizeof(int) == pointer_free)) && *(tmp + size + staff) > 0) {
//        printf("Case 2");
//        for (int i = 1; i < (size + staff); i++) {
//            *(tmp + i) = 0;
//        }
//        int size1 = *(tmp + size + staff);
//        *(tmp) += size1;
//        *(tmp + size + staff) = 0;
//        *(pointer_free) = tmp - pointer_free;
//    }else if(*(tmp - staff) == 0 && *(tmp + size + staff) < -1){
//        printf("Case 3");
//        for (int i = 1; i < (size + staff); i++) {
//            *(tmp + i) = 0;
//        }
//
//    }
//
//}
//
//int memory_check(void *ptr){
//
//}


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

int main(void){


    char region[100];
    memory_init(region, 100);


    char *pointer = memory_alloc(12);
    char *pointer1 = memory_alloc(10);
    char *pointer2 = memory_alloc(12);
    char *pointer3 = memory_alloc(10);

    memory_free(pointer1);
    memory_free(pointer);
    memory_free(pointer2);
    memory_free(pointer3);


    return 0;
}