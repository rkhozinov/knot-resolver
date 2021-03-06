/*  Copyright (C) 2015-2017 CZ.NIC, z.s.p.o. <knot-dns@labs.nic.cz>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "daemon/tls.h"

#include "lib/generic/array.h"
#include "lib/generic/map.h"

#include <uv.h>
#include <stdbool.h>


struct engine;

/** Ways to listen for DNS on a port. */
typedef struct {
	int sock_type;	/**< SOCK_DGRAM or SOCK_STREAM */
	bool tls;	/**< only used together with .tcp */
} endpoint_flags_t;

static inline bool endpoint_flags_eq(endpoint_flags_t f1, endpoint_flags_t f2)
{
	/* memcmp() would typically work, but there's no guarantee. */
	return f1.sock_type == f2.sock_type && f1.tls == f2.tls;
}

/** Wrapper for a single socket to listen on. */
struct endpoint {
	uv_handle_t *handle; /** uv_udp_t or uv_tcp_t */
	uint16_t port;
	endpoint_flags_t flags;
};

/** @cond internal Array of endpoints */
typedef array_t(struct endpoint) endpoint_array_t;
/* @endcond */

struct net_tcp_param {
	uint64_t in_idle_timeout;
	uint64_t tls_handshake_timeout;
};

struct network {
	uv_loop_t *loop;

	/** Map: address string -> endpoint_array_t.
	 * \note even same address-port-flags tuples may appear.
	 * TODO: trie_t, keyed on *binary* address-port pair. */
	map_t endpoints;

	struct tls_credentials *tls_credentials;
	tls_client_params_t *tls_client_params; /**< Use tls_client_params_*() functions. */
	struct tls_session_ticket_ctx *tls_session_ticket_ctx;
	struct net_tcp_param tcp;
	int tcp_backlog;
};

void network_init(struct network *net, uv_loop_t *loop, int tcp_backlog);
void network_deinit(struct network *net);

/** Start listenting on addr#port with flags.
 * \note if we did listen on that combination already,
 *       nothing is done and kr_ok() is returned.
 * \note there's no short-hand to listen both on UDP and TCP. */
int network_listen(struct network *net, const char *addr, uint16_t port,
		   endpoint_flags_t flags);

/** Start listenting on an open file-descriptor. */
int network_listen_fd(struct network *net, int fd, bool use_tls);

/** Stop listening on all addr#port with equal flags.
 * \return kr_error(ENOENT) if nothing matched. */
int network_close(struct network *net, const char *addr, uint16_t port,
		  endpoint_flags_t flags);

int network_set_tls_cert(struct network *net, const char *cert);
int network_set_tls_key(struct network *net, const char *key);
void network_new_hostname(struct network *net, struct engine *engine);
int network_set_bpf(struct network *net, int bpf_fd);
void network_clear_bpf(struct network *net);
