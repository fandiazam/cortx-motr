/* -*- C -*- */
/*
 * Copyright (c) 2017-2020 Seagate Technology LLC and/or its Affiliates
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
#ifndef __MOTR_FDMI_MODULE_H__
#define __MOTR_FDMI_MODULE_H__

#include "module/module.h"
#include "rpc/conn_pool.h"
#include "fdmi/fol_fdmi_src.h"

/** Levels of m0_fdmi_module::fdm_module. */
enum {
	M0_LEVEL_FDMI
};

struct m0_fdmi_module_source {
	/** FOL source internal context. */
	struct m0_fol_fdmi_src_ctx   fdms_ffs_ctx;
	/**
	 * List of all locked transactions.
	 * Entries are linked by m0_be_tx::t_fdmi_linkage.
	 */
	struct m0_tl                 fdms_ffs_locked_tx_list;
	struct m0_mutex              fdms_ffs_locked_tx_lock;
};

struct m0_fdmi_module_plugin {
	struct m0_tl                 fdmp_fdmi_filters;
	struct m0_mutex              fdmp_fdmi_filters_lock;

	struct m0_tl                 fdmp_fdmi_recs;
	struct m0_mutex              fdmp_fdmi_recs_lock;

	bool                         fdmp_dock_inited;
	/**
	 * Connection pool stores connects for
	 * posting release request to the source.
	 */
	struct m0_rpc_conn_pool      fdmp_conn_pool;
};

struct m0_fdmi_module {
	struct m0_module             fdm_module;
	struct m0_fdmi_module_source fdm_s;
	struct m0_fdmi_module_plugin fdm_p;
};

M0_INTERNAL struct m0_fdmi_module *m0_fdmi_module__get(void);

#endif /* __MOTR_FDMI_MODULE_H__ */

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
