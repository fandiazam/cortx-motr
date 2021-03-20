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


#include "lib/memory.h"		/* M0_ALLOC_PTR */
#include "lib/misc.h"		/* M0_SET0 */
#include "lib/errno.h"		/* ENOENT */

#include "net/test/service.h"

/**
   @defgroup NetTestServiceInternals Test Service
   @ingroup NetTestInternals

   @see
   @ref net-test

   @{
 */

/** Service state transition matrix. @see @ref net-test-lspec-state */
const static bool
state_transition[M0_NET_TEST_SERVICE_NR][M0_NET_TEST_SERVICE_NR] = {
	[M0_NET_TEST_SERVICE_UNINITIALIZED] = {
		[M0_NET_TEST_SERVICE_UNINITIALIZED] = false,
		[M0_NET_TEST_SERVICE_READY]	    = true,
		[M0_NET_TEST_SERVICE_FINISHED]	    = false,
		[M0_NET_TEST_SERVICE_FAILED]	    = false,
	},
	[M0_NET_TEST_SERVICE_READY] = {
		[M0_NET_TEST_SERVICE_UNINITIALIZED] = true,
		[M0_NET_TEST_SERVICE_READY]	    = false,
		[M0_NET_TEST_SERVICE_FINISHED]	    = true,
		[M0_NET_TEST_SERVICE_FAILED]	    = true,
	},
	[M0_NET_TEST_SERVICE_FINISHED] = {
		[M0_NET_TEST_SERVICE_UNINITIALIZED] = true,
		[M0_NET_TEST_SERVICE_READY]	    = false,
		[M0_NET_TEST_SERVICE_FINISHED]	    = false,
		[M0_NET_TEST_SERVICE_FAILED]	    = false,
	},
	[M0_NET_TEST_SERVICE_FAILED] = {
		[M0_NET_TEST_SERVICE_UNINITIALIZED] = true,
		[M0_NET_TEST_SERVICE_READY]	    = false,
		[M0_NET_TEST_SERVICE_FINISHED]	    = false,
		[M0_NET_TEST_SERVICE_FAILED]	    = false,
	},
};

int m0_net_test_service_init(struct m0_net_test_service *svc,
			     struct m0_net_test_service_ops *ops)
{
	M0_PRE(svc != NULL);
	M0_PRE(ops != NULL);

	M0_SET0(svc);
	svc->nts_ops = ops;

	svc->nts_svc_ctx = svc->nts_ops->ntso_init(svc);
	if (svc->nts_svc_ctx != NULL)
		m0_net_test_service_state_change(svc,
				M0_NET_TEST_SERVICE_READY);

	M0_POST(ergo(svc->nts_svc_ctx != NULL,
		     m0_net_test_service_invariant(svc)));

	return svc->nts_errno;
}

void m0_net_test_service_fini(struct m0_net_test_service *svc)
{
	M0_PRE(m0_net_test_service_invariant(svc));
	M0_PRE(svc->nts_state != M0_NET_TEST_SERVICE_UNINITIALIZED);

	svc->nts_ops->ntso_fini(svc->nts_svc_ctx);
	m0_net_test_service_state_change(svc,
			M0_NET_TEST_SERVICE_UNINITIALIZED);
}

bool m0_net_test_service_invariant(struct m0_net_test_service *svc)
{
	if (svc == NULL)
		return false;
	if (svc->nts_ops == NULL)
		return false;
	return true;
}

int m0_net_test_service_step(struct m0_net_test_service *svc)
{
	M0_PRE(m0_net_test_service_invariant(svc));
	M0_PRE(svc->nts_state == M0_NET_TEST_SERVICE_READY);

	svc->nts_errno = svc->nts_ops->ntso_step(svc->nts_svc_ctx);
	if (svc->nts_errno != 0)
		m0_net_test_service_state_change(svc,
				M0_NET_TEST_SERVICE_FAILED);

	M0_POST(m0_net_test_service_invariant(svc));
	return svc->nts_errno;
}

int m0_net_test_service_cmd_handle(struct m0_net_test_service *svc,
				   struct m0_net_test_cmd *cmd,
				   struct m0_net_test_cmd *reply)
{
	struct m0_net_test_service_cmd_handler *handler;
	int					i;

	M0_PRE(m0_net_test_service_invariant(svc));
	M0_PRE(cmd != NULL);
	M0_PRE(reply != NULL);
	M0_PRE(svc->nts_state == M0_NET_TEST_SERVICE_READY);

	svc->nts_errno = -ENOENT;
	for (i = 0; i < svc->nts_ops->ntso_cmd_handler_nr; ++i) {
		handler = &svc->nts_ops->ntso_cmd_handler[i];
		if (handler->ntsch_type == cmd->ntc_type) {
			svc->nts_errno = handler->ntsch_handler(
					 svc->nts_svc_ctx, cmd, reply);
			break;
		}
	}

	M0_POST(m0_net_test_service_invariant(svc));
	return svc->nts_errno;
}

void m0_net_test_service_state_change(struct m0_net_test_service *svc,
				      enum m0_net_test_service_state state)
{
	M0_PRE(m0_net_test_service_invariant(svc));

	M0_ASSERT(state_transition[svc->nts_state][state]);
	svc->nts_state = state;

	M0_POST(m0_net_test_service_invariant(svc));
}

enum m0_net_test_service_state
m0_net_test_service_state_get(struct m0_net_test_service *svc)
{
	M0_PRE(m0_net_test_service_invariant(svc));

	return svc->nts_state;
}

/**
   @} end of NetTestServiceInternals group
 */

/*
 *  Local variables:
 *  c-indentation-style: "K&R"
 *  c-basic-offset: 8
 *  tab-width: 8
 *  fill-column: 79
 *  scroll-step: 1
 *  End:
 */
