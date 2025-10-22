//
// Created by Cisco on 22/10/2025.
//

#include "http_client.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total = size * nmemb;
    http_response_t *resp = (http_response_t*)userdata;
    char *newp = realloc(resp->data, resp->len + total + 1);
    if (!newp) return 0;
    resp->data = newp;
    memcpy(resp->data + resp->len, ptr, total);
    resp->len += total;
    resp->data[resp->len] = '\0';
    return total;
}

int http_get_json(const char *url, http_response_t *out) {
    memset(out, 0, sizeof(*out));
    CURL *curl = curl_easy_init();
    if (!curl) return -1;
    struct curl_slist *hdrs = NULL;
    hdrs = curl_slist_append(hdrs, "Accept: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "nbp-gold/1.0");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    // Bezpiecznie: tylko HTTPS (NBP tak wymaga od 01.08.2025)
    // curl domyślnie weryfikuje cert, zostawiamy włączone.

    CURLcode res = curl_easy_perform(curl);
    long code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

    curl_slist_free_all(hdrs);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK || code < 200 || code >= 300) {
        free(out->data);
        memset(out, 0, sizeof(*out));
        return -2;
    }
    return 0;
}

void http_response_free(http_response_t *resp) {
    if (resp && resp->data) free(resp->data);
    if (resp) memset(resp, 0, sizeof(*resp));
}
