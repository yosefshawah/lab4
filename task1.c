#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    char displayMode;
} state;

typedef struct{
char *name;
void (*fun)(state*);
} fun_desc;

void toggleDebugMode(state* s);
void setFileName(state* s);
void setUnitSize(state* s);
void loadIntoMemory(state* s);
void toggleDisplayMode(state* s);
void memoryDisplay(state* s);
void saveIntoFile(state* s);
void memoryModify(state* s);
void quit(state* s);

int main(int argc, char** argv) {
    fun_desc menu[] = { 
        { "Toggle Debug Mode", toggleDebugMode },
        { "Set File Name", setFileName },
        { "Set Unit Size", setUnitSize },
        { "Load Into Memory", loadIntoMemory },
        { "Toggle Display Mode", toggleDisplayMode },
        { "Memory Display", memoryDisplay },
        { "Save Into File", saveIntoFile },
        { "Memory Modify", memoryModify },
        { "Quit", quit },
        { NULL, NULL }
    };

    int menu_length = sizeof(menu) / sizeof(fun_desc) - 1;
    state* s = malloc(sizeof(state));
    s->debug_mode = 0;
    s->displayMode = 0;
    strcpy(s->file_name, "");
    s->unit_size = 1;
    char input[100];

     while (1) {
        printf("Choose action:\n");
        for (int i = 0; i < menu_length; i++) {
            printf("%d-%s\n", i, menu[i].name);
        }
        printf("Option: ");

        if (fgets(input, 100, stdin) == NULL) {
            break;
        }

        int option;
        sscanf(input, "%d\n", &option);

        if (option < 0 || option > menu_length - 1) {
            printf("Not within bounds\n");
            break;
        }

        printf("Within bounds\n");
        menu[option].fun(s);
        printf("DONE.\n\n");
    }
    
}


void toggleDebugMode(state* s) {
    s->debug_mode ^= 1;
    const char* debug_msg = (s->debug_mode) ? "Debug flag now on\n" : "Debug flag now off\n";
    fprintf(stderr, "%s", debug_msg);
}

void setFileName(state* s){
    printf("Please enter file name:\n");
    fgets(s->file_name, 100, stdin);
    s->file_name[strlen(s->file_name) - 1] = '\0';
    if(s->debug_mode){
        fprintf(stderr, "Debug: file name set to '%s'\n", s->file_name);
    }
}

void setUnitSize(state* s){
    printf("Please enter unit size:\n");
    char buffer[100];
    fgets(buffer, 100, stdin);
    int value;
    sscanf(buffer, "%d\n", &value);

    if( value == 4 || value == 2 || value == 1){
        s->unit_size = value;
        if(s->debug_mode){
            fprintf(stderr, "Debug: unit size set to '%d'\n", s->unit_size);
        }
    }
    
    else {
        fprintf(stderr,"Invalid unit size\n");
    }
}


void toggleDisplayMode(state* s) {
    s->displayMode ^= 1;

    const char* display_msg = (s->displayMode) ? "Display flag now on, hexadecimal representation\n" : "Display flag now off, decimal representation\n";
    fprintf(stderr, "%s", display_msg);
}

void memoryModify(state* s){
    char input[100];
    int value;
    int location;

    fprintf(stderr, "Please enter location(hexadecimal) value(hexadecimal)\n");
    fgets(input, 100, stdin);
    sscanf(input, "%x %x", &location, &value);
    memcpy(s->mem_buf + location, &value, s->unit_size);
}

void quit(state* s){
    free(s);
    exit(EXIT_SUCCESS);
}



void memoryDisplay(state* s){
    static char* hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};//this is from the task it desc.
    static char* dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};

    printf("Please enter address(hexadecimal) length(decimal):\n");
    char input[100];
    fgets(input, 100, stdin);
    int address, length;
    sscanf(input, "%x %d\n", &address, &length);
    void* buffer = (void*)address;
    
    if(address == 0){
        buffer = s->mem_buf;
    }
    
    for(int i = 0; i < length; i++){
        int temp = *((int*)(buffer + i * s->unit_size));
        if(s->displayMode){
            printf(hex_formats[s->unit_size-1], temp);
        } else {
            printf(dec_formats[s->unit_size-1], temp);
        }
    }
}

void saveIntoFile(state* s){
    if(strcmp(s->file_name, "") == 0){
        fprintf(stderr, "File name is empty\n");
        return;
    }

    FILE* file = fopen(s->file_name, "r+");
    if(file == NULL){
        perror("fopen");
        return;
    }

    int target;
    int length;
    int src;

    printf("Please enter address(hexadecimal) target(hexadecimal) length(decimal):\n");
    char input[100];
    fgets(input, 100, stdin);

    sscanf(input, "%x %x %d\n", &src, &target, &length);
    void* buffer = (void*)src;
    
    if(src == 0){
        buffer = s->mem_buf;
    }
    
    fseek(file, target, SEEK_SET);
    fwrite(buffer, s->unit_size, length, file);
    
    fclose(file);
}

void loadIntoMemory(state* s){
    if(strcmp(s->file_name, "") == 0){
        fprintf(stderr, "File name is empty\n");
        return;
    }

    FILE* file = fopen(s->file_name, "r");
    if(file == NULL){
        perror("fopen");
        return;
    }

    printf("Please enter location(hexadecimal) length(decimal):\n");
    char buffer[100];
    fgets(buffer, 100, stdin);
    int location, length;
    sscanf(buffer, "%x %d\n", &location, &length);
    
    if(s->debug_mode){
        fprintf(stderr, "Debug: file_name: %s\n", s->file_name);
        fprintf(stderr, "Debug: location: %x\n", location);
        fprintf(stderr, "Debug: length: %d\n", length);
    }

    fseek(file, location, SEEK_SET);
    int bytesRead = fread(s->mem_buf, s->unit_size, length, file);
    fprintf(stderr, "Loaded %d units into memory\n", bytesRead);
    fclose(file);
}

