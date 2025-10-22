#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_client.h"
#include "json_utils.h"
#include "gold_quote.h"
#include "http_server.h"

static void menu_print(void) {
    puts("\n=== NBP Gold & FX ===");
    puts("1) Pobierz i wypisz skrót tabeli A");
    puts("2) Policz wycenę złota dla wagi");
    puts("3) Uruchom serwer HTTP (API)");
    puts("0) Wyjście");
    printf("Wybór: ");
}

int main(void) {
    int choice = -1;
    while (1) {
        menu_print();
        if (scanf("%d", &choice) != 1) { puts("Błędny wybór."); return 1; }
        if (choice == 0) break;

        switch (choice) {
            case 1: {
                http_response_t r;
                if (http_get_json("https://api.nbp.pl/api/exchangerates/tables/A/?format=json", &r) != 0) {
                    puts("Błąd pobierania tabeli A.");
                    break;
                }
                char *view = NULL;
                if (summarize_table_a(r.data, &view) == 0) {
                    puts(view);
                    free(view);
                } else {
                    puts("Błąd parsowania JSON.");
                }
                http_response_free(&r);
            } break;
            case 2: {
                double weight; char unit[4] = "g";
                printf("Podaj wagę: ");
                if (scanf("%lf", &weight) != 1 || weight <= 0) { puts("Błędna waga."); break; }
                printf("Jednostka (g/oz) [g]: ");
                scanf("%3s", unit);
                gold_quote_t q;
                if (compute_gold_quote(weight, unit, &q) != 0) { puts("Błąd pobrania ceny z NBP."); break; }
                printf("Waga: %.4f g (%.6f oz)\n", q.weight_g, q.weight_oz);
                printf("Cena bazowa 1 oz: %.2f PLN\n", q.base_per_oz_pln);
                printf("Wartość bazowa: %.2f PLN\n", q.base_total_pln);
                printf("KUPNO (+5%%): %.2f PLN\n", q.buy_pln);
                printf("SPRZEDAŻ (+10%%): %.2f PLN\n", q.sell_pln);
            } break;
            case 3: {
                unsigned short port;
                printf("Port [8080]: ");
                if (scanf("%hu", &port) != 1 || port == 0) port = 8080;
                if (start_http_server(port) != 0) {
                    puts("Nie udało się uruchomić serwera.");
                }
            } break;
            default:
                puts("Nieznana opcja.");
        }
    }
    return 0;
}
