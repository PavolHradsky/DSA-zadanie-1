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

void freeSpotUpdate(){                                       //hladanie najblizsieho volneho miesta v pamati
    int stuff = 3*sizeof(int);
    char *tmp = int2char(header);                           //vytvorenie docastneho pointeru, s ktorym sa budem hybat po pamati

    tmp += stuff/3;
    int *freeSpot = char2int(tmp);
    printf("freeSpot: %d\n", *freeSpot);

    tmp += *freeSpot;

    while(*(char2int(tmp)) < 0){
        printf("tmpNow: %d\n", *(char2int(tmp)));
        tmp += (*(char2int(tmp)) * (-1)) + stuff;
    }
    printf("tmp-freeSpot: %d\n", tmp - int2char(freeSpot));
    *freeSpot = tmp - int2char(freeSpot);
    printf("freeSpotNow: %d\n", *freeSpot);


}

void *memory_alloc(unsigned int size){
    int stuff = 3 * sizeof(int);                            //velkost hlavicky a paticky
    char *tmp = int2char(header);                           //vytvorenie pointera, s ktorym sa budem hybat po pamati
    tmp += sizeof(int);                                     //posun na miesto, kde ukladam najblizsie volne miesto
    printf("nextFree in alloc: %d\n", *(char2int(tmp)));
    tmp += *(char2int(tmp));                                            //posun na najblizsie volne miesto
    printf("tmpAfterJMP: %d\n", *(char2int(tmp)));

    int currentSize = *(char2int(tmp));                     //ulozenie velkosti volneho bloku

    if ((tmp + stuff + size) > (int2char(header) + *header - stuff/3)){
        return NULL;
    }

    char *result = tmp;
    *char2int(tmp) = (int)(size * (-1));                    //ulozenie novej velkosti v zapornom tvare (alokovana)
//    printf("tmp now: %d\n", *(char2int(tmp)));
    tmp += stuff/3;
    *(char2int(tmp)) = size + stuff/3 * 2;
    tmp += stuff/3;                                         //posun o hlavicku
    for(int i = 0; i < size; i++){                          //zapis size-krat -2 (miesto pouzitelne pre pouzivatela)
        *(tmp + i) = -2;
    }
    tmp += size;                                            //posun na koniec alokovaneho miesta
    *char2int(tmp) = (int)size * (-1);                      //zapisanie paticky v rovnakom tvare ako hlavicka



    tmp += stuff/3;                                         //posun o paticku
    *char2int(tmp) = (int)(currentSize - (size + stuff));   //vytvorenie (zapisanie) noveho volneho miesta
    //    printf("tmp now: %d\n", *(char2int(tmp)));
    tmp += *(char2int(tmp));                                //posun o toto volne miesto
    tmp += stuff/3 * 2;                                         //posun o hlavicku a smernik na volny blok
    *char2int(tmp) = (int)(currentSize - (size + stuff));   //update paticky volneho miesta
//    printf("tmp now: %d\n", *(char2int(tmp)));

    freeSpotUpdate();                                        //zavolanie funkcie, ktora najde najblizsie volne miesto od zaciatku a aktualizuje udaj na 4. mieste v pamati

//    printf("tmp now: %d\n", *tmp);
    return result;
}

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
    *(header + 1) = sizeof(int);
    *(header + 2) = (int)(size - (3*sizeof(int) + staff));
    *char2int(int2char(header) + size - sizeof(int)) = (int)(size - (3*sizeof(int) + staff));


}

int main(void){


    char region[100];
    memory_init(region, 100);
    printf("reg[0]: %d\n", *char2int(region));
    printf("reg[4]: %d\n", *char2int(region + 4));
    printf("reg[8]: %d\n", *char2int(region + 8));
//    printf("reg[96]: %d\n", *char2int(region + 96));

    char *pointer = memory_alloc(12);
    char *pointer1 = memory_alloc(23);
    char *pointer2 = memory_alloc(23);

//    char *pointer = memory_alloc(10);
//    char *pointer1 = memory_alloc(12);
//    char *pointer2 = memory_alloc(24);
//    char *pointer3 = memory_alloc(12);
//
//    memory_free(pointer);
//    memory_free(pointer3);
//    memory_free(pointer1);

    return 0;
}