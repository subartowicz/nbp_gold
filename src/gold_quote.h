//
// Created by Cisco on 22/10/2025.
//

#ifndef GOLD_QUOTE_H
#define GOLD_QUOTE_H

typedef struct {
    double weight_g;
    double weight_oz;
    double base_per_oz_pln;
    double base_total_pln;
    double buy_pln;   // +5%
    double sell_pln;  // +10%
} gold_quote_t;

int fetch_gold_price_pln_per_gram(double *out_price_g_pln, char *date_buf, int date_buf_sz);
int compute_gold_quote(double weight, const char *unit, gold_quote_t *out); // unit: "g"|"oz"

#endif
