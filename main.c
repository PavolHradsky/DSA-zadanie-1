/**
 * DSA Zadianie 1 - Spravca pamati
 * 
 * FIIT STU 2021
 * Pavol Hradsky
*/

#include <stdio.h>
static char *header;
void *memory_alloc(unsigned int size){
    char *tmp = header + sizeof(int);
    tmp = tmp + (*tmp);
    int lastSize;
    if(*tmp > 0){
        lastSize = *tmp;
        if ((tmp + size) > header + *header){
            return NULL;
        }
        *tmp = size * (-1);
        for(int i = 0; i < size; i++){
            tmp++;
            *tmp = -1;
        }
        tmp++;
        *tmp = lastSize - size;
        *(header + sizeof(int)) += (size+1);
        return (tmp - size);
    }else{
        return NULL;
    }
}

int memory_free(void *valid_ptr){

}

int memory_check(void *ptr){

}


void memory_init(void *ptr, unsigned int size){
    header = (char*)ptr;
    for (int i = 0; i < size; i++){
        *(header + i) = 0;
    }
    *header = size;
    *(header + sizeof(int)) = 4;
    *(header + 2*sizeof(int)) = (size-8);


}

int main(void){

    char region[50];
    memory_init(region, 50);
    char *pointer = memory_alloc(10);
    char *pointer1 = memory_alloc(12);
    char *pointer2 = memory_alloc(24);
    char *pointer3 = memory_alloc(12);

    return 0;
}