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


#pragma once

#ifndef __MOTR_RM_RWLOCK_H__
#define __MOTR_RM_RWLOCK_H__

#include "lib/errno.h"
#include "lib/misc.h"
#include "lib/memory.h"
#include "fid/fid.h"
#include "rm/rm.h"
#include "rm/rm_internal.h"

/**
   @page ReadWrite Distributed Lock DLD

   - @ref RWLockDLD-fspec-ds
   - @ref RWLockDLD-fspec-sub

   @section RWLockDLD-fspec Functional Specification
   This section describes the data structure and the external interfaces of
   the distributed RW lock implemented using resource manager.

   @section RWLockDLD-fspec-ds Data Structures

   The distributed RW lock will have the following data structure:
   - m0_rw_lockable
     This holds generic RM resource and pointer to the fid identifying it.
     RW lock itself is represented by @ref m0_rm_credit which can be granted for
     RW lockable resource.

   @section RWLockDLD-fspec-sub Subroutines

   The asynchronous distributed RW lock provides the functions listed in
   in the sub-sections below:

   @subsection RWLockDLD-fspec-sub-cons Constructors and Destructors

   @subsection RWLockDLD-fspec-sub-opi Operational Interfaces
   - m0_rw_lockable_init()
   - m0_rw_lockable_fini()
   - m0_rm_rwlock_owner_init()
   - m0_rm_rwlock_owner_fini()
   - m0_rm_rwlock_req_init()
   - m0_rm_rwlock_req_fini()
   - m0_rw_lockable_type_register()
   - m0_rw_lockable_type_deregister()

   Initialization and acquiring of RW lock for writing:
   @code
   m0_rw_lockable_init(lockable, fid, dom);
   m0_fid_tgenerate(&owner_fid, M0_RM_OWNER_FT);
   m0_rm_rwlock_owner_init(owner, &owner_fid, lockable, creditor);

   m0_rm_rwlock_req_init(req, owner, ops,
                         RIF_MAY_BORROW | RIF_MAY_REVOKE,
                         RM_RWLOCK_WRITE);
   m0_rm_credit_get(req);
   rc = m0_sm_timedwait(&in->rin_sm,
                        M0_BITS(RI_SUCCESS, RI_FAILURE),
                        M0_TIME_NEVER);

   ...
   m0_rm_credit_put(req);
   m0_rm_rwlock_req_fini(req)

   m0_rm_rwlock_owner_fini(owner);
   m0_rw_lockable_fini(lockable);
   @endcode

 */

/**
   @defgroup Distributed RWLock
   @ingroup rm

   @see rm
   @ref RWLockDLD-fspec "Functional Specification"

   @{
 */

enum m0_rm_rwlock_req_type {
	RM_RWLOCK_READ,
	RM_RWLOCK_WRITE
};

/**
 * Resource for which RW lock can be acquired.
 * RW lock is represented as credits for lockable resource.
 */
struct m0_rw_lockable {
	/**
	 * RW lockable resource identifier.
	 * Lockable resources are equal if their identifiers are equal.
	 */
	const struct m0_fid   *rwl_fid;
	/** Embedded RM resource */
	struct m0_rm_resource  rwl_resource;
};

M0_INTERNAL void m0_rw_lockable_init(struct m0_rw_lockable *lockable,
				     const struct m0_fid   *fid,
				     struct m0_rm_domain   *dom);

M0_INTERNAL void m0_rw_lockable_fini(struct m0_rw_lockable *lockable);

M0_INTERNAL void m0_rm_rwlock_owner_init(struct m0_rm_owner    *owner,
					 struct m0_fid         *fid,
					 struct m0_rw_lockable *lockable,
					 struct m0_rm_remote   *creditor);

M0_INTERNAL void m0_rm_rwlock_owner_fini(struct m0_rm_owner *owner);

M0_INTERNAL void m0_rm_rwlock_req_init(struct m0_rm_incoming           *req,
				       struct m0_rm_owner              *owner,
				       const struct m0_rm_incoming_ops *ops,
				       enum m0_rm_incoming_flags        flags,
				       enum m0_rm_rwlock_req_type       type);

M0_INTERNAL void m0_rm_rwlock_req_fini(struct m0_rm_incoming *req);

M0_INTERNAL int m0_rw_lockable_type_register(struct m0_rm_domain        *dom,
					     struct m0_rm_resource_type *rtype);
M0_INTERNAL
void m0_rw_lockable_type_deregister(struct m0_rm_resource_type *rtype);

/** Fid used with rwlockable resources */
extern const struct m0_fid M0_RWLOCK_FID;

/** RW lockable domain and type initialiser. */
M0_INTERNAL int
m0_rwlockable_domain_type_init(struct m0_rm_domain        *rwl_dom,
			       struct m0_rm_resource_type *rwl_rt);

/** RW lockable domain and type finaliser. */
M0_INTERNAL void
m0_rwlockable_domain_type_fini(struct m0_rm_domain        *rwl_dom,
			       struct m0_rm_resource_type *rwl_rt);

/** @} end of RWLock */

#endif /* __MOTR_RM_RWLOCK_H__ */

/*
 *  Local variables:
 *  c-indentation-style: "K&R"
 *  c-basic-offset: 8
 *  tab-width: 8
 *  fill-column: 79
 *  scroll-step: 1
 *  End:
 */
