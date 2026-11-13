#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
FILE *open_file(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Utilizing: %s as <input_file>\n", argv[0]);
        return NULL;
    }
    FILE *f = fopen(argv[1], "rb");
    if(!f) {
        fprintf(stderr, "Error when opening the file.");
        return NULL;
        
    }
    return f; 

}
typedef struct { 
    const unsigned char magic[8];
    size_t length;
    const char *name;
} FileSignature;

const FileSignature signatures[] = {
    {{0x42, 0x4D}, 2, "BMP"},
    {{0x89, 0x50, 0x4E, 0x47}, 4, "PNG"},
    {{0xFF, 0xD8, 0xFF, 0xE0}, 4, "JPG"}, 
    {{0x7F, 0x45, 0x4C, 0x46}, 4, "ELF"},
    {{0x50, 0x4B, 0x03, 0x04}, 4, "ZIP"}, // Standard ZIP
    {{0x50, 0x4B, 0x05, 0x06}, 4, "ZIP"}, // Empty ZIP
    {{0x50, 0x4B, 0x07, 0x08}, 4, "ZIP"}, // Spanned ZIP
    {{0x25, 0x50, 0x44, 0x46}, 4, "PDF"}
};
const size_t num_signatures = sizeof(signatures) / sizeof(signatures[0]);

const char *file_type(const unsigned char *buff, size_t n) {
    for(size_t i = 0; i < num_signatures; i++) {
        if( n >= signatures[i].length && memcmp(buff, signatures[i].magic, signatures[i].length) == 0) {
            return signatures[i].name;
        }
    return "Unknown";
    }
}
long file_size(FILE *f) {
    if(fseek(f, 0, SEEK_END)) {
        perror( "fseek failed.");
        //fclose(f);
        return -1L;
    }
    long size = ftell(f);
    if(size == -1L) {
        perror("ftell failed.");
        //fclose(f);
        return -1L;
    }
    rewind(f);
    return size;
}
void hexdump(FILE *f) {
    unsigned char buffer[16];
    unsigned long long int offset = 0;
    size_t n;
    while((n = fread(buffer, 1, 16, f)) > 0) {
        printf("%08llx: ", offset);
        for(size_t j = 0; j < 16; j++) {
            
            if(j < n) {
                printf("%02x", buffer[j]);
            } else {
                printf("  ");
            }
            if((j & 1) == 1) {
                printf(" ");
            }
        }
        printf(" ");
        for(size_t i = 0; i < n; i++) {
            if(isprint(buffer[i])) {
                printf("%c", buffer[i]);
            } else {
                printf(".");
            }
        }
        printf("\n");
        offset += n;
    }
    printf("\n");
    
}
const char *get_extension(const char *filename) {
    const char *base = strrchr(filename, '/');
    base = base ? base + 1 : filename;
    const char *dot = strrchr(base, '.');
    if (!dot || dot == base) {   
        return "";
    }
    return dot + 1;
}
int compare_insensitive(const char *s1, const char *s2) {
    while(*s1 && *s2) {
        if(toupper((unsigned char)*s1) != toupper((unsigned char)*s2)) {
            return 1;
        }
        s1++;
        s2++;
    }
    return *s1 != *s2;
}
int main(int argc, char *argv[]) {
   
    FILE *f=open_file(argc, argv);
    if(!f) {
        return 1;
    }
    long size = file_size(f);
    if(size < 0) {
        fclose(f);
        return 1;
    }
    printf("File is %ld bytes.\n", size);
    unsigned char magic[8];
    size_t magic_n = fread(magic, 1, sizeof(magic), f);
    const char *detected_type = file_type(magic, magic_n);
    const char *extension = get_extension(argv[1]);

    printf("File type: %s\n", detected_type);
    if(strcmp(detected_type, "Unknown") != 0 && compare_insensitive(extension, detected_type) != 0) {
        fprintf(stderr, "Warning: Extension '.%s' does not match detected file type '%s'.\n", extension, detected_type);
    }
    printf("\n");
    rewind(f);
    hexdump(f);
    
    fclose(f);

    return 0;
}
            