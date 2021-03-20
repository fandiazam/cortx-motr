/* -*- C -*- */
/*
 * Copyright (c) 2012-2020 Seagate Technology LLC and/or its Affiliates
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For any questions about this software or licensing,
 * please email opensource@seagate.com or cortx-questions@seagate.com.
 *
 */


#pragma once

#ifndef __MOTR_LIB_COOKIE_H__
#define __MOTR_LIB_COOKIE_H__

#include "lib/types.h"
#include "xcode/xcode.h"

/**
 * @defgroup cookie Cookie
 *
 * In a network-file-system, when a client queries for an in-memory object to
 * a server, server searches through a set of data-structures to retrieve the
 * object. Multiple queries asking for the same object lead to a repeated
 * search.
 *
 * Cookie mechanism avoids such redundant search operations. When the first
 * query for an object arrives, server searches the object, embeds it's address
 * in a cookie, and then sends this cookie to a client. Client then uses this
 * cookie in subsequent queries for the same object.
 *
 * As client is unaware of memory-updates at server-end, its necessary for
 * server to verify that received cookie is not a stale one. Server achieves
 * this by maintaining a global counter called generation-count. It embeds
 * a same value of generation-count in an object and a cookie associated with
 * it. On reception of a cookie, before returning a required object,
 * server ensures that value of generation-count in the cookie matches
 * with the one in the object.
 * @{
 */

/**
 * Holds an address of a remote object and its generation count.
 */
struct m0_cookie {
	uint64_t co_addr;
	uint64_t co_generation;
} M0_XCA_RECORD M0_XCA_DOMAIN(be|rpc);

/**
 * Initializes the gencount. Gets called during motr initialization.
 */
M0_INTERNAL int m0_cookie_global_init(void);

M0_INTERNAL void m0_cookie_global_fini(void);

/**
 * Increments generation-count by one and assigns the same to *gen.
 */
M0_INTERNAL void m0_cookie_new(uint64_t * gen);

/**
 * Embeds address of an object along with a generation-count in a cookie.
 *
 * @param cookie (out)   address of a cookie in which obj gets embedded
 * @param obj (in)	 address of an object
 */
M0_INTERNAL void m0_cookie_init(struct m0_cookie *cookie, const uint64_t *obj);

/**
 * Retrieves address of an object from a cookie.
 *
 * @param cookie (in)   address of a cookie that holds the address of an object
 * @param addr (out)    pointer to a memory location which holds retrieved
 *                      address
 */
M0_INTERNAL int m0_cookie_dereference(const struct m0_cookie *cookie,
				      uint64_t **addr);

/**
 * Checks if address is pointing to a valid memory location.
 * @see m0_addr_is_sane_and_aligned()
 */
M0_INTERNAL bool m0_addr_is_sane(const uint64_t *addr);

/**
 * Checks if address is aligned to 8-byte address and is pointing to a valid
 * memory location.
 * @see m0_addr_is_sane()
 */
M0_INTERNAL bool m0_addr_is_sane_and_aligned(const uint64_t *addr);


/**
 * Returns 'true' when cookie is NULL.
 */
M0_INTERNAL bool m0_cookie_is_null(const struct m0_cookie *cookie);

/**
 * Compares two cookies.
 */
M0_INTERNAL bool m0_cookie_is_eq(const struct m0_cookie *cookie1,
				 const struct m0_cookie *cookie2);

/**
 * A macro to retrive address of a parent structure, associated with an object
 * embedded in a cookie.
 */
#define m0_cookie_of(cookie, type, field)		      \
({							      \
	uint64_t	       *__gen;			      \
	const struct m0_cookie *__cookie = (cookie);	      \
	m0_cookie_dereference(__cookie, &__gen) != 0 ? NULL : \
			container_of(__gen, type, field);     \
})

/** @} end of cookie group */
/*__M0_LIB_COOKIE_H__*/
#endif

M0_EXTERN const struct m0_cookie M0_COOKIE_NULL;
/*
 *  Local variables:
 *  c-indentation-style: "K&R"
 *  c-basic-offset: 8
 *  tab-width: 8
 *  fill-column: 80
 *  scroll-step: 1
 *  End:
 */
