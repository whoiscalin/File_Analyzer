#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
FILE *open_file(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Utilizing: %s as <input_file>", argv[0]);
        return NULL;
    }
    FILE *f = fopen(argv[1], "rb");
    if(!f) {
        perror("Error when opening the file.\n");
        return NULL;
        
    }
    return f; 

}
unsigned char *file_type(const unsigned char *buff, size_t n) {
    const unsigned char bmp[] = {0x42, 0x4D};
    if(n >= sizeof(bmp) && memcmp(buff, bmp, sizeof(bmp)) == 0) {
        return "BMP";
    }
    return "Unknown";
}
long file_size(FILE *f) {
    if(fseek(f, 0, SEEK_END)) {
        perror("fseek failed.\n");
        fclose(f);
        return -1L;
    }
    long size = ftell(f);
    if(size == -1L) {
        perror("ftell failed.\n");
        fclose(f);
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
        printf("  ");
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

    printf("File type: %s\n\n", file_type(magic, magic_n));
    rewind(f);
    hexdump(f);
    fclose(f);
    return 0;
}
            