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


#include "lib/memory.h"
#include "lib/misc.h"
#include "lib/types.h"
#include "lib/refs.h"
#include "motr/magic.h"
#include "sm/sm.h"
#include "fop/fop.h"
#include "fop/fom.h"
#include "fop/fop_item_type.h"
#include "fop/fom_generic.h"
#include "rpc/rpc_opcodes.h"
#include "rpc/item.h"
#include "sss/ss_svc.h"
#include "sss/ss_fops.h"
#include "sss/ss_fops_xc.h"

struct m0_fop_type m0_fop_ss_fopt;
struct m0_fop_type m0_fop_ss_rep_fopt;

extern struct m0_sm_state_descr     ss_fom_phases[];
M0_EXTERN struct m0_sm_conf         ss_fom_conf;
extern const struct m0_fom_type_ops ss_fom_type_ops;
const struct m0_fop_type_ops        ss_fop_type_ops;

static const struct m0_rpc_item_type_ops ss_item_type_ops = {
	M0_FOP_DEFAULT_ITEM_TYPE_OPS,
};

M0_INTERNAL int m0_ss_fops_init(void)
{
#ifndef __KERNEL__
	m0_sm_conf_extend(m0_generic_conf.scf_state, ss_fom_phases,
			  m0_generic_conf.scf_nr_states);

	/*
	 * Extend phases this FOM needs to open separate BE transaction.
	 * Because extend phases execute before M0_FOPH_TXN_INIT phase.
	 * FOM handler catch M0_FOPH_TXN_INIT phase. If it first time then
	 * move to extended phase else simple way.
	 */
	ss_fom_phases[M0_FOPH_TXN_INIT].sd_allowed |= M0_BITS(SS_FOM_SWITCH);

#endif
	m0_fop_ss_rep_fopt.ft_magix = 0;
	M0_FOP_TYPE_INIT(&m0_fop_ss_fopt,
			 .name      = "Start Stop fop",
			 .opcode    = M0_SSS_SVC_REQ_OPCODE,
			 .xt        = m0_sss_req_xc,
			 .rpc_flags = M0_RPC_ITEM_TYPE_REQUEST,
			 .fop_ops   = &ss_fop_type_ops,
			 .fom_ops   = &ss_fom_type_ops,
			 .sm        = &ss_fom_conf,
			 .svc_type  = &m0_ss_svc_type,
			 .rpc_ops   = &ss_item_type_ops);
	M0_FOP_TYPE_INIT(&m0_fop_ss_rep_fopt,
			 .name      = "Start Stop reply fop",
			 .opcode    = M0_SSS_SVC_REP_OPCODE,
			 .xt        = m0_sss_rep_xc,
			 .rpc_flags = M0_RPC_ITEM_TYPE_REPLY,
			 .fop_ops   = &ss_fop_type_ops,
			 .fom_ops   = &ss_fom_type_ops,
			 .sm        = &ss_fom_conf,
			 .svc_type  = &m0_ss_svc_type,
			 .rpc_ops   = &ss_item_type_ops);
	return 0;
}

M0_INTERNAL void m0_ss_fops_fini(void)
{
	m0_fop_type_fini(&m0_fop_ss_fopt);
	m0_fop_type_fini(&m0_fop_ss_rep_fopt);
}

M0_INTERNAL void m0_ss_fop_release(struct m0_ref *ref)
{
	struct m0_fop *fop = container_of(ref, struct m0_fop, f_ref);

	M0_PRE(fop != NULL);

	m0_free(fop->f_data.fd_data);
	fop->f_data.fd_data = NULL;

	m0_fop_fini(fop);
	m0_free(fop);
}

/*
 *  Local variables:
 *  c-indentation-style: "K&R"
 *  c-basic-offset: 8
 *  tab-width: 8
 *  fill-column: 80
 *  scroll-step: 1
 *  End:
 */
