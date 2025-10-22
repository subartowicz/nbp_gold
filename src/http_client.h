//
// Created by Cisco on 22/10/2025.
//

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stddef.h>

typedef struct {
    char *data;
    size_t len;
} http_response_t;

int http_get_json(const char *url, http_response_t *out); // zwraca 0 OK
void http_response_free(http_response_t *resp);

#endif
