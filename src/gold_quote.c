//
// Created by Cisco on 22/10/2025.
//

#include "gold_quote.h"
#include "http_client.h"
#include "json_utils.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define NBP_GOLD_URL "https://api.nbp.pl/api/cenyzlota/?format=json"
#define TROY_OUNCE_G 31.1034768

int fetch_gold_price_pln_per_gram(double *out_price_g_pln, char *date_buf, int date_buf_sz) {
    http_response_t resp;
    int rc = http_get_json(NBP_GOLD_URL, &resp);
    if (rc != 0) return -1;
    gold_price_t gp;
    rc = parse_gold_price(resp.data, &gp);
    http_response_free(&resp);
    if (rc != 0) return -2;
    if (out_price_g_pln) *out_price_g_pln = gp.price_per_gram_pln;
    if (date_buf && date_buf_sz>0) snprintf(date_buf, date_buf_sz, "%s", gp.effective_date);
    return 0;
}

int compute_gold_quote(double weight, const char *unit, gold_quote_t *out) {
    memset(out, 0, sizeof(*out));
    double price_g = 0.0;
    char date[16];
    if (fetch_gold_price_pln_per_gram(&price_g, date, sizeof(date)) != 0) return -1;

    double weight_g = 0.0;
    if (unit && strcmp(unit, "oz")==0) {
        weight_g = weight * TROY_OUNCE_G;
    } else {
        weight_g = weight; // g
    }
    double price_per_oz = price_g * TROY_OUNCE_G;
    double base_total = price_g * weight_g;

    // Marże (jeśli chcesz odwrotnie – zamień mnożniki poniżej)
    double buy  = base_total * 1.05; // KUPNO: +5%
    double sell = base_total * 1.10; // SPRZEDAŻ: +10%

    out->weight_g = weight_g;
    out->weight_oz = weight_g / TROY_OUNCE_G;
    out->base_per_oz_pln = price_per_oz;
    out->base_total_pln = base_total;
    out->buy_pln  = buy;
    out->sell_pln = sell;
    return 0;
}
