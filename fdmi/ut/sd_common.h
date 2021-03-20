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

#ifndef __MOTR_FDMI_UT_FDMI_SD_COMMON_H__
#define __MOTR_FDMI_UT_FDMI_SD_COMMON_H__

#include "rpc/formation2_internal.h" /* m0_rpc_frm_ops */

#include "fdmi/filterc.h"

#define _QUOTE(s) #s
#define QUOTE(s) _QUOTE(s)

/* See "fdmi/ut/Makefile.sub" for M0_FDMI_UT_DIR */
#define M0_FDMI_UT_PATH(name)   QUOTE(M0_FDMI_UT_DIR) "/" name

void fdmi_serv_start_ut(const struct m0_filterc_ops *filterc_ops);
void fdmi_serv_stop_ut(void);

struct fdmi_sd_ut_ctx {
	struct m0_motr          motr;
	/* Actually not used, but added to req->rh_rpc_machines
	 * to pass some checks */
	struct m0_rpc_machine   rpc_machine;
	struct m0_reqh_service *fdmi_service;
};

extern struct fdmi_sd_ut_ctx g_sd_ut;

extern const struct m0_filterc_ops filterc_stub_ops;

struct test_rpc_env {
	struct m0_net_domain        tre_net_dom;
	struct m0_rpc_session      *tre_session;
	struct m0_rpc_conn         *tre_conn;
	struct m0_rpc_machine       tre_rpc_machine;
	struct m0_rpc_chan         *tre_rpc_chan;
	struct m0_net_buffer_pool   tre_buffer_pool;
	struct m0_net_end_point    *tre_ep;
	char                       *ep_addr_local;
	char                       *ep_addr_remote;
};

void prepare_rpc_env(struct test_rpc_env         *env,
	             struct m0_reqh              *reqh,
	             const struct m0_rpc_frm_ops *frm_ops,
		     bool                         sender,
		     struct m0_rpc_conn          *rpc_conn,
		     struct m0_rpc_session       *rpc_session);

void unprepare_rpc_env(struct test_rpc_env *env);

void fdmi_ut_packet_send_failed(struct m0_rpc_machine *mach,
	                        struct m0_rpc_packet *p);

#endif /* __MOTR_FDMI_UT_FDMI_SD_COMMON_H__ */

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
