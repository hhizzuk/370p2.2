
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXSIZE 300
#define MAXLINELENGTH 1000
#define MAXFILES 6

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry {
    char label[7];
    char location;
    int offset;
};

struct RelocationTableEntry {
    int offset;
    char inst[7];
    char label[7];
    int file;
};

struct FileData {
    int textSize;
    int dataSize;
    int symbolTableSize;
    int relocationTableSize;
    int textStartingLine;
    int dataStartingLine;
    int text[MAXSIZE];
    int data[MAXSIZE];
    SymbolTableEntry symbolTable[MAXSIZE];
    RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
    int text[MAXSIZE];
    int data[MAXSIZE];
    SymbolTableEntry symTable[MAXSIZE];
    RelocationTableEntry relocTable[MAXSIZE];
    int textSize;
    int dataSize;
    int symTableSize;
    int relocTableSize;
};

// Function prototypes
void fixLocal(FileData *files, int);
int findOrigOffset(int);
int findSizes(struct FileData files[MAXFILES], int, int);
int isUpper(char *);
int findGlobals(FileData files[MAXFILES], int, int, char *);
void assignStarts(FileData files[MAXFILES], int);
int convert(FileData files[MAXFILES], int, int, int);
int parseHex(char *line);
int handleBeqOffset(FileData *files, int currentFile, int currentOffset, char *targetLabel);

int totalSize = 0;

int main(int argc, char *argv[]) {
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    int i, j;
    
    if (argc <= 2) {
        printf("error: usage: %s <obj file> ... <output-exe-file>\n", argv[0]);
        exit(1);
    }
    
    outFileString = argv[argc - 1];
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }
    
    FileData files[MAXFILES];
    int textcount = 0;

    // Read all input files
    for (i = 0; i < argc - 2; i++) {
        inFileString = argv[i+1];
        inFilePtr = fopen(inFileString, "r");
        
        if (inFilePtr == NULL) {
            printf("error in opening %s\n", inFileString);
            exit(1);
        }
        
        char line[MAXLINELENGTH];
        int sizeText, sizeData, sizeSymbol, sizeReloc;
        
        // Parse header
        fgets(line, MAXSIZE, inFilePtr);
        sscanf(line, "%d %d %d %d", &sizeText, &sizeData, &sizeSymbol, &sizeReloc);
        
        files[i].textSize = sizeText;
        files[i].dataSize = sizeData;
        files[i].symbolTableSize = sizeSymbol;
        files[i].relocationTableSize = sizeReloc;
        files[i].textStartingLine = textcount;

        totalSize += files[i].dataSize + files[i].textSize;
        
        // Read text section
        for (j = 0; j < sizeText; j++) {
            fgets(line, MAXLINELENGTH, inFilePtr);
            line[strcspn(line, "\n")] = 0;
            if (strncmp(line, "0x", 2) == 0) {
                files[i].text[j] = parseHex(line);
            } else {
                files[i].text[j] = atoi(line);
            }
            ++textcount;
        }
        
        // Read data section
        for (j = 0; j < sizeData; j++) {
            fgets(line, MAXLINELENGTH, inFilePtr);
            line[strcspn(line, "\n")] = 0;
            if (strncmp(line, "0x", 2) == 0) {
                files[i].data[j] = parseHex(line);
            } else {
                files[i].data[j] = atoi(line);
            }
        }
        
        // Read symbol table
        for (j = 0; j < sizeSymbol; j++) {
            fgets(line, MAXLINELENGTH, inFilePtr);
            char label[7];
            char type;
            int addr;
            sscanf(line, "%s %c %d", label, &type, &addr);
            files[i].symbolTable[j].offset = addr;
            strcpy(files[i].symbolTable[j].label, label);
            files[i].symbolTable[j].location = type;
            
            if (!strcmp("Stack", label) && type != 'U') {
                printf("error: Stack label defined in input file\n");
                exit(1);
            }
        }
        
        // Read relocation table
        for (j = 0; j < sizeReloc; j++) {
            fgets(line, MAXLINELENGTH, inFilePtr);
            int addr;
            char opcode[7], label[7];
            sscanf(line, "%d %s %s", &addr, opcode, label);
            files[i].relocTable[j].offset = addr;
            strcpy(files[i].relocTable[j].inst, opcode);
            strcpy(files[i].relocTable[j].label, label);
            files[i].relocTable[j].file = i;
        }
        fclose(inFilePtr);
    }

    // Process files
    assignStarts(files, argc - 2);
    
    // Check for duplicate defined global labels
    for (i = 0; i < argc - 2; i++) {
        for (int count = 0; count < files[i].symbolTableSize; count++) {
            if (files[i].symbolTable[count].location == 'U') continue;
            
            for (int j = i + 1; j < argc - 2; j++) {
                for (int k = 0; k < files[j].symbolTableSize; k++) {
                    if ((!strcmp(files[i].symbolTable[count].label, files[j].symbolTable[k].label)) && 
                        files[j].symbolTable[k].location != 'U') {
                        printf("error: duplicate defined label\n");
                        exit(1);
                    }
                }
            }
        }
    }

    // Fix local and global references
    fixLocal(files, argc);

    // Write combined output
    for (i = 0; i < argc - 2; i++) {
        for (j = 0; j < files[i].textSize; j++) {
            fprintf(outFilePtr, "0x%08X\n", files[i].text[j]);
        }
    }
    for (i = 0; i < argc - 2; i++) {
        for (j = 0; j < files[i].dataSize; j++) {
            fprintf(outFilePtr, "0x%08X\n", files[i].data[j]);
        }
    }

    fclose(outFilePtr);
    return 0;
}

int parseHex(char *line) {
    unsigned int value;
    sscanf(line, "0x%x", &value);
    return (int)value;
}

int handleBeqOffset(FileData *files, int currentFile, int currentOffset, char *targetLabel) {
    int targetAddr;
    
    if (isUpper(targetLabel)) {
        // Global label
        targetAddr = findGlobals(files, currentFile, currentFile, targetLabel);
    } else {
        // Local label - need to find in current file's symbol table
        for (int i = 0; i < files[currentFile].symbolTableSize; i++) {
            if (!strcmp(files[currentFile].symbolTable[i].label, targetLabel)) {
                targetAddr = files[currentFile].symbolTable[i].offset;
                if (files[currentFile].symbolTable[i].location == 'T') {
                    targetAddr += files[currentFile].textStartingLine;
                }
                break;
            }
        }
    }
    
    // Calculate relative offset: target - (current + 1)
    int relativeOffset = targetAddr - (currentOffset + files[currentFile].textStartingLine + 1);
    return relativeOffset & 0xFFFF; // Ensure 16-bit offset
}

void fixLocal(FileData *files, int argc) {
    for (int i = 0; i < argc - 2; i++) {
        for (int j = 0; j < files[i].relocationTableSize; j++) {
            int index = files[i].text[files[i].relocTable[j].offset];
            
            if (!strcmp(files[i].relocTable[j].inst, "beq")) {
                // Handle beq instructions
                int newOffset = handleBeqOffset(files, i, files[i].relocTable[j].offset, 
                                              files[i].relocTable[j].label);
                files[i].text[files[i].relocTable[j].offset] = 
                    (files[i].text[files[i].relocTable[j].offset] & 0xFFFF0000) | newOffset;
            }
            else if ((!strcmp(files[i].relocTable[j].inst, "lw")) || 
                     (!strcmp(files[i].relocTable[j].inst, "sw"))) {
                if (isUpper(files[i].relocTable[j].label)) {
                    int temp = findGlobals(files, i, argc - 2, files[i].relocTable[j].label);
                    if (!strcmp(files[i].relocTable[j].label, "Stack")) {
                        files[i].text[files[i].relocTable[j].offset] = 
                            convert(files, argc - 2, totalSize + temp, index);
                    } else {
                        files[i].text[files[i].relocTable[j].offset] = 
                            convert(files, argc - 2, temp, index);
                    }
                } else {
                    int temp = findOrigOffset(index);
                    int size = findSizes(files, i, argc - 2);
                    files[i].text[files[i].relocTable[j].offset] = 
                        convert(files, argc - 2, temp + size, index);
                }
            } 
            else if (!strcmp(files[i].relocTable[j].inst, ".fill")) {
                if (isUpper(files[i].relocTable[j].label)) {
                    if (!strcmp(files[i].relocTable[j].label, "Stack")) {
                        files[i].data[files[i].relocTable[j].offset] = totalSize;
                    } else {
                        int temp = findGlobals(files, i, argc - 2, files[i].relocTable[j].label);
                        // Adjust offset based on section
                        for (int k = 0; k < files[i].symbolTableSize; k++) {
                            if (!strcmp(files[i].symbolTable[k].label, files[i].relocTable[j].label)) {
                                if (files[i].symbolTable[k].location == 'T') {
                                    temp += files[i].textStartingLine;
                                }
                                break;
                            }
                        }
                        files[i].data[files[i].relocTable[j].offset] = temp;
                    }
                } else {
                    // Local .fill handling
                    int temp = files[i].data[files[i].relocTable[j].offset];
                    int sum = files[i].textStartingLine;
                    if (temp >= files[i].textSize) {
                        sum = files[i].dataStartingLine + (temp - files[i].textSize);
                    }
                    files[i].data[files[i].relocTable[j].offset] = sum;
                }
            }
        }
    }
}

int findOrigOffset(int input) {
    return input & 0xFFFF; // Get offset portion (last 16 bits)
}

int findSizes(struct FileData files[MAXFILES], int exception, int argc) {
    int size = 0;
    for (int i = 0; i < argc; i++) {
        if (i != exception) {
            if (exception == 0) {
                size += files[i].textSize;
            } else {
                if (i > exception) {
                    size += files[i].textSize;
                } else {
                    size += files[i].dataSize + files[i].textSize;
                }
            }
        }
    }
    return size;
}

int isUpper(char *input) {
    return (input[0] >= 'A' && input[0] <= 'Z');
}

int findGlobals(FileData files[MAXFILES], int exception, int size, char *input) {
    int temp = -1;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < files[i].symbolTableSize; j++) {
            if (!strcmp(files[i].symbolTable[j].label, input)) {
                if (files[i].symbolTable[j].location == 'T') {
                    temp = files[i].symbolTable[j].offset;
                } else if (files[i].symbolTable[j].location == 'D') {
                    temp = files[i].symbolTable[j].offset + files[i].dataStartingLine;
                } else if (files[i].symbolTable[j].location == 'U' && !strcmp("Stack", input)) {
                    temp = 0;
                }
            }
        }
    }
    
    if (temp == -1 && strcmp("Stack", input)) {
        printf("error: undefined global label\n");
        exit(1);
    }
    if (temp == -1 && !strcmp("Stack", input)) {
        temp = 0;
    }
    return temp;
}

void assignStarts(FileData files[MAXFILES], int size) {
    int sum = 0;
    // Calculate text segment starting positions
    for (int i = 0; i < size; i++) {
        files[i].textStartingLine = sum;
        sum += files[i].textSize;
    }
    
    // Calculate data segment starting positions
    for (int i = 0; i < size; i++) {
        files[i].dataStartingLine = sum;
        sum += files[i].dataSize;
    }
}

int convert(FileData files[MAXFILES], int size, int offset, int original) {
    int opAndRegs = original & 0xFFFF0000; // Preserve opcode and registers
    return opAndRegs | (offset & 0xFFFF);  // Combine with new offset
}