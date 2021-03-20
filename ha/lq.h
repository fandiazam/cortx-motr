/* -*- C -*- */
/*
 * Copyright (c) 2016-2020 Seagate Technology LLC and/or its Affiliates
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

#ifndef __MOTR_HA_LINK_QUEUE_H__
#define __MOTR_HA_LINK_QUEUE_H__

/**
 * @defgroup ha
 *
 * HA link queue maintains the queue of m0_ha_msg along with tags.
 * The tags are "pointers" inside the tags range for the queue.
 *
 * * Tags and queue relationship
 * @verbatim
 *
 *  first message in queue               last message in queue
 *  (next to be dequeued)                (has been enqeued last)
 *     v                                             v
 *    +-+-+-+     +-+-+-+-+-+     +-+-+-+-+     +-+-+-+
 *    | | | | ... | | | | | | ... | | | | | |   | | | |      tag value increase
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+->
 *     ^             ^             ^                   ^
 *     |             |             |                   |
 *  confirmed    delivered        next               assign     <-- tags
 *
 * @endverbatim
 *
 * * API
 * - constructor, destructor, invariant:
 *   - m0_ha_lq_init()
 *   - m0_ha_lq_fini()
 *   - m0_ha_lq_invariant()
 * - getters/setters for the entire set of tags:
 *   - m0_ha_lq_tags_get()
 *   - m0_ha_lq_tags_set()
 * - accessors for the tag values:
 *   - m0_ha_lq_assign()
 *   - m0_ha_lq_next()
 *   - m0_ha_lq_delivered()
 *   - m0_ha_lq_confirmed()
 * - query information
 *   - m0_ha_lq_has_tag() - it can be a message in the queue with the given tag
 *   - m0_ha_lq_msg() - returns message by tag
 *   - m0_ha_lq_has_next() - there is a message to consume using m0_ha_lq_next()
 *   - m0_ha_lq_is_delivered() - checks if the message with the tag is delivered
 * - modify the link queue
 *   - m0_ha_lq_enqueue() - adds msg to the queue at the 'assign' position
 *   - m0_ha_lq_next() - returns the message at 'next' position
 *     if 'next' != 'assign', advances 'next' if such message exists
 *   - m0_ha_lq_try_unnext() - decreases 'next' if it's != 'delivered', returns
 *     if the decrease was successful
 *   - m0_ha_lq_mark_delivered() - advances 'delivered'
 *   - m0_ha_lq_dequeue() - removes a message at 'confirmed' position from the
 *     queue, advances 'confirmed', returns 'confirmed' value before the
 *     advancement
 *
 * * Known issues
 * - there is no special handling for tag values overflow;
 * - memory allocation failure is not handled properly in m0_ha_lq_enqueue().
 *
 * @{
 */

#include "lib/types.h"          /* bool */
#include "ha/msg_queue.h"       /* m0_ha_msg_queue */
#include "ha/link_fops.h"       /* m0_ha_link_tags */

struct m0_ha_lq_cfg {
	struct m0_ha_msg_queue_cfg hlqc_msg_queue_cfg;
};

/*
 * func: enqueue()  next()   delivered()   dequeue()
 * var:  assign     next     delivered     confirmed
 */
struct m0_ha_lq {
	struct m0_ha_lq_cfg    hlq_cfg;
	struct m0_ha_msg_queue hlq_mq;
	struct m0_ha_link_tags hlq_tags;
};

M0_INTERNAL void m0_ha_lq_init(struct m0_ha_lq           *lq,
                               const struct m0_ha_lq_cfg *lq_cfg);
M0_INTERNAL void m0_ha_lq_fini(struct m0_ha_lq *lq);
M0_INTERNAL bool m0_ha_lq_invariant(const struct m0_ha_lq *lq);

M0_INTERNAL void m0_ha_lq_tags_get(const struct m0_ha_lq  *lq,
                                   struct m0_ha_link_tags *tags);
M0_INTERNAL void m0_ha_lq_tags_set(struct m0_ha_lq              *lq,
                                   const struct m0_ha_link_tags *tags);

M0_INTERNAL bool m0_ha_lq_has_tag(const struct m0_ha_lq *lq, uint64_t tag);
M0_INTERNAL struct m0_ha_msg *m0_ha_lq_msg(struct m0_ha_lq *lq, uint64_t tag);
M0_INTERNAL struct m0_ha_msg *m0_ha_lq_msg_next(struct m0_ha_lq *lq,
						const struct m0_ha_msg *cur);
M0_INTERNAL struct m0_ha_msg *m0_ha_lq_msg_prev(struct m0_ha_lq *lq,
						const struct m0_ha_msg *cur);
M0_INTERNAL bool m0_ha_lq_has_next(const struct m0_ha_lq *lq);
M0_INTERNAL bool m0_ha_lq_is_delivered(const struct m0_ha_lq *lq, uint64_t tag);

M0_INTERNAL uint64_t m0_ha_lq_tag_assign(const struct m0_ha_lq *lq);
M0_INTERNAL uint64_t m0_ha_lq_tag_next(const struct m0_ha_lq *lq);
M0_INTERNAL uint64_t m0_ha_lq_tag_delivered(const struct m0_ha_lq *lq);
M0_INTERNAL uint64_t m0_ha_lq_tag_confirmed(const struct m0_ha_lq *lq);

M0_INTERNAL uint64_t m0_ha_lq_enqueue(struct m0_ha_lq        *lq,
                                      const struct m0_ha_msg *msg);
M0_INTERNAL struct m0_ha_msg *m0_ha_lq_next(struct m0_ha_lq *lq);
M0_INTERNAL bool m0_ha_lq_try_unnext(struct m0_ha_lq *lq);
M0_INTERNAL void m0_ha_lq_mark_delivered(struct m0_ha_lq *lq, uint64_t tag);
M0_INTERNAL uint64_t m0_ha_lq_dequeue(struct m0_ha_lq *lq);

/** @} end of ha group */
#endif /* __MOTR_HA_LINK_QUEUE_H__ */

/*
 *  Local variables:
 *  c-indentation-style: "K&R"
 *  c-basic-offset: 8
 *  tab-width: 8
 *  fill-column: 80
 *  scroll-step: 1
 *  End:
 */
/*
 * vim: tabstop=8 shiftwidth=8 noexpandtab textwidth=80 nowrap
 */
