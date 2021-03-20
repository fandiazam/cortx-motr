/* -*- C -*- */
/*
 * Copyright (c) 2020 Seagate Technology LLC and/or its Affiliates
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


#include "lib/errno.h"
#include "fid/fid.h"               /* m0_fid */
#include "lib/locality.h"          /* m0_locality_here() */

#include "motr/client.h"
#include "motr/client_internal.h"
#include "motr/layout.h"
#include "dix/layout.h"
#include "cob/cob.h"

#define M0_TRACE_SUBSYSTEM M0_TRACE_SUBSYS_CLIENT
#include "lib/trace.h"
#include "lib/finject.h"

const struct m0_bob_type ol_bobtype;
M0_BOB_DEFINE(M0_INTERNAL, &ol_bobtype,  m0_op_layout);
const struct m0_bob_type ol_bobtype = {
	.bt_name         = "ol_bobtype",
	.bt_magix_offset = offsetof(struct m0_op_layout, ol_magic),
	.bt_magix        = M0_OL_MAGIC,
	.bt_check        = NULL,
};

static const struct m0_client_layout_ops layout_pdclust_ops;
static const struct m0_client_layout_ops layout_capture_ops;

/**----------------------------------------------------------------------------*
 *                          Routines for pdclust layout                        *
 *-----------------------------------------------------------------------------*/
static int pdclust_layout_get(struct m0_client_layout *layout)
{
	return 0;
}

static void pdclust_layout_put(struct m0_client_layout *layout)
{
	return;
}

static int pdclust_layout_alloc(struct m0_client_layout **out)
{
	struct m0_client_layout *layout;

	M0_ENTRY();

	*out = NULL;
	layout = m0_alloc(sizeof(struct m0_client_pdclust_layout));
	if (layout == NULL)
		return M0_ERR(-ENOMEM);
	layout->ml_type = M0_LT_PDCLUST;
	layout->ml_ops = &layout_pdclust_ops;
	*out = layout;

	return M0_RC(0);
}

static const struct m0_client_layout_ops layout_pdclust_ops = {
	.lo_alloc     = pdclust_layout_alloc,
	.lo_get       = pdclust_layout_get,
	.lo_put       = pdclust_layout_put,
	.lo_io_build  = m0__obj_io_build
};

static int pdclust_layout_copy_to_app(struct m0_client_layout *to,
				      void  *from)
{
	struct m0_cob_attr              *attr;
	struct m0_client_pdclust_layout *pdclust;

	M0_PRE(from != NULL);
	M0_PRE(to != NULL);

	attr = (struct m0_cob_attr*)from;
	pdclust = M0_AMB(pdclust, to, pl_layout);
	pdclust->pl_pver = attr->ca_pver;
	pdclust->pl_lid  = attr->ca_lid;
	pdclust->pl_fid  = attr->ca_tfid;

	return M0_RC(0);
}

static int pdclust_layout_copy_from_app(struct m0_client_layout *from,
					void  *to)
{
	struct m0_cob_attr              *attr;
	struct m0_client_pdclust_layout *pdclust;

	M0_PRE(from != NULL);

	attr = (struct m0_cob_attr*)to;
	pdclust = M0_AMB(pdclust, from, pl_layout);
	/* Only layout id is allowed to be changed. */
	attr->ca_lid = pdclust->pl_lid;

	return M0_RC(0);
}

static int pdclust_layout_launch(struct m0_op_layout *ol)
{
	struct m0_obj *obj;

	obj = m0__obj_entity(ol->ol_entity);
	return m0__obj_layout_send(obj, ol);
}

static const struct m0_op_layout_ops m0_op_layout_pdclust_ops = {
	.olo_launch         = pdclust_layout_launch,
	.olo_copy_to_app    = pdclust_layout_copy_to_app,
	.olo_copy_from_app  = pdclust_layout_copy_from_app,
};

/**----------------------------------------------------------------------------*
 *                          Routines for capture layout                       *
 *-----------------------------------------------------------------------------*/

int m0_client_layout_capture(struct m0_client_layout  *layout,
		             struct m0_obj            *obj,
		             struct m0_client_layout **out)
{
	struct m0_client_pdclust_layout *pdlayout;
	struct m0_capture_layout        *cap_layout;

	M0_PRE(obj != NULL);
	M0_PRE(layout->ml_type == M0_LT_PDCLUST);
	pdlayout = M0_AMB(pdlayout, layout, pl_layout);
	M0_PRE(m0_fid_eq(&pdlayout->pl_pver, &obj->ob_attr.oa_pver));
	M0_PRE(pdlayout->pl_lid == obj->ob_attr.oa_layout_id);

	*out = m0_client_layout_alloc(M0_LT_CAPTURE);
	if (*out == NULL)
		return M0_ERR(-ENOMEM);
	cap_layout = M0_AMB(cap_layout, *out, cl_layout);
	cap_layout->cl_pver    = pdlayout->pl_pver;
	cap_layout->cl_lid     = pdlayout->pl_lid;
	cap_layout->cl_orig_id = obj->ob_entity.en_id;
	return M0_RC(0);
}
M0_EXPORTED(m0_client_layout_capture);

static int capture_layout_copy_to_app(struct m0_client_layout *to, void *from)
{
	struct m0_dix_layout            *dix_layout;
	struct m0_dix_capture_ldesc     *dix_cap_ldesc;
	struct m0_capture_layout        *cap_layout;

	M0_ENTRY();
	M0_PRE(to != NULL);

	cap_layout = M0_AMB(cap_layout, to, cl_layout);
	dix_layout = (struct m0_dix_layout *)from;
	dix_cap_ldesc = &dix_layout->u.dl_cap_desc;
	cap_layout->cl_orig_id = dix_cap_ldesc->ca_orig_id;
	cap_layout->cl_pver = dix_cap_ldesc->ca_pver;
	cap_layout->cl_lid = dix_cap_ldesc->ca_lid;

	return M0_RC(0);
}

static int capture_layout_copy_from_app(struct m0_client_layout *from,
					void  *to)
{
	struct m0_dix_layout            *dix_layout;
	struct m0_dix_capture_ldesc     *dix_cap_ldesc;
	struct m0_capture_layout *cap_layout;

	M0_ENTRY();
	M0_PRE(from != NULL);

	cap_layout = M0_AMB(cap_layout, from, cl_layout);
	dix_layout = (struct m0_dix_layout *)to;
	dix_layout->dl_type = DIX_LTYPE_CAPTURE_DESCR;
	dix_cap_ldesc = &dix_layout->u.dl_cap_desc;
	dix_cap_ldesc->ca_orig_id = cap_layout->cl_orig_id;
	dix_cap_ldesc->ca_pver = cap_layout->cl_pver;
	dix_cap_ldesc->ca_lid = cap_layout->cl_lid;

	return M0_RC(0);
}

static const struct m0_op_layout_ops m0_op_layout_capture_ops = {
	.olo_launch   = m0_layout_op_launch,
	.olo_copy_to_app = capture_layout_copy_to_app,
	.olo_copy_from_app = capture_layout_copy_from_app,
};

static int capture_layout_get_sync(struct m0_obj *obj)
{
	struct m0_dix_layout dlayout;

	M0_ENTRY();
	m0__dix_layout_get_sync(obj, &dlayout);
	return M0_RC(capture_layout_copy_to_app(obj->ob_layout, &dlayout));
}

static int capture_layout_get(struct m0_client_layout *layout)
{
	struct m0_obj *obj;

	M0_ENTRY();

	/* Retrieve layout for the object this layout associated with. */
	obj = layout->ml_obj;
	return M0_RC(capture_layout_get_sync(obj));
}

static void capture_layout_put(struct m0_client_layout *layout)
{
	struct m0_capture_layout *cap_layout;

	cap_layout = M0_AMB(cap_layout, layout, cl_layout);
	M0_SET0(cap_layout);
}

static int capture_layout_io_build(struct m0_io_args *args,
				   struct m0_op **op)
{
	struct m0_obj            *obj;
	struct m0_capture_layout *cap_layout;

	obj = args->ia_obj;
	cap_layout = M0_AMB(cap_layout, obj->ob_layout, cl_layout);
	/* Navigate IO to original object. */
	obj->ob_entity.en_id = cap_layout->cl_orig_id;
	return M0_RC(m0__obj_io_build(args, op));
};

static int capture_layout_alloc(struct m0_client_layout **out)
{
	struct m0_client_layout *layout;

	M0_ENTRY();

	*out = NULL;
	layout = m0_alloc(sizeof(struct m0_capture_layout));
	if (layout == NULL)
		return M0_ERR(-ENOMEM);
	layout->ml_type = M0_LT_CAPTURE;
	layout->ml_ops = &layout_capture_ops;
	*out = layout;

	return M0_RC(0);
}

static const struct m0_client_layout_ops layout_capture_ops = {
        .lo_alloc    = capture_layout_alloc,
        .lo_get      = capture_layout_get,
        .lo_put      = capture_layout_put,
        .lo_io_build = capture_layout_io_build,
};

/**----------------------------------------------------------------------------*
 *                           Client LAYOUT APIS                                *
 *-----------------------------------------------------------------------------*/
M0_INTERNAL int m0_client__layout_get(struct m0_client_layout *layout)
{
	int rc = 0;

	M0_ENTRY();
	M0_PRE(layout != NULL);

	if (layout->ml_ops->lo_get != NULL)
		rc = layout->ml_ops->lo_get(layout);
	return M0_RC(rc);
}

M0_INTERNAL void m0_client__layout_put(struct m0_client_layout *layout)
{
	M0_ENTRY();
	M0_PRE(layout != NULL);

	if (layout->ml_ops->lo_put != NULL)
		layout->ml_ops->lo_put(layout);

	M0_LEAVE();
}

/**
 * Checks a layout operation is not malformed or corrupted.
 */
static bool layout_op_invariant(struct m0_op_layout *ol)
{
	return M0_RC(ol != NULL &&
		     m0_op_layout_bob_check(ol) &&
		     ol->ol_oc.oc_op.op_size >= sizeof *ol &&
		     ol->ol_ops != NULL &&
		     m0_ast_rc_bob_check(&ol->ol_ar) &&
		     m0_op_common_bob_check(&ol->ol_oc));
}

/*
 * Callback for an LAYOUT operation being finalised.
 */
static void layout_op_cb_fini(struct m0_op_common *oc)
{
	struct m0_op_layout *ol;

	M0_ENTRY();

	M0_PRE(oc != NULL);
	M0_PRE(oc->oc_op.op_size >= sizeof *ol);

	ol = bob_of(oc, struct m0_op_layout, ol_oc, &ol_bobtype);
	M0_PRE(layout_op_invariant(ol));

	m0_op_common_bob_fini(&ol->ol_oc);
	m0_ast_rc_bob_fini(&ol->ol_ar);
	m0_op_layout_bob_fini(ol);

	M0_LEAVE();
}

/*
 * 'free entry' on the operations vector for LAYOUT operations.
 */
static void layout_op_cb_free(struct m0_op_common *oc)
{
	struct m0_op_layout *ol;

	M0_ENTRY();

	M0_PRE(oc != NULL);
	M0_PRE((oc->oc_op.op_size >= sizeof *ol));

	/* By now, fini() has been called and bob_of cannot be used */
	ol = M0_AMB(ol, oc, ol_oc);
	m0_free(ol);

	M0_LEAVE();
}

/*
 * Callback for an LAYOUT operation being launched.
 */
static void layout_op_cb_launch(struct m0_op_common *oc)
{
	int                         rc;
	struct m0_op        *op = &oc->oc_op;
	struct m0_op_layout *ol = bob_of(oc, struct m0_op_layout,
					 ol_oc, &ol_bobtype);

	M0_ENTRY();
	M0_PRE(oc != NULL);
	M0_PRE(M0_IN(op->op_code, (M0_EO_LAYOUT_GET,
				   M0_EO_LAYOUT_SET)));
	M0_PRE(m0_sm_group_is_locked(&op->op_sm_group));
	M0_PRE(layout_op_invariant(ol));

	rc = ol->ol_ops->olo_launch(ol);
	if (rc == 0)
		m0_sm_move(&op->op_sm, 0, M0_OS_LAUNCHED);
	else {
		/* m0_op_launch() has held the lock. */
		m0_sm_fail(&op->op_sm, M0_OS_FAILED, rc);
		m0_op_failed(op);
	}
	M0_LEAVE();
}

static int layout_op_init(struct m0_obj                  *obj,
				 struct m0_client_layout *layout,
				 enum m0_entity_opcode    opcode,
				 struct m0_op            *op)
{
	int                  rc;
	struct m0_op_common *oc;
	struct m0_op_layout *ol;
	struct m0_locality  *locality;

	M0_ENTRY();

	op->op_code = opcode;
	rc = m0_op_init(op, &m0_op_conf, &obj->ob_entity);
	if (rc != 0)
		return M0_RC(rc);
	/*
	 * Initialise m0_op_common part.
	 */
	oc = M0_AMB(oc, op, oc_op);
	m0_op_common_bob_init(oc);
	oc->oc_cb_launch = layout_op_cb_launch;
	oc->oc_cb_fini   = layout_op_cb_fini;
	oc->oc_cb_free   = layout_op_cb_free;

	/* Initialise the rest of m0_op_layout. */
	ol = M0_AMB(ol, oc, ol_oc);
	ol->ol_entity = &obj->ob_entity;
	ol->ol_layout = layout;
	switch(ol->ol_layout->ml_type) {
	case M0_LT_PDCLUST:
		ol->ol_ops = &m0_op_layout_pdclust_ops;
		break;
	case M0_LT_CAPTURE:
		ol->ol_ops = &m0_op_layout_capture_ops;
		break;
	case M0_LT_COMPOSITE:
		ol->ol_ops = &m0_op_layout_composite_ops;
		break;
	default:
		M0_IMPOSSIBLE("layout type not implemented");
	}

	/* Pick a locality thread for this op. */
	locality = m0__locality_pick(NULL);
	M0_ASSERT(locality != NULL);
	ol->ol_sm_grp = locality->lo_grp;
	M0_SET0(&ol->ol_ar);

	m0_op_layout_bob_init(ol);
	m0_ast_rc_bob_init(&ol->ol_ar);

	return M0_RC(0);
}

int m0_client_layout_op(struct m0_obj *obj,
		 enum m0_entity_opcode opcode,
		 struct m0_client_layout *layout,
		 struct m0_op **op)
{
	int                  rc = 0;
	struct m0_op_common *oc;
	struct m0_op_layout *ol;

	M0_ENTRY();

	M0_PRE(layout != NULL);
	M0_PRE(op != NULL);
	M0_PRE(M0_IN(opcode, (M0_EO_LAYOUT_GET,
			      M0_EO_LAYOUT_SET)));

	rc = m0_op_get(op, sizeof(struct m0_op_layout))?:
	     layout_op_init(obj, layout, opcode, *op);
	if (rc != 0)
		goto error;

	M0_POST(rc == 0);
	M0_POST(*op != NULL);
	oc = bob_of(*op, struct m0_op_common, oc_op, &oc_bobtype);
	ol = bob_of(oc, struct m0_op_layout, ol_oc, &ol_bobtype);
	M0_POST(layout_op_invariant(ol));
	m0_sm_group_lock(&(*op)->op_sm_group);
	M0_POST((*op)->op_sm.sm_rc == 0);
	m0_sm_group_unlock(&(*op)->op_sm_group);

	return M0_RC(0);
error:
	M0_ASSERT(rc != 0);
	return M0_ERR(rc);
}
M0_EXPORTED(m0_client_layout_op);

struct m0_client_layout *m0_client_layout_alloc(enum m0_client_layout_type type)
{
	int                                rc;
	struct m0_client_layout           *layout;
	const struct m0_client_layout_ops *ops;

	M0_ENTRY();
	M0_PRE(type >= 0 && type < M0_LT_NR);

	switch(type) {
	case M0_LT_PDCLUST:
		ops = &layout_pdclust_ops;
		break;
	case M0_LT_COMPOSITE:
		ops = &layout_composite_ops;
		break;
	case M0_LT_CAPTURE:
		ops = &layout_capture_ops;
		break;
	default:
		M0_IMPOSSIBLE("Layout type not supported yet.");
	}
	rc = ops->lo_alloc(&layout);
	if (rc != 0)
		M0_ASSERT(layout == NULL);

	M0_LEAVE();
	return layout;
}
M0_EXPORTED(m0_client_layout_alloc);

void m0_client_layout_free(struct m0_client_layout *layout)
{
	m0_free(layout);
}
M0_EXPORTED(m0_client_layout_free);

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
