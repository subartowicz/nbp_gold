//
// Created by Cisco on 22/10/2025.
//

#include "json_utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../third_party/cjson/cJSON.h"

int parse_gold_price(const char *json, gold_price_t *out) {
    memset(out, 0, sizeof(*out));
    cJSON *root = cJSON_Parse(json);
    if (!root) return -1;
    // API zwraca tablicę obiektów; weźmy ostatni/pojedynczy element [0]
    cJSON *item = cJSON_GetArrayItem(root, 0);
    if (!item) { cJSON_Delete(root); return -2; }
    cJSON *date = cJSON_GetObjectItemCaseSensitive(item, "data");
    if (!date) date = cJSON_GetObjectItemCaseSensitive(item, "Data");
    cJSON *price = cJSON_GetObjectItemCaseSensitive(item, "cena");
    if (!price) price = cJSON_GetObjectItemCaseSensitive(item, "Cena");
    if (!cJSON_IsString(date) || !cJSON_IsNumber(price)) { cJSON_Delete(root); return -3; }

    strncpy(out->effective_date, date->valuestring, sizeof(out->effective_date)-1);
    out->price_per_gram_pln = price->valuedouble;

    cJSON_Delete(root);
    return 0;
}

// Prosty skrót: wybierz kilka walut (USD, EUR, GBP, CHF)
int summarize_table_a(const char *json, char **short_view) {
    *short_view = NULL;
    cJSON *root = cJSON_Parse(json);
    if (!root) return -1;
    if (!cJSON_IsArray(root)) { cJSON_Delete(root); return -2; }
    cJSON *table = cJSON_GetArrayItem(root, 0);
    if (!table) { cJSON_Delete(root); return -3; }

    const char *date = cJSON_GetObjectItemCaseSensitive(table, "effectiveDate")->valuestring;
    cJSON *rates = cJSON_GetObjectItemCaseSensitive(table, "rates");
    if (!cJSON_IsArray(rates)) { cJSON_Delete(root); return -4; }

    const char *wanted[] = {"USD","EUR","GBP","CHF"};
    double vals[4] = {0};
    int found = 0;

    int cnt = cJSON_GetArraySize(rates);
    for (int i=0; i<cnt; ++i) {
        cJSON *r = cJSON_GetArrayItem(rates, i);
        const char *code = cJSON_GetObjectItemCaseSensitive(r, "code")->valuestring;
        double mid = cJSON_GetObjectItemCaseSensitive(r, "mid")->valuedouble;
        for (int j=0; j<4; ++j) if (strcmp(code, wanted[j])==0) { vals[j]=mid; found++; }
        if (found==4) break;
    }
    char buf[512];
    snprintf(buf, sizeof(buf),
        "Tabela A [%s]: USD=%.4f, EUR=%.4f, GBP=%.4f, CHF=%.4f",
        date, vals[0], vals[1], vals[2], vals[3]);

    *short_view = strdup(buf);
    cJSON_Delete(root);
    return (*short_view)?0:-5;
}
