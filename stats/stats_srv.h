/* -*- C -*- */
/*
 * Copyright (c) 2013-2020 Seagate Technology LLC and/or its Affiliates
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

#ifndef __MOTR_STATS_STATS_SVC_H__
#define __MOTR_STATS_STATS_SVC_H__
#ifndef __KERNEL__

/**
   @page DLD-stats-svc-fspecs Functional Specification

   - @ref DLD-stats-svc-fspecs-ds
   - @ref DLD-stats-svc-fspecs-int-if
   - @ref DLD-stats-svc-fspecs-ext-if

   <hr>
   @section DLD-stats-svc-fspecs-ds Data Structures
   - m0_stats
   - stats_svc
   - stats_list
   - stats_update_fom
   - stats_query_fom

   @section DLD-stats-svc-fspecs-int-if Internal Interfaces
   - stats_svc_rso_start()
   - stats_svc_rso_stop()
   - stats_svc_rso_fini()
   - stats_svc_rsto_service_allocate()

   - stats_update_fom_create()
   - stats_update_fom_tick()
   - stats_update_fom_home_locality()
   - stats_update_fom_fini()

   - stats_query_fom_create()
   - stats_query_fom_tick()
   - stats_query_fom_home_locality()
   - stats_query_fom_fini()

   @section DLD-stats-svc-fspecs-ext-if External Interfaces
   - m0_stats_svc_init()
   - m0_stats_svc_fini()
*/

#include "stats/stats_fops.h"
#include "reqh/reqh.h"
#include "reqh/reqh_service.h"
#include "fop/fom_generic.h"

/**
 * @defgroup stats_svc Stats Service
 * @{
 */

extern struct m0_reqh_service_type m0_stats_svc_type;

/**
 * In-memory representation of statistical information
 */
struct m0_stats {
	uint64_t	    s_magic;
	/** Linkage to list of global stats object */
	struct m0_tlink     s_linkage;
	/** Summary record represent statistics */
	struct m0_stats_sum s_sum;
};

/**
 * Stats service.
 */
struct stats_svc {
	uint64_t	       ss_magic;
	/**
	 * List of statistic objects.
	 * Initially this list does not contains any stats object. As
	 * stats_update request is recieved, stats_update FOM creates new entry
	 * for respective stats otherwise update existing stats object.
	 */
	struct m0_tl           ss_stats;
	/** Embedded request handler service object. */
	struct m0_reqh_service ss_reqhs;
};

/**
 * Stats update fom.
 */
struct stats_update_fom {
	uint64_t      suf_magic;
	struct m0_fom suf_fom;
};

enum stats_update_fom_phases {
	STATS_UPDATE_FOM_INIT = M0_FOPH_INIT,
	STATS_UPDATE_FOM_FINISH = M0_FOPH_FINISH,
	STATS_UPDATE_FOM_UPDATE_OBJECT = M0_FOPH_NR + 1,
};

/*
 * Stats query fom.
 */
struct stats_query_fom {
	uint64_t      sqf_magic;
	struct m0_fom sqf_fom;
};

enum stats_query_fom_phases {
	STATS_QUERY_FOM_READ_OBJECT = M0_FOPH_NR + 1,
};

/**
 * Initialize stats service.
 */
M0_INTERNAL int m0_stats_svc_init(void);

/**
 * Finalize stats service.
 */
M0_INTERNAL void m0_stats_svc_fini(void);

M0_INTERNAL struct m0_stats *
m0_stats_get(struct m0_tl *stats_list, uint64_t id);

#define STATS_SVC_ALLOC_PTR(ptr) M0_ALLOC_PTR(ptr)

/** @} end group stats_service */

#endif /* __KERNEL__ */
#endif /* __MOTR_STATS_STATS_SVC_H_ */
/*
 *  Local variables:
 *  c-indentation-style: "K&R"
 *  c-basic-offset: 8
 *  tab-width: 8
 *  fill-column: 80
 *  scroll-step: 1
 *  End:
 */
