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


#include "lib/assert.h"  /* m0_failed_condition */
#include "lib/string.h"  /* m0_streq */
#include "ut/ut.h"

void test_0C(void)
{
	M0_UT_ASSERT(m0_failed_condition == NULL);
	M0_UT_ASSERT(!_0C(true && _0C(false)));
	M0_UT_ASSERT(m0_streq(m0_failed_condition, "false"));
	M0_UT_ASSERT(_0C(true));
	M0_UT_ASSERT(m0_failed_condition == NULL);
}
