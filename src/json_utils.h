//
// Created by Cisco on 22/10/2025.
//

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <stdbool.h>

typedef struct {
    double price_per_gram_pln; // NBP "Cena" (1g)
    char effective_date[16];   // YYYY-MM-DD
} gold_price_t;

int parse_gold_price(const char *json, gold_price_t *out); // 0 OK

// Zwraca wskaźnik do JSON z tabelą – do dalszego użycia / echo.
// (opcjonalny skrót do podglądu)
int summarize_table_a(const char *json, char **short_view); // 0 OK

#endif
