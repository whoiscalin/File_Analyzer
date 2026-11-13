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
const char *file_type(const unsigned char *buff, size_t n) {
    const unsigned char bmp[] = {0x42, 0x4D};
    const unsigned char png[] = {[0]=0x89, [1]=0x50, [2]=0x4e, [3]=0x47};
    const unsigned char jpg[] = {[0]=0xff, [1]=0xd8, [2]=0xff, [3]=0xe0};
    const unsigned char elf[] = {[0]=0x7f, [1]=0x45, [2]=0x4c, [3]=0x46};
    const unsigned char szip[] = {[0]=0x50, [1]=0x4b, [2]=0x03, [3]=0x04};
    const unsigned char ezip[] = {[0]=0x50, [1]=0x4b, [2]=0x05, [3]=0x06};
    const unsigned char mzip[] = {[0]=0x50, [1]=0x4b, [2]=0x07, [3]=0x08};
    const unsigned char pdf[] = {[0]=0x25, [1]=0x50, [2]=0x44, [3]=0x46};
    if(n >= sizeof(bmp) && memcmp(buff, bmp, sizeof(bmp)) == 0) {
        return "BMP";
    }
    if( n >= sizeof(png) && memcmp(buff, png,  sizeof(png))==0) {
        return "PNG";
    }
    if( n >= sizeof(jpg) && memcmp(buff, jpg,  sizeof(jpg))==0) {
        return "JPG";
    }
    if( n >= sizeof(elf) && memcmp(buff, elf,  sizeof(elf))==0) {
        return "ELF";
    }
    if( n >= sizeof(szip) && memcmp(buff, szip,  sizeof(szip))==0) {
        return "Standard ZIP archive";
    }
    if( n >= sizeof(ezip) && memcmp(buff, ezip,  sizeof(ezip))==0) {
        return "Empty ZIP archive";
    }
    if( n >= sizeof(mzip) && memcmp(buff, mzip,  sizeof(mzip))==0) {
        return "Spanned ZIP ";
    }
    if( n >= sizeof(pdf) && memcmp(buff, pdf,  sizeof(pdf))==0) {
        return "PDF ";
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
int extension_index(char *argv[1]) {
    int index;
    for(int i = 0; i < strlen(argv[1]); i++) {
        if(argv[1][i]=='.') {
            return i+1;
        }

    }
    return 1;
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
    char extension[3];
    for(int i = 0; i < 3; i++) {
        extension[i] = argv[1][i+extension_index(argv)];
    }
    if(strcmp(extension, file_type(magic, magic_n))) {
        perror("Extension does not match with file type.\n");
        
    } else {
    printf("File type: %s\n\n", file_type(magic, magic_n));
    }
    rewind(f);
    hexdump(f);
    
    fclose(f);

    return 0;
}
            