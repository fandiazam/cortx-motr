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


#pragma once

#ifndef __MOTR_SNS_LS_SOLVE_H__
#define __MOTR_SNS_LS_SOLVE_H__

#include "matvec.h"

/**
   @defgroup m0_linsys Systems of Linear Equations Solving Algorithm:

   A systems of linear equations solving algorithm is a part of M0 core.
   It is based on Gauss method and performs the following:
   @li Solves a linear system of equations represented by (NxN) matrix and (1xN) vector, result is (1xN) vector.
   @{
 */

/**
 * Represents linear system of equations [l_mat]*[l_res]=[l_vec]
 */
struct m0_linsys {
	struct m0_matrix *l_mat;
	struct m0_matvec *l_vec;
	struct m0_matvec *l_res;
};

/**
 * @pre m0_matrix_init(mat) && m0_matvec_init(vec) && m0_vec_init(res)
 * @pre mat->m_height > 0 && mat->width > 0
 * @pre mat->m_width == mat->m_height && res->mv_size == vec->mv_size &&
 *          vec->mv_size == mat->m_width
 */
M0_INTERNAL void m0_linsys_init(struct m0_linsys *linsys,
				struct m0_matrix *mat,
				struct m0_matvec *vec, struct m0_matvec *res);

M0_INTERNAL void m0_linsys_fini(struct m0_linsys *linsys);

/**
 * Solves given system of linear equations, writes result into 'linsys->l_res'.
 */
M0_INTERNAL void m0_linsys_solve(struct m0_linsys *linsys);

/**
 * Inverts a square matrix.
 *
 * @param[in]  mat         Pointer to an input square matrix
 * @param[out] mat_inverse Pointer to the inverse of an input matrix
 * @retval     0	   matrix inversion is successful
 * @retval    -ENOMEM	   out of memory
 * @retval    -EDOM	   input matrix is singular
 * @pre m0_matrix_is_square(mat) and m0_matrix_is_square(mat_inverse)
 * @pre mat->m_width == mat_inverse->m_width
 */
M0_INTERNAL int m0_matrix_invert(const struct m0_matrix *mat,
		                 struct m0_matrix *mat_inverse);


/** @} end group m0_linsys */

/* __MOTR_SNS_LS_SOLVE_H__*/
#endif

/*
 *  Local variables:
 *  c-indentation-style: "K&R"
 *  c-basic-offset: 8
 *  tab-width: 8
 *  fill-column: 80
 *  scroll-step: 1
 *  End:
 */
