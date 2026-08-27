#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int main(void) {
    int x = 20, y = 26;
    int result = add(x, y);
    printf("add(%d, %d) = %d\n", x, y, result);
    return 0;
}