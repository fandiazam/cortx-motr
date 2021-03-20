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


#define M0_TRACE_SUBSYSTEM M0_TRACE_SUBSYS_BE
#include "lib/trace.h"

#include "be/engine.h"
#include "be/tx_service.h"
#include "lib/errno.h"
#include "lib/memory.h"

/**
 * @addtogroup be
 * @{
 */

/* ------------------------------------------------------------------
 * TX service
 * ------------------------------------------------------------------ */

/** Transaction service. */
struct tx_service {
	struct m0_reqh_service ts_reqh;
};

static int txs_allocate(struct m0_reqh_service           **out,
			const struct m0_reqh_service_type *stype);

static const struct m0_reqh_service_type_ops txs_stype_ops = {
	.rsto_service_allocate = txs_allocate
};

struct m0_reqh_service_type m0_be_txs_stype = {
	.rst_name  = "be-tx-service",
	.rst_ops   = &txs_stype_ops,
	.rst_level = M0_BE_TX_SVC_LEVEL,
};

M0_INTERNAL int m0_be_txs_register(void)
{
	return m0_reqh_service_type_register(&m0_be_txs_stype);
}

M0_INTERNAL void m0_be_txs_unregister(void)
{
	m0_reqh_service_type_unregister(&m0_be_txs_stype);
}

static int  txs_start(struct m0_reqh_service *service);
static void txs_stop(struct m0_reqh_service *service);
static void txs_fini(struct m0_reqh_service *service);

static const struct m0_reqh_service_ops txs_ops = {
	.rso_start = txs_start,
	.rso_stop  = txs_stop,
	.rso_fini  = txs_fini
};

/** Allocates and initialises transaction service. */
static int txs_allocate(struct m0_reqh_service           **service,
			const struct m0_reqh_service_type *stype)
{
	struct tx_service *s;

	M0_ENTRY();
	M0_PRE(stype == &m0_be_txs_stype);

	M0_ALLOC_PTR(s);
	if (s == NULL)
		return M0_RC(-ENOMEM);

	*service = &s->ts_reqh;
	(*service)->rs_ops = &txs_ops;

	return M0_RC(0);
}

/** Finalises and deallocates transaction service. */
static void txs_fini(struct m0_reqh_service *service)
{
	M0_ENTRY();
	m0_free(container_of(service, struct tx_service, ts_reqh));
	M0_LEAVE();
}

static int txs_start(struct m0_reqh_service *service)
{
	M0_ENTRY();
	return M0_RC(0);
}

static void txs_stop(struct m0_reqh_service *service)
{
	M0_ENTRY();
	M0_LEAVE();
}

M0_INTERNAL int m0_be_tx_service_init(struct m0_be_engine *en,
				      struct m0_reqh      *reqh)
{
	M0_PRE(en->eng_service == NULL);
	return m0_reqh_service_setup(&en->eng_service, &m0_be_txs_stype,
				     reqh, NULL, NULL);
}

M0_INTERNAL void m0_be_tx_service_fini(struct m0_be_engine *en)
{
	m0_reqh_service_quit(en->eng_service);
	en->eng_service = NULL;
}

/** @} end of be group */

#undef M0_TRACE_SUBSYSTEM

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
