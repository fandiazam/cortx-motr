/* -*- C -*- */
/*
 * Copyright (c) 2014-2020 Seagate Technology LLC and/or its Affiliates
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

#ifndef __MOTR_GRAPH_GRAPH_H__
#define __MOTR_GRAPH_GRAPH_H__

#include "fid/fid.h"
#include "fid/fid_xc.h"
#include "lib/cookie.h"
#include "lib/cookie_xc.h"
#include "xcode/xcode_attr.h"

/**
 * @defgroup graph
 *
 * @{
 */

struct m0_graph;
struct m0_garc;
struct m0_gvertice;

struct m0_gvertice_type;
struct m0_garc_type;

struct m0_gvertice_header;
struct m0_garc_src;
struct m0_garc_dst;

struct m0_garc {
	struct m0_fid    as_fid;
	struct m0_cookie as_local;
	struct m0_cookie as_remote;
} M0_XCA_RECORD;

struct m0_gvertice {
	uint64_t         vh_typeid;
	struct m0_fid    vh_fid;
	uint64_t         vh_gen;
	struct m0_cookie vh_cookie;
	struct m0_garc   vh_next;
	struct m0_garc   vh_prev;
} M0_XCA_RECORD;

struct m0_graph {
	struct m0_gvertice g_anchor;
} M0_XCA_RECORD;

enum { M0_GRAPH_ARC_PER_NODE_MAX = 16 };

struct m0_gvertice_type {
	uint64_t                    vt_id;
	const char                 *vt_name;
	const struct m0_xcode_type *vt_xt;
	uint32_t                    vt_arc_nr;
	const struct m0_garc_type  *vt_arc[M0_GRAPH_ARC_PER_NODE_MAX];
};

struct m0_garc_type {
	const char                *at_name;
	size_t                     at_field;
	const struct m0_garc_type *at_reverse;
};

bool m0_gvertice_invariant(const struct m0_gvertice *vertice);
bool m0_graph_invariant(const struct m0_graph *graph);
bool m0_gvertice_type_invariant(const struct m0_gvertice_type *vt);
bool m0_garc_type_invariant(const struct m0_gvertice_type *vt,
			    const struct m0_garc_type *atype);

void m0_gvertice_link(struct m0_gvertice *src, struct m0_gvertice *dst,
		      const struct m0_garc_type *atype);
void m0_gvertice_unlink(struct m0_gvertice *src, struct m0_gvertice *dst,
			const struct m0_garc_type *atype);
bool m0_gvertice_linked(const struct m0_gvertice *src,
			const struct m0_gvertice *dst,
			const struct m0_garc_type *atype);
bool m0_gvertice_is_set(const struct m0_gvertice *vertice,
			const struct m0_garc_type *atype);

void m0_gvertice_init(struct m0_graph *g, struct m0_gvertice *vertice,
		      const struct m0_gvertice_type *vt,
		      const struct m0_fid *fid);
void m0_gvertice_fini(struct m0_graph *g, struct m0_gvertice *vertice);

struct m0_gvertice *m0_garc_try(const struct m0_gvertice *vertice,
				const struct m0_garc_type *atype);

int m0_garc_follow(const struct m0_gvertice *vertice,
		   const struct m0_garc_type *atype, ...);

void m0_gvertice_type_register(struct m0_gvertice_type *vt);
void m0_garc_type_register(const struct m0_garc_type *atype);
void m0_garc_type_pair_register(struct m0_garc_type *direct,
				struct m0_garc_type *reverse);

void m0_garc_type_add(struct m0_gvertice_type *vt,
		      const struct m0_garc_type *atype);
M0_INTERNAL int  m0_graph_mod_init(void);
M0_INTERNAL void m0_graph_mod_fini(void);

/** @} end of graph group */
#endif /* __MOTR_GRAPH_GRAPH_H__ */

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
