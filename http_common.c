////////////////////////////////////////////////////////////////////////////////
//
//  Grid2Home Smart Energy 2
//
////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011-2012 Grid2Home, Inc. All rights reserved.
//
//  This source code and any compilation or derivative thereof is the
//  proprietary information of Grid2Home, Inc and is confidential in nature.
//
//  \file          http_common.c
//
//  \author        F.P. Johnston
//
//  \brief         HTTP functions and data common to both client and server.
//
////////////////////////////////////////////////////////////////////////////////

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if     defined(STDLIB_BUILD)

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#endif

#include <g2h_config.h>
#include <common.h>

#if     !defined(STDLIB_BUILD)

#include <lwip/sockets.h>
#include <psal_assert.h>
#include <psal_types.h>

#define assert(x) PSAL_ASSERT(x)

#endif

#define DLOG_MODULE             DLOG_ID_HTTP
#define THIS_FILE               "http_common.c"

#include <dlog.h>
#include <xmalloc.h>
#include <http_api.h>
#include <http_private.h>

#if     !defined(HTTP_MAX_CHUNK)
#define HTTP_MAX_CHUNK      UINT_MAX
#endif

#define RFC1123FMT          "%a, %d %b %Y %H:%M:%S GMT"

// Must include this here so reference below works.

static bool null_test(void);

// Global data

http_ctl_t http_ctl =
{
#if     defined(STDLIB_BUILD)
    .ni_flags    = NI_NUMERICHOST | NI_NUMERICSERV,
#endif
    .max_chunk   = HTTP_MAX_CHUNK,
    .blocking    = false,
    .ai_family   = DEFAULT_FAMILY,
    .request_cb  = NULL,
    .response_cb = NULL,
    .sockets     = { NULL },
    .max_sd      = -1,
    .read_fds    = { { 0 } },
    .write_fds   = { { 0 } },
    .tv          = { .tv_sec = 1, .tv_usec = 0 },
    .nclients    = 0,
    .seqclient   = 0,
    .nservers    = 0,
    .seqserver   = 0,
    .http_sd     = -1,
    .https_sd    = -1,
    .server_name = NULL,
    .new_cb      = NULL,
    .test_cb     = null_test,
};

// Local data

struct http_methods
{
    http_method_t value;
    const char *string;
};

static const struct http_methods methods[] =
{
    { HTTP_GET,     "GET",     },
    { HTTP_PUT,     "PUT",     },
    { HTTP_POST,    "POST",    },
    { HTTP_DELETE,  "DELETE",  },
    { HTTP_HEAD,    "HEAD",    },
    { HTTP_OPTIONS, "OPTIONS", },
    { HTTP_ECHO,    "ECHO",    },
};

// Local functions

#if     defined(HTTP_EXTRA)
static time_t convert_time(const char *timestr);
#endif

static int next_header(http_conn_t *conn);

static int recv_line(http_conn_t *conn);

//
//  check_length() - Check to see what to do when we receive data.
//
////////////////////////////////////////////////////////////////////////////////

int check_length(http_conn_t *conn)
{
    assert(conn != NULL);

    http_data_t *entity = &conn->entity;

    const char *token = strtok(conn->line, ";");

    if (token == NULL)
    {
        return -HTTP_STATUS_BAD_REQUEST;
    }

    char *endptr;

    errno = 0;

    uint size = (uint)strtoul(token, &endptr, 16);

    if (errno != 0 || !isspace((int)*endptr))
    {
        return -HTTP_STATUS_BAD_REQUEST;
    }

    if (size == 0)
    {
        conn->state = RECV_LAST;
    }
    else
    {
        uint ntotal = entity->size + size;

        if (entity->start == NULL)
        {
            if (!xcalloc(&entity->start, 1, ntotal + 1))
            {
                return -HTTP_STATUS_SERVER_ERROR;
            }
        }
        else
        {
            if (!xrealloc(&entity->start, ntotal + 1))
            {
                return -HTTP_STATUS_SERVER_ERROR;
            }
        }

        entity->size  = ntotal;
        entity->start = entity->start;

        conn->pos     = 0;
        conn->state   = RECV_CHUNK;
    }

    return 0;
}

//
//  check_recv() - Check to see what to do when we receive data.
//
////////////////////////////////////////////////////////////////////////////////

int check_recv(http_conn_t *conn,
    int (*check_first)(http_conn_t *conn),
    int (*check_header)(http_conn_t *conn))
{
    assert(conn != NULL);
    assert(check_first != NULL);
    assert(check_header != NULL);

    int rcode;

    switch (conn->state)
    {
        case RECV_STATUS:
        case RECV_REQUEST:
        case RECV_HEADER:
        case RECV_LENGTH:
        case RECV_CRLF:
        case RECV_LAST:
        {
            rcode = recv_line(conn);

            if (rcode < 0)
            {
                return rcode;
            }
            else if (conn->state == RECV_CRLF || conn->state == RECV_LAST)
            {
                if (rcode != 1)         // Did we get the blank line we wanted?
                {
                    return -HTTP_STATUS_BAD_REQUEST;
                }
            }
            break;
        }

        default:
            break;
    }

    switch (conn->state)
    {
        case RECV_STATUS:
        case RECV_REQUEST:
            return (*check_first)(conn);

        case RECV_HEADER:
            return (*check_header)(conn);

        case RECV_LENGTH:
            return check_length(conn);

        case RECV_CRLF:
            conn->state = RECV_LENGTH;

            break;

        case RECV_LAST:
            conn->state = RECV_DONE;

            break;

        case RECV_CHUNK:
        case RECV_ENTITY:
        {
            assert(conn->entity.start != NULL);

            uint nbytes = conn->entity.size - conn->entity.offset;
            char *addr = conn->entity.start + conn->entity.offset;
            rcode = http_recv(conn, addr, nbytes);

            // If the connection was closed by the server, and we are neither
            // chunked nor using a Content-Length header, then just take the
            // data we received. Otherwise make sure we got what we expected.

            if (rcode == -ECONNRESET)   // Connection closed by peer?
            {
                if (!conn->chunked && !conn->xi.content_length)
                {
                    conn->entity.size = conn->entity.offset;

                    conn->state = (conn->state == RECV_ENTITY) ? RECV_DONE : RECV_CRLF;

                    break;
                }
                else if (rcode < (int)nbytes)
                {
                    dlog(DLOG_ERR, "read %d bytes, expected %d bytes\n", rcode, nbytes);

                    return -ENODATA;
                }
            }
            else if (rcode < 0)
            {
                return rcode;
            }

            if ((conn->entity.offset += (uint)rcode) == conn->entity.size)
            {
                conn->state = (conn->state == RECV_ENTITY) ? RECV_DONE : RECV_CRLF;
            }

            addr[rcode] = '\0';

            dlog(DLOG_INFO, "Entity: %u byte%s\n", rcode, rcode == 1 ? "" : "s");
            dlog(DLOG_DEBUG, "%s\n", addr);

            break;
        }

        default:
            return -HTTP_STATUS_SERVER_ERROR;
    }

    return -HTTP_STATUS_OK;
}

//
//  check_send() - Check to see what to do next for a connection.
//
////////////////////////////////////////////////////////////////////////////////

int check_send(http_conn_t *conn)
{
    assert(conn != NULL);

    http_ctl_t *h       = &http_ctl;
    http_data_t *entity = &conn->entity;
    int rcode;

#if     G2H_HTTPS

    if (conn->https && !tls_data(conn))
    {
        return -EAGAIN;                 // Tell caller to try again later
    }

#endif

    for (;;)
    {
        switch (conn->state)
        {
            case SEND_REQUEST:
            case SEND_STATUS:
                http_send(conn, conn->line, strlen(conn->line)); // TODO: check errors

                conn->state = SEND_HEADER;

                break;

            case SEND_HEADER:
            {
                rcode = next_header(conn);

                if (rcode == -1)
                {
                    return -ERANGE;
                }
                else if (rcode == 0)    // If last header output, change state
                {
                    conn->state = conn->chunked ? SEND_LENGTH : SEND_ENTITY;
                }

                http_send(conn, conn->line, strlen(conn->line)); // TODO: check errors

                break;
            }

            case SEND_ENTITY:
                if (entity->start != NULL)
                {
                    http_send(conn, entity->start, entity->size); // TODO: check errors
                }

                conn->state = SEND_DONE;

                break;

            case SEND_LENGTH:
                if (entity->start == NULL)
                {
                    conn->state = SEND_DONE;
                }
                else
                {
                    uint length = min(h->max_chunk, entity->size - entity->offset);

                    if (!xsnprintf(&rcode, conn->line, sizeof(conn->line),
                                   "%X" CRLF, length))
                    {
                        return -ENOMEM;
                    }

                    uint nbytes = (uint)rcode;

                    http_send(conn, conn->line, nbytes); // TODO: check errors

                    conn->state = (length == 0) ? SEND_LAST : SEND_CHUNK;
                }
                break;

            case SEND_CHUNK:
            {
                uint nbytes = entity->size - entity->offset;
                nbytes = min(h->max_chunk, nbytes);

                http_send(conn, entity->start + entity->offset, nbytes); // TODO: check errors

                entity->offset += nbytes;

                conn->state = SEND_CRLF;

                break;
            }

            case SEND_CRLF:
            case SEND_LAST:
                strcpy(conn->line, CRLF);

                http_send(conn, conn->line, strlen(conn->line)); // TODO: check errors

                conn->state = (conn->state == SEND_CRLF) ? SEND_LENGTH : SEND_DONE;

                break;

            case SEND_DONE:
                xfree(&entity->start);

                return 0;

            default:
                return -EPROTO;
        }
    }
}

//
//  check_sockets() - Check if there is input or output to process for any
//                    client or server connections. This function will work
//                    whether select() can block or not.
//
//  Returns:  >= 0 - No. of active client and server connections. Note that this
//                   will be at least 1 if a client test is in progress.
//
//            <  0 - Error code. It will be -EAGAIN if select() times out (or
//                   has nothing to process in nonblocking mode), unless a
//                   client test is active, in which case it will return 1.
//
////////////////////////////////////////////////////////////////////////////////

int check_sockets(void)
{
    http_ctl_t *h = &http_ctl;
    bool client_test  = (*h->test_cb)();

    if (h->max_sd == -1)                // Nothing initialized yet
    {
        return client_test ? 1 : -EPROTO;
    }

    int sd            = h->max_sd;
    fd_set read_fds   = h->read_fds;
    fd_set write_fds  = h->write_fds;
    struct timeval tv = h->tv;
    int rcode         = select(sd + 1, &read_fds, &write_fds, NULL, &tv);

    if (rcode == 0)
    {
        // Ensure we don't return an error if there is a client test active.

        return client_test ? 1 : -EAGAIN;
    }
    else if (rcode == -1)
    {
        dlog(DLOG_ERR, "select(): %s\n", strerror(errno));

        return -errno;
    }

    uint nbits = (uint)rcode;           // No. of events to process

    if (h->http_sd != -1 && FD_ISSET((uint)h->http_sd, &read_fds))
    {
        assert(h->new_cb != NULL);

        (*h->new_cb)(HTTP);             // New incoming connection
        --nbits;
    }
    else if (h->https_sd != -1 && FD_ISSET((uint)h->https_sd, &read_fds))
    {
        assert(h->new_cb != NULL);

        (*h->new_cb)(HTTPS);            // New incoming connection
        --nbits;
    }

    // Loop through all sockets. The checks for the maximum socket and the
    // number of bits remaining are optimizations to obviate the need to check
    // any more sockets than we need to.

#if     HTTP_RX_TIMEOUT != 0

    uint now = xtime();

#endif

    for (sd = 0; sd <= h->max_sd && nbits != 0; ++sd)
    {
        http_conn_t *conn = h->sockets[sd];

        if (conn == NULL)
        {
            continue;
        }

        int readable  = FD_ISSET((uint)sd, &read_fds)  ? (--nbits, 1) : 0;
        int writeable = FD_ISSET((uint)sd, &write_fds) ? (--nbits, 1) : 0;

#if     HTTP_RX_TIMEOUT != 0

        // Check socket to see if any timeouts occurred

        if (!conn->client && now >= conn->rxtimeout + HTTP_RX_TIMEOUT)
        {
            dlog(DLOG_WARNING, "Timeout for socket %u\n", sd);

            assert(conn->error_cb != NULL);

            (*conn->error_cb)(conn);

            continue;
        }

#endif

        if (readable && h->sockets[sd] != NULL)
        {
            // HTTP connections will process all data received before returning,
            // but HTTPS connections process TLS messages one at a time. This
            // conditional code ensures that we process as many of them as we
            // can now, instead of waiting for another receive event.

#if     G2H_HTTPS

            do

#endif
            {
                assert(conn->input_cb != NULL);

                (*conn->input_cb)(conn);

#if     G2H_HTTPS

            } while (h->sockets[sd] != NULL && conn->https && tls_pending(conn) > 0);

#endif

        }
        
        if (writeable && h->sockets[sd] != NULL)
        {
            FD_CLR((uint)sd, &h->write_fds);
            
            assert(conn->output_cb != NULL);

            (*conn->output_cb)(conn);
        }
    }

    int ntotal = (int)(h->nclients + h->nservers);

    if (ntotal == 0 && client_test)
    {
        return 1;                       // Must have at least 1 if test running
    }

    return ntotal;
}

//
//  close_socket() - Close a socket descriptor, clear the corresponding file
//                   descriptor bits, then calculate the new maximum socket
//                   descriptor.
//
////////////////////////////////////////////////////////////////////////////////

void close_socket(int *psd)
{
    assert(psd != NULL);

    int sd = *psd;                      // Save a local copy

    *psd = -1;                          // Reset socket descriptor

    assert(sd >= 0 && sd < HTTP_SOCKETS);

    http_ctl_t *h = &http_ctl;

    FD_CLR((uint)sd, &h->read_fds);
    FD_CLR((uint)sd, &h->write_fds);

    close(sd);

    if (sd == h->max_sd)
    {
        // Scan from top socket down until we either hit -1, or we find a
        // socket that's marked as readable or writeable.

        while (--h->max_sd >= 0
                && !FD_ISSET((uint)h->max_sd, &h->read_fds)
                && !FD_ISSET((uint)h->max_sd, &h->write_fds))
        {
            ;
        }

        // Note: psd may point to either h->http_sd or h->https_sd, but if
        // so, by the time we get here that socket descriptor will be -1,
        // and the code below will work as expected to reset h->max_sd.

        h->max_sd = max(h->max_sd, h->http_sd);
        h->max_sd = max(h->max_sd, h->https_sd);
    }
}


//
//  convert_time() - Convert time from RFC 1123 format to internal format.
//
////////////////////////////////////////////////////////////////////////////////

#if     defined(HTTP_EXTRA)

static time_t convert_time(const char *timestr)
{
    assert(timestr != NULL);

    char weekday[3 + 1], month[3 + 1], tzone[3 + 1];
    uint second, minute, hour, mday, mon, year, wday;

    int rcode = sscanf(timestr, "%3s, %u %3s %u %u:%u:%u %3s\n", weekday,
                       &mday, month, &year, &hour, &minute, &second, tzone);

    if (rcode != 8 || (strcmp(tzone, "GMT") && strcmp(tzone, "UTC")))
    {
        return (time_t)-1;
    }

    const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    const char *monstr = strstr(months, month);

    if (monstr == NULL)
    {
        return (time_t)-1;
    }

    mon = (uint)(monstr - months);

    if (mon % 3)
    {
        return (time_t)-1;
    }

    const char *weekdays = "SunMonTueWedThuFriSat";
    const char *wdaystr = strstr(weekdays, weekday);

    if (wdaystr == NULL)
    {
        return (time_t)-1;
    }

    wday = (uint)(wdaystr - weekdays);

    if (wday % 3)
    {
        return (time_t)-1;
    }

    struct timezone tz;
    struct timeval tv;

    if (gettimeofday(&tv, &tz) == -1)
    {
        return (time_t)-1;
    }

    minute -= (uint)tz.tz_minuteswest;

    // TODO: Add daylight savings time adjustment?

    struct tm tm =
    {
        .tm_sec   = (int)second,
        .tm_min   = (int)minute,
        .tm_hour  = (int)hour,
        .tm_mday  = (int)mday,
        .tm_mon   = (int)mon / 3,
        .tm_year  = (int)year - 1900,
        .tm_wday  = (int)wday / 3,
        .tm_yday  = 0,
        .tm_isdst = -1,
    };                                  //lint !e785

    return mktime(&tm);
}

#endif

//
//  end_connection() - End connection.
//
////////////////////////////////////////////////////////////////////////////////

void end_connection(http_conn_t *conn)
{
    assert(conn != NULL);

    http_ctl_t *h = &http_ctl;

#if     G2H_HTTPS

    if (conn->https)
    {
        tls_end(conn);
    }

#endif

    h->sockets[conn->sd] = NULL;        // Disconnect from socket table

    reset_connection(conn);

    conn->input_cb  = NULL;
    conn->output_cb = NULL;
    conn->error_cb  = NULL;
    conn->state     = SOCKET_IDLE;

    close_socket(&conn->sd);

    xfree(&conn);
}

//
//  init_connection() - Initialize data structures for connection.
//
////////////////////////////////////////////////////////////////////////////////

http_conn_t *init_connection(int sd)
{
    http_ctl_t *h = &http_ctl;
    http_conn_t *conn;

    if (!xcalloc(&conn, 1, sizeof(*conn)))
    {
        return NULL;
    }

    conn->sd           = sd;
    conn->state        = SOCKET_IDLE;
    conn->content_type = MIME_UNKNOWN;
    conn->persistent   = true;
    conn->method       = HTTP_UNKNOWN;
    conn->accept       = MIME_UNKNOWN;
    conn->status       = HTTP_STATUS_OK;
    conn->rxtimeout    = xtime();       // Expiration timer for receiving input

    h->sockets[sd]   = conn;
    h->max_sd        = max(h->max_sd, sd);

    FD_SET((uint)sd, &h->read_fds);

    return conn;
}

//
//  init_socket() - Initialize a socket.
//
////////////////////////////////////////////////////////////////////////////////

int init_socket(int family, int type, int protocol)
{
    // Create a socket

    int sd = socket(family, type, protocol);

    if (sd == -1)
    {
        dlog(DLOG_ERR, "socket(): %s\n", strerror(errno));

        return -errno;
    }

    int rcode = set_mode(sd);

    if (rcode < 0)
    {
        close_socket(&sd);

        return rcode;
    }

    return sd;
}

//
//  methodtoint() - Convert HTTP method from string to value.
//
////////////////////////////////////////////////////////////////////////////////

http_method_t methodtoint(const char *method)
{
    if (method != NULL)
    {
        uint i;

        for (i = 0; i < countof(methods); ++i)
        {
            if (!strcasecmp(method, methods[i].string))
            {
                return methods[i].value;
            }
        }
    }

    return HTTP_UNKNOWN;
}

//
//  methodtostr() - Convert HTTP method from value to string.
//
////////////////////////////////////////////////////////////////////////////////

const char *methodtostr(http_method_t method)
{
    uint i;

    for (i = 0; i < countof(methods); ++i)
    {
        if (method == methods[i].value)
        {
            return methods[i].string;
        }
    }

    return "UNKNOWN";
}

//
//  next_header() - Format next header.
//
////////////////////////////////////////////////////////////////////////////////

static int next_header(http_conn_t *conn)
{
    assert(conn != NULL);

    int rcode;
    uint size = sizeof(conn->line);
    bool last = false;                  // Assume not on last header

    if (conn->xo.connection && !conn->persistent)
    {
        conn->xo.connection = false;

        if (!xsnprintf(NULL, conn->line, size, "Connection: %s" CRLF, "close"))
        {
            return -1;
        }
    }
#if     defined(HTTP_EXTRA)
    else if (conn->xo.date)
    {
        conn->xo.date = false;

        char timebuf[MAX_LINE_LEN + 1];

        strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&conn->date));

        if (!xsnprintf(NULL, conn->line, size, "Date: %s" CRLF, timebuf))
        {
            return -1;
        }
    }
#endif
    else if (conn->xo.transfer_encoding)
    {
        conn->xo.content_length = false; // No Content-Length if chunking
        conn->xo.transfer_encoding = false;

        if (!xsnprintf(NULL, conn->line, size, "Transfer-Encoding: %s" CRLF,
                       conn->chunked ? "chunked" : ""))
        {
            return -1;
        }
    }
    else if (conn->xo.content_encoding)
    {
        conn->xo.content_encoding = false;

        if (!xsnprintf(NULL, conn->line, size, "Content-Encoding: exi" CRLF))
        {
            return -1;
        }
    }
    else if (conn->xo.content_length)
    {
        conn->xo.content_length = false;

        if (!xsnprintf(NULL, conn->line, size, "Content-Length: %u" CRLF,
                       conn->content_length))
        {
            return -1;
        }
    }
    else if (conn->xo.location)
    {
        conn->xo.location = false;

        if (!xsnprintf(NULL, conn->line, size, "Location: %s" CRLF, conn->location))
        {
            return -1;
        }
    }
    else if (conn->xo.content_type)
    {
        conn->xo.content_type = false;

        if (!xsnprintf(&rcode, conn->line, size, "Content-Type: "))
        {
            return -1;
        }

        uint nbytes = (uint)rcode;

        size -= (uint)rcode;

        if (conn->content_type == MIME_APPLICATION_EXI)
        {
            if (!xsnprintf(NULL, conn->line + nbytes, size,
                           "application/sep+exi" CRLF))
            {
                return -1;
            }
        }
        else if (conn->content_type == MIME_TEXT_XML)
        {
            if (!xsnprintf(NULL, conn->line + nbytes, size,
                             APPLICATION_SEP_XML CRLF))
            {
                return -1;
            }
        }
        else
        {
            if (!xsnprintf(NULL, conn->line + nbytes, size, "text/html" CRLF))
            {
                return -1;
            }
        }
    }
#if     defined(HTTP_EXTRA)
    else if (conn->xo.last_modified)
    {
        conn->xo.last_modified = false;

        char timebuf[MAX_LINE_LEN + 1];

        strftime(timebuf, sizeof(timebuf), RFC1123FMT,
                 gmtime(&conn->last_modified));

        if (!xsnprintf(NULL, conn->line, size, "Last-Modified: %s" CRLF, timebuf))
        {
            return -1;
        }
    }
#endif
    else if (conn->xo.accept)
    {
        conn->xo.accept = false;

        if (!xsnprintf(&rcode, conn->line, size, "Accept: "))
        {
            return -1;
        }

        uint nbytes = (uint)rcode;

        size -= (uint)rcode;

        if (conn->accept == MIME_APPLICATION_EXI)
        {
            if (!xsnprintf(NULL, conn->line + nbytes, size,
                             "application/sep+exi" CRLF))
            {
                return -1;
            }
        }
        else if (conn->accept == MIME_TEXT_XML)
        {
            if (!xsnprintf(NULL, conn->line + nbytes, size,
                             APPLICATION_SEP_XML CRLF))
            {
                return -1;
            }
        }
        else
        {
            if (!xsnprintf(NULL, conn->line + nbytes, size, "text/html" CRLF))
            {
                return -1;
            }
        }
    }
    else if (conn->xo.host)
    {
        conn->xo.host = false;

#if     G2H_IP == IPVX

        if (http_ctl.ai_family == AF_INET6)

#endif

#if     G2H_IP & IPV6

        {
            if (!xsnprintf(NULL, conn->line, size, "Host: [%.*s]:%u" CRLF,
                           MAX_HOST_LEN + 1, conn->host, conn->port))
            {
                return -1;
            }
        }

#endif

#if     G2H_IP == IPVX

        else

#endif

#if     G2H_IP & IPV4

        {
            if (!xsnprintf(NULL, conn->line, size, "Host: %.*s:%u" CRLF,
                           MAX_HOST_LEN + 1, conn->host, conn->port))
            {
                return -1;
            }
        }

#endif

    }
    else if (conn->xo.te)
    {
        conn->xo.te = false;

        if (!xsnprintf(NULL, conn->line, size, "TE: %s" CRLF,
                       conn->chunked ? "chunked" : ""))
        {
            return -1;
        }
    }
#if     defined(HTTP_EXTRA)
    else if (conn->xo.user_agent)
    {
        conn->xo.user_agent = false;

        if (!xsnprintf(NULL, conn->line, size, "User-Agent: %.*s" CRLF,
                       sizeof(conn->user_agent), conn->user_agent))
        {
            return -1;
        }
    }
#endif
    else if (conn->xo.allow)
    {
        conn->xo.allow = false;

        if (!xsnprintf(NULL, conn->line, size,
                       "Allow: GET, POST, PUT, HEAD, OPTIONS" CRLF))
        {
            return -1;
        }
    }
#if     defined(HTTP_EXTRA)
    else if (conn->xo.server)
    {
        conn->xo.server = false;

        if (!xsnprintf(NULL, conn->line, size, "Server: %.*s" CRLF,
                       sizeof(conn->server), conn->server))
        {
            return -1;
        }
    }
#endif
    else                                // No more headers, output blank line
    {
        if (!xsnprintf(NULL, conn->line, size, CRLF))
        {
            return -1;
        }

        last = true;                    // Last header has been output
    }

    return last ? 0 : 1;
}

//
//  null_test() - Dummy function for test callback.
//
////////////////////////////////////////////////////////////////////////////////

static bool null_test(void)
{
    return false;                       // No test running
}

//
//  recv_header() - Read header for request or response.
//
////////////////////////////////////////////////////////////////////////////////

int recv_header(http_conn_t *conn, uint type)
{
    assert(conn != NULL);

    const char *name, *value;

    if ((name = strtok(conn->line, ":")) == NULL || !isgraph((int)*name))
    {
        return -HTTP_STATUS_BAD_REQUEST;
    }

    if ((value = strtok(NULL, "\r\n")) != NULL)
    {
        value += strspn(value, "\t ");
    }

    dlog(DLOG_INFO, "%s: %s\n", name, value != NULL ? value : "");

    if ((type & HTTP_GENERAL) && value != NULL)
    {
        if (!strcasecmp(name, "Connection") && !strcasecmp(value, "close"))
        {
            conn->persistent = false;
        }
#if     defined(HTTP_EXTRA)
        else if (!strcasecmp(name, "Date"))
        {
            if ((conn->date = convert_time(value)) != (time_t)-1)
            {
                conn->xi.date = true;
            }
        }
#endif
        else if (!strcasecmp(name, "Transfer-Encoding"))
        {
            if (!strcasecmp(value, "chunked"))
            {
                conn->chunked = true;
            }
        }
    }

    if ((type & HTTP_ENTITY) && value != NULL)
    {
        if (!strcasecmp(name, "Content-Encoding") && !strcasecmp(value, "exi"))
        {
            conn->xi.content_encoding = true;
        }
        else if (!strcasecmp(name, "Content-Length"))
        {
            conn->xi.content_length = true;
            conn->content_length = (uint)strtoul(value, NULL, 10);
        }
        else if (!strcasecmp(name, "Content-Type"))
        {
            conn->xi.content_type = true;

            if (!strcasecmp(value, "application/sep+exi"))
            {
                conn->content_type = MIME_APPLICATION_EXI;
            }
            else if (!strcasecmp(value, APPLICATION_SEP_XML))
            {
                conn->content_type = MIME_TEXT_XML;
            }
            else
            {
                conn->content_type = MIME_UNKNOWN;
            }
        }
        else if (!strcasecmp(name, "Location"))
        {
            conn->xi.location = true;
            strncpy(conn->location, value, MAX_URI_LEN+1);
        }
#if     defined(HTTP_EXTRA)
        else if (!strcasecmp(name, "Last-Modified"))
        {
            if ((conn->last_modified = convert_time(value)) != (time_t)-1)
            {
                conn->xi.last_modified = true;
            }
        }
#endif
    }

    if (type & HTTP_REQUEST)
    {
        if (!strcasecmp(name, "Accept") && value != NULL)
        {
            if (!strcasecmp(value, "application/sep+exi"))
            {
                conn->xi.content_type = true;
                conn->content_type = MIME_APPLICATION_EXI;
            }
            else
            {
                if (strcasecmp(value, APPLICATION_SEP_XML) && strcasecmp(value, "*/*"))
                {
#if     defined(HTTP_STRICT)
                    return -HTTP_STATUS_NONE_ACCEPTABLE;
#else
                    dlog(DLOG_WARNING, "Unexpected value for Accept header: %s\n",
                           value);
#endif
                }

                conn->xi.content_type = true;
                conn->content_type = MIME_TEXT_XML;
            }
        }
        else if (!strcasecmp(name, "Host"))
        {
            conn->xi.host = true;

            if (value == NULL)
            {
                memset(conn->host, '\0', sizeof(conn->host));
            }
            else
            {
                if (!xsnprintf(NULL, conn->host, sizeof(conn->host), "%s", value))
                {
                    return -1;
                }
            }
        }
        else if (!strcasecmp(name, "TE") && value != NULL)
        {
            if (!strcasecmp(value, "chunked"))
            {
                conn->chunked = true;
            }
        }
#if     defined(HTTP_EXTRA)
        else if (!strcasecmp(name, "User-Agent"))
        {
            conn->xi.user_agent = true;

            if (value == NULL)
            {
                memset(conn->user_agent, '\0', sizeof(conn->user_agent));
            }
            else
            {
                if (!xsnprintf(NULL, conn->user_agent, sizeof(conn->user_agent),
                               "%s", value))
                {
                    return -1;
                }
            }
        }
#endif
    }

    if (type & HTTP_RESPONSE)
    {
        if (!strcasecmp(name, "Allow"))
        {
            // TODO: what to do here?
        }
#if     defined(HTTP_EXTRA)
        else if (!strcasecmp(name, "Server"))
        {
            conn->xi.server = true;

            if (value == NULL)
            {
                memset(conn->server, '\0', sizeof(conn->server));
            }
            else
            {
                if (!xsnprintf(NULL, conn->server, sizeof(conn->server), "%s", value))
                {
                    return -1;
                }
            }
        }
#endif
    }

    return 0;
}

//
//  recv_line() - Read next line from client, terminated by either LF or CRLF.
//                The returned count will only include the LF, and thus a count
//                of 1 means that the line is blank.
//
////////////////////////////////////////////////////////////////////////////////

static int recv_line(http_conn_t *conn)
{
    assert(conn != NULL);

    for (;;)
    {
        char c;
        int rcode = http_recv(conn, &c, 1);

        if (rcode < 0)
        {
            return rcode;
        }

        if ((conn->line[conn->pos++] = c) == '\n')
        {
            break;
        }

        if (conn->pos == MAX_LINE_LEN)
        {
            dlog(DLOG_ERR, "Input line exceeds max. length of %u bytes\n",
                   MAX_LINE_LEN);

            return -HTTP_STATUS_SERVER_ERROR;
        }
    }

    // Translate CRLF to LF

    if (conn->pos >= 2 && conn->line[conn->pos - 2] == '\r')
    {
        conn->line[conn->pos - 2] = '\n';
        --conn->pos;
    }

    int count = (int)conn->pos;
    conn->pos = 0;
    conn->line[count] = '\0';

    return count;
}

//
//  reset_connection() - Reset connection data.
//
////////////////////////////////////////////////////////////////////////////////

void reset_connection(http_conn_t *conn)
{
    assert(conn != NULL);

    xfree(&conn->entity.start);
    xfree(&conn->inbuf.start);
    xfree(&conn->outbuf.start);

    memset(conn->uri, '\0', sizeof(conn->uri));

    conn->entity.offset = 0;
    conn->entity.size   = 0;

    conn->inbuf.size    = 0;
    conn->inbuf.offset  = 0;

    conn->outbuf.size   = 0;
    conn->outbuf.offset = 0;

    memset(conn->host, '\0', sizeof(conn->host));

#if     defined(HTTP_EXTRA)

    memset(conn->server, '\0', sizeof(conn->server));
    memset(conn->user_agent, '\0', sizeof(conn->user_agent));

#endif

    conn->line[0] = '\0';
    conn->pos     = 0;

    // Reset input flags

    conn->xi.accept             = false;
    conn->xi.allow              = false;
    conn->xi.connection         = false;
    conn->xi.date               = false;
    conn->xi.content_encoding   = false;
    conn->xi.content_length     = false;
    conn->xi.location           = false;
    conn->xi.content_type       = false;
    conn->xi.host               = false;
    conn->xi.last_modified      = false;
    conn->xi.server             = false;
    conn->xi.te                 = false;
    conn->xi.transfer_encoding  = false;
    conn->xi.user_agent         = false;

    // Reset output flags

    conn->xo.accept             = false;
    conn->xo.allow              = false;
    conn->xo.connection         = false;
    conn->xo.date               = false;
    conn->xo.content_encoding   = false;
    conn->xo.content_length     = false;
    conn->xo.location           = false;
    conn->xo.content_type       = false;
    conn->xo.host               = false;
    conn->xo.last_modified      = false;
    conn->xo.server             = false;
    conn->xo.te                 = false;
    conn->xo.transfer_encoding  = false;
    conn->xo.user_agent         = false;
}

//
//  set_blocking() - Set socket to be blocking or nonblocking.
//
////////////////////////////////////////////////////////////////////////////////

int set_blocking(int blocking)
{
    http_ctl_t *h = &http_ctl;

    if ((h->blocking = (bool)blocking) == true)
    {
        h->tv.tv_sec = 1;
    }
    else
    {
        h->tv.tv_sec = 0;
    }

    return 0;
}

//
//  set_callback() - Set functions to call when HTTP requests or responses
//                   received.
//
////////////////////////////////////////////////////////////////////////////////

int set_callback(http_header_t type, void (*callback)(http_conn_t *conn))
{
    assert(callback != NULL);

    http_ctl_t *h = &http_ctl;

    switch (type)
    {
        case HTTP_REQUEST:
            h->request_cb = callback;

            break;

        case HTTP_RESPONSE:
            h->response_cb = callback;

            break;

        default:
            return -EPROTO;
    }

    return 0;
}

//
//  set_family() - Set address/protocol family to use for connections.
//
////////////////////////////////////////////////////////////////////////////////

int set_family(int family)
{
    http_ctl_t *h = &http_ctl;

    switch (family)
    {
        case AF_UNSPEC:
        case AF_INET:

#if     AF_INET6 != AF_INET
        case AF_INET6:
#endif

#if     defined(PF_INET) && AF_INET != PF_INET
        case PF_INET:
#endif

#if     AF_INET6 != PF_INET6
        case PF_INET6:
#endif

            h->ai_family = (uint16_t)family;

            break;

        default:
            return -EPROTO;
    }

    return 0;                   // Family set
}

//
//  set_flags() - Set NI flags for getnameinfo().
//
////////////////////////////////////////////////////////////////////////////////

int set_flags(int flags)
{
#if     defined(STDLIB_BUILD)
    http_ctl_t *h = &http_ctl;

    h->ni_flags = flags;
#endif

    return 0;
}

//
//  set_mode() - Set socket to be nonblocking (if we can).
//
////////////////////////////////////////////////////////////////////////////////

int set_mode(int sd)
{
    http_ctl_t *h = &http_ctl;

    if (!h->blocking)
    {
        int flags;

        if ((  flags = fcntl(sd, F_GETFL, 0                 )) == -1
            ||         fcntl(sd, F_SETFL, flags | O_NONBLOCK)  == -1)
        {
            dlog(DLOG_ERR, "Can't set nonblocking mode\n");

            return -errno;
        }
    }

    return 0;                   // Nonblocking mode set
}
