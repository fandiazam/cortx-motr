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

#ifndef __MOTR_DIX_ENCDEC_H__
#define __MOTR_DIX_ENCDEC_H__

#include "dix/layout.h"    /* m0_dix_ldesc */
#include "dix/layout_xc.h" /* m0_dix_ldesc_xc */

/**
 * @addtogroup dix
 *
 * @{
 */

struct dix_meta_val {
	struct m0_fid       mv_fid;
	struct m0_dix_ldesc mv_dld;
} M0_XCA_RECORD;

/** @} end of dix group */
#endif /* __MOTR_DIX_ENCDEC_H__ */

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
