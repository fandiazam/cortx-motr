/* -*- C -*- */
/*
 * Copyright (c) 2015-2020 Seagate Technology LLC and/or its Affiliates
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


#define M0_TRACE_SUBSYSTEM M0_TRACE_SUBSYS_SPIEL
#include "lib/trace.h"

#include "lib/tlist.h"
#include "lib/memory.h"
#include "lib/string.h"        /* m0_strdup, m0_strings_dup, m0_strings_free */
#include "lib/errno.h"
#include "lib/finject.h"       /* M0_FI_ENABLED */
#include "lib/locality.h"      /* m0_locality0_get */
#include "fid/fid.h"           /* m0_fid_sscanf */
#include "conf/obj.h"

#include "reqh/reqh.h"
#include "spiel/spiel.h"
#include "spiel/spiel_internal.h"

/**
 * @defgroup spiel-api-fspec-intr Spiel API Internals
 * @{
 */

int m0_spiel_init(struct m0_spiel *spiel, struct m0_reqh *reqh)
{
	M0_PRE(reqh != NULL);
	M0_ENTRY("spiel=%p reqh=%p", spiel, reqh);

	M0_SET0(spiel);
	spiel->spl_core.spc_rmachine =
		m0_reqh_rpc_mach_tlist_head(&reqh->rh_rpc_machines);
	if (spiel->spl_core.spc_rmachine == NULL)
		return M0_ERR(-ENOENT);
	spiel->spl_core.spc_confc = &spiel->spl_rconfc.rc_confc;
	return M0_RC(0);
}
M0_EXPORTED(m0_spiel_init);

void m0_spiel_fini(struct m0_spiel *spiel)
{
	M0_ENTRY("spiel=%p", spiel);
	M0_LEAVE();
}
M0_EXPORTED(m0_spiel_fini);

int m0_spiel_rconfc_start(struct m0_spiel *spiel,
			  m0_rconfc_cb_t   expired_cb)
{
	int               rc;
	struct m0_rconfc *rconfc = &spiel->spl_rconfc;

	M0_ENTRY();
	M0_PRE(spiel_rmachine(spiel) != NULL);
	M0_PRE(m0_fid_is_set(spiel_profile(spiel)));

	rc = m0_rconfc_init(rconfc, &spiel->spl_core.spc_profile,
			    m0_locality0_get()->lo_grp, spiel_rmachine(spiel),
			    expired_cb, NULL);
	if (rc != 0)
		return M0_ERR(rc);

	rc = m0_rconfc_start_sync(rconfc);
	if (rc != 0) {
		m0_rconfc_stop_sync(rconfc);
		m0_rconfc_fini(rconfc);
	}
	return M0_RC(rc);
}
M0_EXPORTED(m0_spiel_rconfc_start);

void m0_spiel_rconfc_stop(struct m0_spiel *spiel)
{
	M0_ENTRY();
	m0_rconfc_stop_sync(&spiel->spl_rconfc);
	m0_rconfc_fini(&spiel->spl_rconfc);
	M0_LEAVE();
}
M0_EXPORTED(m0_spiel_rconfc_stop);

int m0_spiel_cmd_profile_set(struct m0_spiel *spiel, const char *profile_str)
{
	M0_ENTRY("spiel = %p, profile = %s", spiel, profile_str);
	M0_PRE(spiel != NULL);
	if (profile_str == NULL)
		profile_str = "<0:0>";
	return M0_RC(m0_fid_sscanf(profile_str, spiel_profile(spiel)));
}
M0_EXPORTED(m0_spiel_cmd_profile_set);

/** @} */
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
