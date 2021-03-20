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

#ifndef __MOTR_NET_LNET_PVT_H__
#define __MOTR_NET_LNET_PVT_H__

/**
   @addtogroup LNetXODFS
   @{
 */

/* forward references to other static functions */
static bool nlx_tm_invariant(const struct m0_net_transfer_mc *tm);
static void nlx_tm_ev_worker(struct m0_net_transfer_mc *tm);
static bool nlx_ep_invariant(const struct m0_net_end_point *ep);
static int nlx_ep_create(struct m0_net_end_point **epp,
			 struct m0_net_transfer_mc *tm,
			 const struct nlx_core_ep_addr *cepa);
static bool nlx_xo_buffer_bufvec_invariant(const struct m0_net_buffer *nb);

/**
   Inline helper to get the Core EP address pointer from an end point.
 */
static inline
struct nlx_core_ep_addr *nlx_ep_to_core(struct m0_net_end_point *ep)
{
	struct nlx_xo_ep *xep;
	M0_PRE(nlx_ep_invariant(ep));
	xep = container_of(ep, struct nlx_xo_ep, xe_ep);
	return &xep->xe_core;
}

static int nlx_xo_core_bev_to_net_bev(struct m0_net_transfer_mc *tm,
				      struct nlx_core_buffer_event *lcbev,
				      struct m0_net_buffer_event *nbev);

static m0_time_t nlx_tm_get_buffer_timeout_tick(const struct
						m0_net_transfer_mc *tm);
static int nlx_tm_timeout_buffers(struct m0_net_transfer_mc *tm, m0_time_t now);

/** @} */ /* LNetXODFS */

/**
   @addtogroup LNetCore
   @{
 */

/* forward references to other static functions */
static bool nlx_core_buffer_invariant(const struct nlx_core_buffer *cb);
static bool nlx_core_tm_is_locked(const struct nlx_core_transfer_mc *ctm);

/**
   Compute the checksum for a memory location reference.
 */
static uint32_t nlx_core_kmem_loc_checksum(const struct nlx_core_kmem_loc *loc);

/**
   Test if a memory location reference is valid.
 */
static inline bool nlx_core_kmem_loc_invariant(
					   const struct nlx_core_kmem_loc *loc)
{
	return _0C(loc != NULL) && _0C(loc->kl_page != NULL) &&
		_0C(loc->kl_checksum == nlx_core_kmem_loc_checksum(loc));
}

/**
   Test if a memory location object is empty (not set).
 */
static inline bool nlx_core_kmem_loc_is_empty(
					   const struct nlx_core_kmem_loc *loc)
{
	return loc->kl_page == NULL &&
		loc->kl_offset == 0 &&
		loc->kl_checksum == 0;
}

/**
   Compare memory location objects for equality.
   These objects cannot be compared atomically.  The caller must ensure
   that appropriate synchronization is used if the objects can change.
 */
static inline bool nlx_core_kmem_loc_eq(const struct nlx_core_kmem_loc *a,
					const struct nlx_core_kmem_loc *b)
{
	if (a == NULL || b == NULL)
		return a == b;
	return a->kl_page == b->kl_page &&
		a->kl_offset == b->kl_offset &&
		a->kl_checksum == b->kl_checksum;
}

/**
   Decodes a NID string into a NID.
   @param lcdom The domain private data.
   @param nidstr the string to be decoded.
   @param nid On success, the resulting NID is returned here.
   @retval -EINVAL the NID string could not be decoded
 */
static int nlx_core_nidstr_decode(struct nlx_core_domain *lcdom,
				  const char *nidstr,
				  uint64_t *nid);

/**
   Encode a NID into its string representation.
   @param lcdom The domain private data.
   @param nid The NID to be converted.
   @param nidstr On success, the string form is set here.
 */
static int nlx_core_nidstr_encode(struct nlx_core_domain *lcdom,
				  uint64_t nid,
				  char nidstr[M0_NET_LNET_NIDSTR_SIZE]);

/** @} */ /* LNetCore */

#endif /* __MOTR_NET_LNET_PVT_H__ */

/*
 *  Local variables:
 *  c-indentation-style: "K&R"
 *  c-basic-offset: 8
 *  tab-width: 8
 *  fill-column: 80
 *  scroll-step: 1
 *  End:
 */
