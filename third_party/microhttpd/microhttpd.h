#ifndef MICROHTTPD_H
#define MICROHTTPD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* Stałe zwrotne */
#define MHD_YES 1
#define MHD_NO  0

/* Kody HTTP używane w Twoim projekcie */
#define MHD_HTTP_OK                   200
#define MHD_HTTP_BAD_REQUEST          400
#define MHD_HTTP_METHOD_NOT_ALLOWED   405
#define MHD_HTTP_NOT_FOUND            404

/* Tryb zarządzania pamięcią odpowiedzi (zgodne z libmicrohttpd) */
enum MHD_ResponseMemoryMode {
  MHD_RESPMEM_PERSISTENT = 0,
  MHD_RESPMEM_MUST_COPY  = 1,
  /* (inne tryby pomijamy) */
};

/* Rodzaj wartości (GET/POST). W projekcie używasz GET */
enum MHD_ValueKind {
  MHD_GET_ARGUMENT_KIND = 0
};

typedef struct MHD_Connection MHD_Connection;
typedef struct MHD_Response   MHD_Response;
typedef struct MHD_Daemon     MHD_Daemon;

/* Callback handlera (zgodne z libmicrohttpd) */
typedef int (*MHD_AccessHandlerCallback)(
    void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **ptr);

/* Proste, minimalne struktury — do kompilacji (to jest stub) */
struct MHD_Response {
  const char *data;
  size_t length;
  int must_copy;
};

struct MHD_Connection { int dummy; };
struct MHD_Daemon     { int dummy; };

/* Flagi/opcje serwera (symbolicznie) */
#define MHD_USE_SELECT_INTERNALLY 1
#define MHD_OPTION_END 0

/* API używane w Twoim projekcie */
struct MHD_Response* MHD_create_response_from_buffer(size_t length,
                                                     void *data,
                                                     enum MHD_ResponseMemoryMode mode);

int  MHD_add_response_header(struct MHD_Response *resp,
                             const char *header,
                             const char *value);

int  MHD_queue_response(struct MHD_Connection *conn,
                        unsigned int status_code,
                        struct MHD_Response *resp);

void MHD_destroy_response(struct MHD_Response *resp);

/* Start/stop „serwera” (stub) */
struct MHD_Daemon* MHD_start_daemon(unsigned int flags,
                                    unsigned short port,
                                    void *accept_policy_cb,
                                    void *accept_policy_cls,
                                    MHD_AccessHandlerCallback handler_cb,
                                    void *handler_cls,
                                    int options, ...);

void MHD_stop_daemon(struct MHD_Daemon *daemon);

/* Pobieranie wartości z query (GET). W stubie zwracamy zawsze NULL. */
const char* MHD_lookup_connection_value(struct MHD_Connection *connection,
                                        enum MHD_ValueKind kind,
                                        const char *key);

#ifdef __cplusplus
}
#endif

#endif /* MICROHTTPD_H */
