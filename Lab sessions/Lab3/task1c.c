#include <stdio.h>
#include <malloc.h>
#include <mem.h>

typedef struct virus virus;
typedef struct link link;

struct virus {
    unsigned short length;
    char name[16];
    char signature[];
};

struct link {
    virus *v;
    link *next;
};

void PrintHex(char *buffer, long length);

void list_print(link *virus_list);
/* Print the data of every link in list. Each item followed by a newline character. */

link* list_append(link* virus_list, virus* data);
/* Add a new link with the given data to the list
   (either at the end or the beginning, depending on what your TA tells you),
   and return a pointer to the list (i.e., the first link in the list).
   If the list is null - create a new entry and return a pointer to the entry. */

void list_free(link *virus_list); /* Free the memory allocated by the list. */

void detect_virus(char *buffer, link *virus_list, unsigned int size);

void readSignaturesFile(FILE *fileSignatures, link **virus_list);

void readInfectedFile(FILE *fileInfected, char **buffer, unsigned int *fileSize);

void delete_virus(char *buffer, link *virus_list, unsigned int fileSize, char *fileName);

int main(int argc, char **argv) {
    char *fileName = argv[1];
    FILE *fileInfected = fopen(fileName, "rb+");
    FILE *fileSignatures = fopen("signatures", "rb+");

    char *buffer;
    unsigned int fileSize;
    link *virus_list = NULL;

    if(fileInfected == NULL){
        perror("Error Occured");
        printf("Error code: %d\n", errno);

        printf("File Being Created\n\n");
        fileInfected = fopen(fileName, "wb+");

        char data[] = "Eden Keshet";
        fwrite(data, sizeof(data[0]), sizeof(data)/ sizeof(data[0]), fileInfected);

        if(fileInfected == NULL) {
            perror("Error Occured");
            printf("Error code: %d\n", errno);
            exit(1);
        }
    }

    if(fileSignatures == NULL){
        perror("Error Occured");
        printf("Error code: %d\n", errno);
        exit(1);
    }

/*
    Start of the program
*/

    readSignaturesFile(fileSignatures, &virus_list);

    list_print(virus_list);

    readInfectedFile(fileInfected, &buffer, &fileSize);

    detect_virus(buffer, virus_list, fileSize);

/*
    //##    delete_virus    ##
*/
    delete_virus(buffer, virus_list, fileSize, fileName);

    readInfectedFile(fileInfected, &buffer, &fileSize);

    detect_virus(buffer, virus_list, fileSize);

/*
    free Memory
*/
    free(buffer);
    list_free(virus_list);
    fclose(fileInfected);

    return 0;
}

void readInfectedFile(FILE *fileInfected, char **buffer, unsigned int *fileSize) {
    rewind(fileInfected);

    fseek(fileInfected, 0 , SEEK_END);

    *fileSize = (unsigned int) ftell(fileInfected);

    rewind(fileInfected);

    *buffer = (char*) malloc(sizeof(char)*(*fileSize));

    if(*buffer == NULL){
        perror("Error Occured");
        printf("Error code: %d\n", errno);
        exit(2);
    }

    size_t dataInFile = fread(*buffer, 1, *fileSize, fileInfected);

    if(dataInFile != *fileSize){
        perror("Error Occurred");
        printf("Error code: %d\n", errno);
        exit(3);
    }
}

void readSignaturesFile(FILE *fileSignatures, link **virus_list) {
    rewind(fileSignatures);

    char header[1];
    char length[2];

    fread(header, 1, 1, fileSignatures);

    while (1){
        int length1 = fread(length, 1, 2, fileSignatures);

        if (length1 ==0)
            break;

        int lengthTmp = 0;

        if (header[0]==1)
            lengthTmp = length[1] | length[0] << 8;
        else if (header[0]==1)
            lengthTmp = length[0] | length[1] << 8;

        virus *virusTmp = (virus*) malloc((size_t) lengthTmp);
        virusTmp->length = (unsigned short) lengthTmp;

        fread(virusTmp->name, 1, (size_t) (virusTmp->length - 2), fileSignatures);

        *virus_list = list_append(*virus_list, virusTmp);
    }
    fclose(fileSignatures);
}

/*

link *startRead2(link *viresus, FILE *fileSignatures) {
    char header[1];
    char length[2];

    fread(header, 1, 1, fileSignatures);

    while (1){
        int length1 = fread(length, 1, 2, fileSignatures);

        if (length1 ==0)
            break;

        int lengthTmp;

        if (header[0]==1)
            lengthTmp = length[1] | length[0] << 8;
        else if (header[0]==1)
            lengthTmp = length[0] | length[1] << 8;

        virus *virusTmp = (virus*) malloc(lengthTmp);
        virusTmp->length = lengthTmp;

        fread(virusTmp->name, 1, virusTmp->length - 2, fileSignatures);

        viresus = list_append(viresus, virusTmp);
    }

    fclose(fileSignatures);

    return viresus;
}
*/

void PrintHex(char *buffer, long length) {
    int i;
    for (i = 0; i < length; ++i) {
        printf("%3X", buffer[i] & 0xff);
    }
    printf("\n");

}

void list_print(link *virus_list){
    link *curr = virus_list;
    while (curr != NULL) {
        printf("name: %s\n", curr->v->name);
        printf("size: %d\n", curr->v->length - 18);
        printf("signature:\n");
        PrintHex(curr->v->signature, virus_list->v->length -18);

        curr = curr->next;
    }
}

link* list_append(link* virus_list, virus* data){
    link* ListTmp = (link*) malloc (data->length+4);
    ListTmp->v = data;
    ListTmp->next = NULL;

    if(virus_list == NULL){
        return ListTmp;
    }

    link* curr = virus_list;
    while (curr->next != NULL){
        curr = curr->next;
    }

    curr->next = ListTmp;
    return virus_list;
}

void list_free(link *virus_list){
    link *curr = virus_list;
    link *next;
    while (curr != NULL){
        next = curr->next;
        free(curr->v);
        curr = next;
    }
}

void detect_virus(char *buffer, link *virus_list, unsigned int size){
    int i;
    link *curr = virus_list;
    unsigned int length;
    while (curr != NULL) {
        length = (unsigned int) (curr->v->length - 18);
        if (length <= size) {
            for (i = 0; i < size - length; ++i) {
                if (memcmp(buffer + i, curr->v->signature, length) == 0) {
                    printf("The starting byte location: %d\n", i);
                    printf("The virus name: %s\n", curr->v->name);
                    printf("The size of the virus signature:%d\n", length);
                    printf("\n");
                    break;
                }
            }
        }
        curr = curr->next;
    }
}


void delete_virus(char *buffer, link *virus_list, unsigned int fileSize, char *fileName) {
    int i;
    link *curr = virus_list;
    unsigned int length;
    while (curr != NULL) {
        length = (unsigned int) (curr->v->length - 18);
        if (length <= fileSize) {
            for (i = 0; i < fileSize - length; ++i) {
                if (memcmp(buffer + i, curr->v->signature, length) == 0) {
                    printf("The starting byte location: %d\n", i);
                    printf("The virus name: %s\n", curr->v->name);
                    printf("The size of the virus signature:%d\n", length);
                    printf("\n");
                    printf("deleting the virus\n");

                    FILE *fileInfected = fopen(fileName, "wb+");

                    if(fileInfected == NULL){
                        perror("Error Occured");
                        printf("Error code: %d\n", errno);
                        exit(4);
                    }

                    fseek(fileInfected, i, SEEK_SET);
                    char data[] = "Eden Keshet";
                    fwrite(data, sizeof(data[0]), sizeof(data)/ sizeof(data[0]), fileInfected);
                    printf("done!\n");

                    break;
                }
            }
        }
        curr = curr->next;
    }
}