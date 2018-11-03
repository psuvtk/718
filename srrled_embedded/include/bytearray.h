#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <stdbool.h>

typedef struct {
    char *data;
    int size;
    int capacity;
} bytearray_t;

void bytearray_init(bytearray_t *buf);
void bytearray_destroy(bytearray_t *buf);

void bytearray_append(bytearray_t *buf, char *pdata, int len);
void bytearray_lremove(bytearray_t *buf, int len);
void bytearray_clear(bytearray_t *buf);

bool bytearray_startswith(bytearray_t *buf, char *pdata, int len);
int bytearray_find(bytearray_t *buf, char *data, int len, int skip);
#endif // BYTEARRAY_H