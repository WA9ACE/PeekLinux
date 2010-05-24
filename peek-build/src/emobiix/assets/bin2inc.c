#include <stdio.h>
#include <stdlib.h>

#define CHAR_PER_LINE 12

int main(void)
{
    int ichar, count = 1, total = 0;
    
    ichar = fgetc(stdin);
    printf("char data[] = {\n\t0x%02x", ichar);
    while (1) {
        ichar = fgetc(stdin);
        if (feof(stdin)) {
            printf("\n};\n");
            printf("const int dataSize = %d;\n", total);
            return (0);
        }
        ++total;
        
        if (count >= CHAR_PER_LINE) {
            printf("\n\t");
            count = 0;
        }
        printf(", 0x%02x", ichar);
        ++count;
    }
}
