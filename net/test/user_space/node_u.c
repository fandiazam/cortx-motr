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


#include <stdio.h>			/* printf */
#include <string.h>			/* strlen */

#include "lib/getopts.h"		/* m0_getopts */
#include "lib/errno.h"			/* EINVAL */
#include "lib/memory.h"			/* m0_alloc */
#include "lib/semaphore.h"		/* m0_semaphore_down */

#include "motr/init.h"                  /* m0_init */
#include "module/instance.h"            /* m0 */

#include "net/test/user_space/common_u.h" /* m0_net_test_u_str_copy */
#include "net/test/node.h"
#include "net/test/initfini.h"		/* m0_net_test_init */

/**
   @page net-test-fspec-cli-node-user Test node command line pamameters
   @verbatim
   $ net/test/m0nettestd.sh -?
   Usage: m0nettestd options...

   where valid options are

	 -a     string: Test node commands endpoint
	 -c     string: Test console commands endpoint
	 -v           : Verbose output
	 -l           : List available LNET interfaces
	 -?           : display this help and exit
   @endverbatim
 */

/**
   @defgroup NetTestUNodeInternals Test Node user-space program
   @ingroup NetTestInternals

   @see @ref net-test

   @{
 */

static bool config_check(struct m0_net_test_node_cfg *cfg)
{
	return cfg->ntnc_addr != NULL && cfg->ntnc_addr_console != NULL;
}

static void config_free(struct m0_net_test_node_cfg *cfg)
{
	m0_net_test_u_str_free(cfg->ntnc_addr);
	m0_net_test_u_str_free(cfg->ntnc_addr_console);
}

static void config_print(struct m0_net_test_node_cfg *cfg)
{
	m0_net_test_u_print_s("cfg->ntnc_addr\t\t= %s\n", cfg->ntnc_addr);
	m0_net_test_u_print_s("cfg->ntnc_addr_console\t= %s\n",
			      cfg->ntnc_addr_console);
	m0_net_test_u_print_time("cfg->ntnc_send_timeout",
				 cfg->ntnc_send_timeout);
}

static int configure(int argc, char *argv[], struct m0_net_test_node_cfg *cfg)
{
	bool list_if = false;

	M0_GETOPTS("m0nettestd", argc, argv,
		M0_STRINGARG('a', "Test node commands endpoint",
		LAMBDA(void, (const char *addr) {
			cfg->ntnc_addr = m0_net_test_u_str_copy(addr);
		})),
		M0_STRINGARG('c', "Test console commands endpoint",
		LAMBDA(void, (const char *addr) {
			cfg->ntnc_addr_console = m0_net_test_u_str_copy(addr);
		})),
		M0_VERBOSEFLAGARG,
		M0_IFLISTARG(&list_if),
		M0_HELPARG('?'),
		);
	if (!list_if)
		config_print(cfg);
	return list_if ? 1 : config_check(cfg) ? 0 : -1;
}

int main(int argc, char *argv[])
{
	static struct m0 instance;

	int rc;
	struct m0_net_test_node_ctx node;
	struct m0_net_test_node_cfg cfg = {
		.ntnc_addr	   = NULL,
		.ntnc_addr_console = NULL,
		/** @todo add to command line parameters */
		.ntnc_send_timeout = M0_MKTIME(3, 0)
	};

	m0_net_test_u_printf_v("m0_init()\n");
	rc = m0_init(&instance);
	m0_net_test_u_print_error("Motr initialization failed.", rc);
	if (rc != 0)
		return rc;

	rc = m0_net_test_init();
	m0_net_test_u_print_error("Net-test initialization failed", rc);
	if (rc != 0)
		goto motr_fini;

	/** @todo add Ctrl+C handler
	   m0_net_test_fini()+m0_net_test_config_fini() */
	/** @todo atexit() */
	rc = configure(argc, argv, &cfg);
	if (rc != 0) {
		if (rc == 1) {
			m0_net_test_u_lnet_info();
			rc = 0;
		} else {
			/** @todo where is the error */
			m0_net_test_u_printf("Error in configuration.\n");
			config_free(&cfg);
		}
		goto net_test_fini;
	}

	m0_net_test_u_printf_v("m0_net_test_node_init()\n");
	rc = m0_net_test_node_init(&node, &cfg);
	m0_net_test_u_print_error("Test node initialization failed.", rc);
	if (rc != 0)
		goto cfg_free;

	m0_net_test_u_printf_v("m0_net_test_node_start()\n");
	rc = m0_net_test_node_start(&node);
	m0_net_test_u_print_error("Test node start failed.", rc);
	if (rc != 0)
		goto node_fini;

	m0_net_test_u_printf_v("waiting...\n");
	m0_semaphore_down(&node.ntnc_thread_finished_sem);

	m0_net_test_u_printf_v("m0_net_test_node_stop()\n");
	m0_net_test_node_stop(&node);
node_fini:
	m0_net_test_u_printf_v("m0_net_test_node_fini()\n");
	m0_net_test_node_fini(&node);
cfg_free:
	config_free(&cfg);
net_test_fini:
	m0_net_test_fini();
motr_fini:
	m0_net_test_u_printf_v("m0_fini()\n");
	m0_fini();

	return rc;
}

/**
   @} end of NetTestUNodeInternals group
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
