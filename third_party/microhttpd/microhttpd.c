#include "microhttpd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* To tylko STUB — nie realizuje prawdziwego HTTP.
 * Pozwala skompilować i uruchomić aplikację „na sucho”.
 */

struct MHD_Response* MHD_create_response_from_buffer(size_t length,
                                                     void *data,
                                                     enum MHD_ResponseMemoryMode mode) {
  struct MHD_Response *r = malloc(sizeof(struct MHD_Response));
  if (!r) return NULL;
  r->length = length;
  r->must_copy = (mode == MHD_RESPMEM_MUST_COPY);
  if (r->must_copy) {
    char *copy = malloc(length + 1);
    if (!copy) { free(r); return NULL; }
    memcpy(copy, data, length);
    copy[length] = '\0';
    r->data = copy;
  } else {
    r->data = (const char*)data;
  }
  return r;
}

int MHD_add_response_header(struct MHD_Response *resp,
                            const char *header,
                            const char *value) {
  (void)resp; (void)header; (void)value;
  /* W stubie nie przechowujemy nagłówków. */
  return MHD_YES;
}

int MHD_queue_response(struct MHD_Connection *conn,
                       unsigned int status_code,
                       struct MHD_Response *resp) {
  (void)conn;
  printf("[MHD stub] HTTP %u\n%.*s\n", status_code, (int)resp->length, resp->data);
  return MHD_YES;
}

void MHD_destroy_response(struct MHD_Response *resp) {
  if (!resp) return;
  if (resp->must_copy && resp->data) free((void*)resp->data);
  free(resp);
}

struct MHD_Daemon* MHD_start_daemon(unsigned int flags,
                                    unsigned short port,
                                    void *accept_policy_cb,
                                    void *accept_policy_cls,
                                    MHD_AccessHandlerCallback handler_cb,
                                    void *handler_cls,
                                    int options, ...) {
  (void)flags; (void)port; (void)accept_policy_cb; (void)accept_policy_cls;
  (void)handler_cb; (void)handler_cls; (void)options;
  /* Tu NIE uruchamiamy prawdziwego serwera — tylko sygnalizujemy start. */
  printf("[MHD stub] Pretend server running on port %hu\n", port);
  struct MHD_Daemon *d = malloc(sizeof(struct MHD_Daemon));
  return d;
}

void MHD_stop_daemon(struct MHD_Daemon *daemon) {
  printf("[MHD stub] Server stopped\n");
  free(daemon);
}

const char* MHD_lookup_connection_value(struct MHD_Connection *connection,
                                        enum MHD_ValueKind kind,
                                        const char *key) {
  (void)connection; (void)kind; (void)key;
  /* W stubie nie parsujemy realnych parametrów — zwracamy NULL,
     dzięki czemu Twój handler wykona ścieżki błędów (400 itp.). */
  return NULL;
}
