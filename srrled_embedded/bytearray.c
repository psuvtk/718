#include "./include/bytearray.h"

void bytearray_init(bytearray_t *buf) {
    buf->data = NULL;
    buf->size = 0;
    buf->capacity = 0;
}


void bytearray_destroy(bytearray_t *buf) {
    if (buf->data != NULL)
        free(buf->data);
    buf->size = 0;
    buf->capacity = 0;
}


void bytearray_append(bytearray_t *buf, char *pdata, int len) {
    if (buf->capacity == 0) {
        buf->data = (char *)malloc(DEFAULT_BUF_LEN * sizeof(char));
        memcpy(buf->data, pdata, len);
        buf->size = len;
        buf->capacity = DEFAULT_BUF_LEN;
        return;
    }

    if (buf->capacity - buf->size >= len) {
        // 足以容纳
        memcpy(&(buf->data)[buf->size], pdata, len);
        buf->size += len;
        return;
    } else {
        // 不足以容纳
        int new_capacity = max(
            buf->capacity * 2,
            ((buf->capacity+len)/DEFAULT_BUF_LEN+1)*DEFAULT_BUF_LEN
        );
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
    memcpy(buf->data, &(buf->data)[len], len);
    buf->size -= len;
}


void bytearray_clear(bytearray_t *buf) {
    buf->size = 0;
}


bool bytearray_startswith(const bytearray_t *buf, const char *pdata, int len) {
    if (buf->size < len)
        return false;
    return memcmp(buf->data, pdata, len) == 0;
}


int bytearray_find(const bytearray_t *buf, const char *pdata, int len, int skip) {
    if (buf->size < len+skip) return -1;
    // FIXME: 是否有高效匹配的实现？
    for (int i = skip; i < buf->size-len; i++) {
        if (memcpy(&(buf->data)[i], pdata, len) == 0)
            return i;
    }
    return -1;
}