#include <stdio.h>
void parse(char *raw);

int main() {
    char raw_data[10];
    printf("Enter data: ");
    scanf("%c", raw_data);
    parse(raw_data);
    return 0;
}

void parse(char *raw) {
    for (int i = 0; i < sizeof(raw) / sizeof(raw[0]); i++) {
        printf("%c\n", raw[i]);
    }
}