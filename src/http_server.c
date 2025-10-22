//
// Created by Cisco on 22/10/2025.
//

#include "http_server.h"
#include "http_client.h"
#include "gold_quote.h"
#include "../third_party/microhttpd/microhttpd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int send_json(struct MHD_Connection *c, const char *json, unsigned int status) {
    struct MHD_Response *resp = MHD_create_response_from_buffer(strlen(json),
                                (void*)json, MHD_RESPMEM_MUST_COPY);
    if (!resp) return MHD_NO;
    MHD_add_response_header(resp, "Content-Type", "application/json; charset=utf-8");
    int ret = MHD_queue_response(c, status, resp);
    MHD_destroy_response(resp);
    return ret;
}

static int send_404(struct MHD_Connection *c, const char *msg) {
    char buf[256];
    snprintf(buf, sizeof(buf), "{\"error\":\"%s\"}", msg);
    return send_json(c, buf, MHD_HTTP_NOT_FOUND);
}

static int handler(void *cls, struct MHD_Connection *connection,
                   const char *url, const char *method, const char *ver,
                   const char *upload_data, size_t *upload_data_size, void **con_cls) {
    (void)cls; (void)ver; (void)upload_data; (void)upload_data_size; (void)con_cls;
    if (strcmp(method, "GET") != 0) {
        return send_json(connection, "{\"error\":\"Only GET\"}", MHD_HTTP_METHOD_NOT_ALLOWED);
    }

    if (strncmp(url, "/api/gold-quote", 15) == 0) {
        const char *w = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "weight");
        const char *unit = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "unit");
        if (!w || !*w) return send_json(connection, "{\"error\":\"weight required\"}", MHD_HTTP_BAD_REQUEST);
        double weight = atof(w);
        if (weight <= 0) return send_json(connection, "{\"error\":\"weight must be > 0\"}", MHD_HTTP_BAD_REQUEST);
        if (!unit || !*unit) unit = "g";
        gold_quote_t q;
        if (compute_gold_quote(weight, unit, &q) != 0) return send_json(connection, "{\"error\":\"nbp fetch failed\"}", 502);

        char out[512];
        snprintf(out, sizeof(out),
            "{\"weightG\":%.6f,\"weightOz\":%.6f,"
            "\"basePerOzPln\":%.2f,\"baseTotalPln\":%.2f,"
            "\"buyPln\":%.2f,\"sellPln\":%.2f}",
            q.weight_g, q.weight_oz, q.base_per_oz_pln, q.base_total_pln, q.buy_pln, q.sell_pln);
        return send_json(connection, out, 200);
    }

    if (strncmp(url, "/api/rates", 10) == 0) {
        const char *table = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "table");
        if (!table || !*table) table = "A";
        char urlbuf[256];
        snprintf(urlbuf, sizeof(urlbuf), "https://api.nbp.pl/api/exchangerates/tables/%s/?format=json", table);
        http_response_t resp;
        if (http_get_json(urlbuf, &resp) != 0) return send_404(connection, "nbp table not found");
        // Możemy zwrócić surowy JSON z NBP
        int ret = send_json(connection, resp.data, 200);
        http_response_free(&resp);
        return ret;
    }

    return send_404(connection, "not found");
}

int start_http_server(unsigned short port) {
    struct MHD_Daemon *d = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, port, NULL, NULL, &handler, NULL, MHD_OPTION_END);
    if (!d) return -1;
    printf("HTTP server running on :%u\n", port);
    printf("Endpoints:\n");
    printf("  /api/gold-quote?weight=10&unit=g\n");
    printf("  /api/gold-quote?weight=1&unit=oz\n");
    printf("  /api/rates?table=A\n");
    getchar(); // prosty blok; zatrzymasz Ctrl+C w terminalu lub Enter (w CLion konsoli)
    MHD_stop_daemon(d);
    return 0;
}
