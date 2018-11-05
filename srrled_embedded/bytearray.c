#include "./include/bytearray.h"

void bytearray_init(bytearray_t *buf) {
    printf("Func::bytearray_init\n");
    buf->data = NULL;
    buf->size = 0;
    buf->capacity = 0;
}


void bytearray_destroy(bytearray_t *buf) {
    printf("Func::bytearray_destroy\n");
    if (buf->data != NULL)
        free(buf->data);
    buf->size = 0;
    buf->capacity = 0;
}


void bytearray_append(bytearray_t *buf, char *pdata, int len) {
    printf("Func::bytearray_append\n");
    if (buf->capacity == 0 && buf->data == NULL) {
        printf("malloc new buf\n");
        buf->data = (char *)malloc(DEFAULT_BUF_LEN * sizeof(char));
        memcpy(buf->data, pdata, len);
        buf->size = len;
        buf->capacity = DEFAULT_BUF_LEN;
        return;
    }

    if (buf->capacity - buf->size >= len) {
        // 足以容纳
        printf("memory enough\n");
        memcpy(&(buf->data)[buf->size], pdata, len);
        buf->size += len;
        return;
    } else {
        // 不足以容纳
        printf("memory not enough, malloc new buf\n");
        int new_capacity = buf->capacity * 2;
        char *new_buf = (char *)malloc(new_capacity * sizeof(char));
        memcpy(new_buf, buf->data, buf->size);
        memcpy(&new_buf[buf->size], pdata, len);
        buf->size += len;
        buf->capacity = new_capacity;
        free(buf->data);
        buf->data = new_buf;
    }
}


void bytearray_lremove(bytearray_t *buf, int len) {
    buf->size -= len;
    memcpy(buf->data, &(buf->data)[len], buf->size);

}


void bytearray_clear(bytearray_t *buf) {
    printf("Func::bytearray_clear\n");
    buf->size = 0;
}


bool bytearray_startswith(const bytearray_t *buf, const char *pdata, int len) {
    printf("Func::bytearray_startswith\n");
    if (buf->size < len)
        return false;
    return memcmp(buf->data, pdata, len) == 0;
}


int bytearray_find(const bytearray_t *buf, const char *pdata, int len, int skip) {
    printf("Func::bytearray_find\n");
    if (buf->size < len+skip) return -1;
    // FIXME: 是否有高效匹配的实现？
    for (int i = skip; i < buf->size-len; i++) {
        if (memcmp(&(buf->data)[i], pdata, len) == 0) 
            return i;
    }
    return -1;
}