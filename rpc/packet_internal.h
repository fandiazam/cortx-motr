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

#ifndef __MOTR_RPC_PACKET_INT_H__
#define __MOTR_RPC_PACKET_INT_H__

#include "lib/vec.h"
#include "lib/tlist.h"
#include "rpc/onwire.h"

/**
 * @addtogroup rpc
 * @{
 */

/* Imports */
struct m0_rpc_item;
struct m0_rpc_frm;

/**
   RPC Packet (aka RPC) is a collection of RPC items that are sent together
   in same network buffer.
 */
struct m0_rpc_packet {

	struct m0_rpc_packet_onwire_header rp_ow;

	/** Onwire size of this packet, including header */
	m0_bcount_t                        rp_size;

	/**
	   List of m0_rpc_item objects placed using ri_plink.
	   List descriptor: packet_item
	 */
	struct m0_tl                       rp_items;

	/**
	   Successfully sent (== 0) or was there any error while sending (!= 0)
	 */
	int                                rp_status;

	struct m0_rpc_frm                 *rp_frm;

	struct m0_rpc_machine             *rp_rmachine;
};

M0_INTERNAL m0_bcount_t m0_rpc_packet_onwire_header_size(void);
M0_INTERNAL m0_bcount_t m0_rpc_packet_onwire_footer_size(void);

M0_TL_DESCR_DECLARE(packet_item, M0_EXTERN);
M0_TL_DECLARE(packet_item, M0_INTERNAL, struct m0_rpc_item);

#define for_each_item_in_packet(item, packet) \
	m0_tl_for(packet_item, &packet->rp_items, item)

#define end_for_each_item_in_packet m0_tl_endfor

M0_INTERNAL bool m0_rpc_packet_invariant(const struct m0_rpc_packet *packet);
M0_INTERNAL void m0_rpc_packet_init(struct m0_rpc_packet *packet,
				    struct m0_rpc_machine *rmach);
M0_INTERNAL void m0_rpc_packet_fini(struct m0_rpc_packet *packet);

/** Removes all items from the packet, finalises and frees it. */
M0_INTERNAL void m0_rpc_packet_discard(struct m0_rpc_packet *packet);

/**
   @pre  !packet_item_tlink_is_in(item)
   @post m0_rpc_packet_is_carrying_item(packet, item)
 */
M0_INTERNAL void m0_rpc_packet_add_item(struct m0_rpc_packet *packet,
					struct m0_rpc_item *item);

/**
   @pre  m0_rpc_packet_is_carrying_item(packet, item)
   @post !packet_item_tlink_is_in(item)
 */
M0_INTERNAL void m0_rpc_packet_remove_item(struct m0_rpc_packet *packet,
					   struct m0_rpc_item *item);

/**
   @post m0_rpc_packet_is_empty(packet)
 */
M0_INTERNAL void m0_rpc_packet_remove_all_items(struct m0_rpc_packet *packet);

M0_INTERNAL bool m0_rpc_packet_is_empty(const struct m0_rpc_packet *packet);

/**
   Returns true iff item is included in packet.
 */
M0_INTERNAL bool m0_rpc_packet_is_carrying_item(const struct m0_rpc_packet
						*packet,
						const struct m0_rpc_item *item);

/**
   Serialises packet in buffer pointed by bufvec.

   @pre !m0_rpc_packet_is_empty(packet)
 */
M0_INTERNAL int m0_rpc_packet_encode(struct m0_rpc_packet *packet,
				     struct m0_bufvec *bufvec);

/**
   Serialises packet in location pointed by cursor.

   @pre !m0_rpc_packet_is_empty(packet)
 */
M0_INTERNAL int m0_rpc_packet_encode_using_cursor(struct m0_rpc_packet *packet,
						  struct m0_bufvec_cursor
						  *cursor);

/**
   Decodes packet from bufvec.
 */
M0_INTERNAL int m0_rpc_packet_decode(struct m0_rpc_packet *packet,
				     struct m0_bufvec *bufvec,
				     m0_bindex_t off, m0_bcount_t len);

/**
   Decodes packet from location pointed by bufvec cursor.
 */
M0_INTERNAL int m0_rpc_packet_decode_using_cursor(struct m0_rpc_packet *packet,
						  struct m0_bufvec_cursor
						  *cursor, m0_bcount_t len);

typedef void item_visit_fn(struct m0_rpc_packet *packet,
			   struct m0_rpc_item *item, int data);

/**
   Iterates through all the items in the packet p and calls visit function
   for each item. Passes opaque_data as it is to visit function.
 */
M0_INTERNAL void m0_rpc_packet_traverse_items(struct m0_rpc_packet *p,
					      item_visit_fn * visit,
					      int opaque_data);

/** @} rpc */
#endif /* __MOTR_RPC_PACKET_INT_H__ */

/*
 *  Local variables:
 *  c-indentation-style: "K&R"
 *  c-basic-offset: 8
 *  tab-width: 8
 *  fill-column: 80
 *  scroll-step: 1
 *  End:
 */
