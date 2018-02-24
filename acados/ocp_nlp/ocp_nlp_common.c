/*
 *    This file is part of acados.
 *
 *    acados is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    acados is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with acados; if not, write to the Free Software Foundation,
 *    Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "acados/ocp_nlp/ocp_nlp_common.h"

#include <assert.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

// blasfeo
#include "blasfeo/include/blasfeo_target.h"
#include "blasfeo/include/blasfeo_common.h"
#include "blasfeo/include/blasfeo_d_blas.h"
// hpipm
#include "hpipm/include/hpipm_d_ocp_qp_dim.h"
// acados
#include "acados/utils/mem.h"



// TODO remove !!!!!!!!!!!!1
#include "acados/sim/sim_erk_integrator.h"
#include "acados/sim/sim_irk_integrator.h"
#include "acados/sim/sim_lifted_irk_integrator.h"



/************************************************
* config
************************************************/

int ocp_nlp_solver_config_calculate_size(int N)
{
	int ii;

	int size = 0;

	size += sizeof(ocp_nlp_solver_config);

	size += 1*ocp_qp_xcond_solver_config_calculate_size();

	size += N*sizeof(sim_solver_config *);

	for (ii=0; ii<N; ii++)
		size += sim_solver_config_calculate_size();

	return size;
}



ocp_nlp_solver_config *ocp_nlp_solver_config_assign(int N, void *raw_memory)
{
	int ii;

    char *c_ptr = (char *) raw_memory;

	ocp_nlp_solver_config *config = (ocp_nlp_solver_config *) c_ptr;
	c_ptr += sizeof(ocp_nlp_solver_config);

	config->N = N;

	config->qp_solver = ocp_qp_xcond_solver_config_assign(c_ptr);
	c_ptr += ocp_qp_xcond_solver_config_calculate_size();

	config->sim_solvers = (sim_solver_config **) c_ptr;
	c_ptr += N*sizeof(sim_solver_config *);

	for (ii=0; ii<N; ii++)
	{
		config->sim_solvers[ii] = sim_solver_config_assign(c_ptr);
		c_ptr += sim_solver_config_calculate_size();
	}

	return config;
}



/************************************************
* dims
************************************************/

/* ocp_nlp_cost_ls */

int ocp_nlp_cost_ls_dims_calculate_size(int N)
{
	int size = 0;

    size += sizeof(ocp_nlp_cost_ls_dims);
    size += 2*(N+1)*sizeof(int); // nv ny

	size += 8; // initial align

	return size;
}



ocp_nlp_cost_ls_dims *ocp_nlp_cost_ls_dims_assign(int N, void *raw_memory)
{
    char *c_ptr = (char *) raw_memory;

	// initial align
	align_char_to(8, &c_ptr);

	// struct
	ocp_nlp_cost_ls_dims *dims = (ocp_nlp_cost_ls_dims *) c_ptr;
	c_ptr += sizeof(ocp_nlp_cost_ls_dims);

	// nv
    assign_int(N+1, &dims->nv, &c_ptr);
	// ny
    assign_int(N+1, &dims->ny, &c_ptr);

	// N
	dims->N = N;

	return dims;
}



void ocp_nlp_cost_ls_dims_initialize(int *nv, int *ny, ocp_nlp_cost_ls_dims *dims)
{
	int N = dims->N;

	// nv
    for (int ii = 0; ii < N+1; ii++)
		dims->nv[ii] = nv[ii];
	// ny
    for (int ii = 0; ii < N+1; ii++)
		dims->ny[ii] = ny[ii];

	return;
}



/* ocp_nlp */

int ocp_nlp_dims_calculate_size(int N)
{
	int size = 0;

    size += sizeof(ocp_nlp_dims);
    size += 8*(N+1)*sizeof(int);

	// dynamics_dims
	size += N*sizeof(sim_dims *);
	size += N*sim_dims_calculate_size();

	size += 8; // initial align

	return size;
}



ocp_nlp_dims *ocp_nlp_dims_assign(int N, void *raw_memory)
{
	int ii;

    char *c_ptr = (char *) raw_memory;

	// initial align
	align_char_to(8, &c_ptr);

	// struct
	ocp_nlp_dims *dims = (ocp_nlp_dims *) c_ptr;
	c_ptr += sizeof(ocp_nlp_dims);

	// dynamics dims
	dims->dynamics_dims = (sim_dims **) c_ptr;
	c_ptr += N*sizeof(sim_dims *);

	for (ii=0; ii<N; ii++)
	{
		dims->dynamics_dims[ii] = sim_dims_assign(c_ptr);
		c_ptr += sim_dims_calculate_size();
	}

	// nx
    assign_int(N+1, &dims->nx, &c_ptr);
	// nu
    assign_int(N+1, &dims->nu, &c_ptr);
	// nb
    assign_int(N+1, &dims->nb, &c_ptr);
	// nbx
    assign_int(N+1, &dims->nbx, &c_ptr);
	// nbu
    assign_int(N+1, &dims->nbu, &c_ptr);
	// ng
    assign_int(N+1, &dims->ng, &c_ptr);
	// nh
    assign_int(N+1, &dims->nh, &c_ptr);
	// ns
    assign_int(N+1, &dims->ns, &c_ptr);

	// N
	dims->N = N;

	return dims;
}



void ocp_nlp_dims_initialize(int *nx, int *nu, int *nbx, int *nbu, int *ng, int *nh, int *ns, void *cost_dims, ocp_nlp_dims *dims)
{
	int ii;

	int N = dims->N;

	// nx
	for (int ii = 0; ii < N+1; ii++)
		dims->nx[ii] = nx[ii];
	// nu
	for (int ii = 0; ii < N+1; ii++)
		dims->nu[ii] = nu[ii];
	// nbx
	for (int ii = 0; ii < N+1; ii++)
		dims->nb[ii] = nbx[ii]+nbu[ii];
	// nbu
	for (int ii = 0; ii < N+1; ii++)
		dims->nbx[ii] = nbx[ii];
	// nb
	for (int ii = 0; ii < N+1; ii++)
		dims->nbu[ii] = nbu[ii];
	// ng
	for (int ii = 0; ii < N+1; ii++)
		dims->ng[ii] = ng[ii];
	// nh
	for (int ii = 0; ii < N+1; ii++)
		dims->nh[ii] = nh[ii];
	// ns
	for (int ii = 0; ii < N+1; ii++)
		dims->ns[ii] = ns[ii];
	
	// TODO sim dims initialize
	for (ii=0; ii<N; ii++)
	{
		dims->dynamics_dims[ii]->nx = nx[ii];
		dims->dynamics_dims[ii]->nu = nu[ii];
	}

	// cost
	dims->cost_dims = cost_dims;

	return;
}



/************************************************
* cost
************************************************/

/* least squares */

int ocp_nlp_cost_ls_calculate_size(ocp_nlp_cost_ls_dims *dims)
{
	// extract dims
	int N = dims->N;
	int *nv = dims->nv;
	int *ny = dims->ny;

	int size = 0;

    size += sizeof(ocp_nlp_cost_ls);

	size += 1*(N+1)*sizeof(int); // nls_mask
	size += 2*(N+1)*sizeof(struct blasfeo_dmat); // W Cyt
	size += 1*(N+1)*sizeof(struct blasfeo_dvec); // y_ref
	size += 1*(N+1)*sizeof(external_function_generic *); // nls_jac

    for (int ii = 0; ii < N+1; ii++)
    {
		size += 1*blasfeo_memsize_dmat(ny[ii], ny[ii]); // W
		size += 1*blasfeo_memsize_dmat(nv[ii], ny[ii]); // Cyt
		size += 1*blasfeo_memsize_dvec(ny[ii]); // y_ref
	}

	size += 8; // initial align
	size += 8; // blasfeo_struct align
	size += 64; // blasfeo_mem align

//	make_int_multiple_of(64, &size);

	return size;
}


// TODO(dimitris): decide whether we cast it to void here
ocp_nlp_cost_ls *ocp_nlp_cost_ls_assign(ocp_nlp_cost_ls_dims *dims, void *raw_memory)
{
    char *c_ptr = (char *) raw_memory;

	// extract sizes
    int N = dims->N;
	int *nv = dims->nv;
	int *ny = dims->ny;

	// initial align
	align_char_to(8, &c_ptr);

	// struct
    ocp_nlp_cost_ls *cost = (ocp_nlp_cost_ls *) c_ptr;
    c_ptr += sizeof(ocp_nlp_cost_ls);

	// dims
	cost->dims = dims;

	// nls mask
	assign_int(N+1, &cost->nls_mask, &c_ptr);

	// blasfeo_struct align
	align_char_to(8, &c_ptr);

	// W
	assign_blasfeo_dmat_structs(N+1, &cost->W, &c_ptr);
	// Cyt
	assign_blasfeo_dmat_structs(N+1, &cost->Cyt, &c_ptr);
	// y_ref
	assign_blasfeo_dvec_structs(N+1, &cost->y_ref, &c_ptr);
	// nls_jac
	cost->nls_jac = (external_function_generic **) c_ptr;
	c_ptr += (N+1)*sizeof(external_function_generic *);

	// blasfeo_mem align
	align_char_to(64, &c_ptr);

	// blasfeo_dmat
	// W
    for (int ii = 0; ii <= N; ii++)
		assign_blasfeo_dmat_mem(ny[ii], ny[ii], cost->W+ii, &c_ptr);
	// W
    for (int ii = 0; ii <= N; ii++)
		assign_blasfeo_dmat_mem(nv[ii], ny[ii], cost->Cyt+ii, &c_ptr);

	// blasfeo_dvec
	// y_ref
    for (int ii = 0; ii <= N; ii++)
		assign_blasfeo_dvec_mem(ny[ii], cost->y_ref+ii, &c_ptr);

	// assert
    assert((char *) raw_memory + ocp_nlp_cost_ls_calculate_size(dims) >= c_ptr);

	return cost;
}


/************************************************
* constraints
************************************************/

int ocp_nlp_constraints_calculate_size(ocp_nlp_dims *dims)
{
	// extract dims
	int N = dims->N;
	int *nx = dims->nx;
	int *nu = dims->nu;
	int *nb = dims->nb;
	int *ng = dims->ng;

	int size = 0;

    size += sizeof(ocp_nlp_constraints);

    size += sizeof(int *)*(N+1);  // idxb
	size += 1*(N+1)*sizeof(struct blasfeo_dvec); // d
	size += 1*(N+1)*sizeof(struct blasfeo_dmat); // DCt

    for (int ii = 0; ii < N+1; ii++)
    {
        size += sizeof(int)*(nb[ii]);  // idxb
		size += 1*blasfeo_memsize_dvec(2*nb[ii]+2*ng[ii]); // d
		size += 1*blasfeo_memsize_dmat(nu[ii]+nx[ii], ng[ii]); // DCt
	}

	size += 8; // initial align
	size += 8; // blasfeo_struct align
	size += 64; // blasfeo_mem align

//	make_int_multiple_of(64, &size);

	return size;
}



ocp_nlp_constraints *ocp_nlp_constraints_assign(ocp_nlp_dims *dims, void *raw_memory)
{
	char *c_ptr = (char *) raw_memory;

	// extract sizes
    int N = dims->N;
	int *nx = dims->nx;
	int *nu = dims->nu;
	int *nb = dims->nb;
	int *ng = dims->ng;

	// initial align
	align_char_to(8, &c_ptr);

	// struct
    ocp_nlp_constraints *constraints = (ocp_nlp_constraints *) c_ptr;
    c_ptr += sizeof(ocp_nlp_constraints);

	// dims
	constraints->dims = dims;

	// pointers align
	align_char_to(8, &c_ptr);

    // pointers
    assign_int_ptrs(N+1, &constraints->idxb, &c_ptr);

	// blasfeo_structs
	// d
	assign_blasfeo_dvec_structs(N+1, &constraints->d, &c_ptr);
	// DCt
	assign_blasfeo_dmat_structs(N+1, &constraints->DCt, &c_ptr);

	// blasfeo_mem align
	align_char_to(64, &c_ptr);

	// blasfeo_dmat
	// DCt
    for (int ii = 0; ii <= N; ii++)
		assign_blasfeo_dmat_mem(nu[ii]+nx[ii], ng[ii], constraints->DCt+ii, &c_ptr);

	// blasfeo_dvec
	// d
    for (int ii = 0; ii <= N; ii++)
		assign_blasfeo_dvec_mem(2*nb[ii]+2*ng[ii], constraints->d+ii, &c_ptr);

    // ints
    for (int ii = 0; ii < N+1; ++ii)
    {
        assign_int(dims->nbx[ii]+dims->nbu[ii], &constraints->idxb[ii], &c_ptr);
    }

	// assert
    assert((char *) raw_memory + ocp_nlp_constraints_calculate_size(dims) >= c_ptr);

	return constraints;
}



/************************************************
* in
************************************************/

// TODO(dimitris): fix order of funs
int ocp_nlp_in_calculate_size(ocp_nlp_solver_config *config, ocp_nlp_dims *dims)
{

	int ii;

	int N = dims->N;

    int size = sizeof(ocp_nlp_in);

	size += N*sizeof(double); // Ts

    // TODO(dimitris): check arguments for cost type
	size += config->cost_calculate_size(dims->cost_dims); // cost

	// dynamics
	size += N*sizeof(void *);
	for (ii=0; ii<N; ii++)
	{
		size += config->sim_solvers[ii]->model_calculate_size(config->sim_solvers[ii], dims->dynamics_dims[ii]);
	}

	size += config->constraints_calculate_size(dims); // constraints

	size += 8; // initial align

//	make_int_multiple_of(64, &size);

    return size;
}



// TODO(dimitris): move num_stages inside args, as nested integrator args
ocp_nlp_in *ocp_nlp_in_assign(ocp_nlp_solver_config *config, ocp_nlp_dims *dims, int num_stages, void *raw_memory)
{

	int ii;

	int N = dims->N;

    char *c_ptr = (char *) raw_memory;

	// initial align
	align_char_to(8, &c_ptr);

	// struct
    ocp_nlp_in *in = (ocp_nlp_in *) c_ptr;
    c_ptr += sizeof(ocp_nlp_in);

	// dims
	in->dims = dims;

	// Ts
	in->Ts = (double *) c_ptr;
	c_ptr += N*sizeof(double);

	// cost
    // TODO(dimitris): check arguments for cost type
	in->cost = config->cost_assign(dims->cost_dims, c_ptr);
	c_ptr += config->cost_calculate_size(dims->cost_dims);

	// dynamics
	in->dynamics = (void **) c_ptr;
	c_ptr += N*sizeof(void *);
	for (ii=0; ii<N; ii++)
	{
		in->dynamics[ii] = config->sim_solvers[ii]->model_assign(config->sim_solvers[ii], dims->dynamics_dims[ii], c_ptr);
		c_ptr += config->sim_solvers[ii]->model_calculate_size(config->sim_solvers[ii], dims->dynamics_dims[ii]);
	}

	// constraints
	in->constraints = config->constraints_assign(dims, c_ptr);
	c_ptr += config->constraints_calculate_size(dims);

    assert((char *) raw_memory + ocp_nlp_in_calculate_size(config, dims) >= c_ptr);

    return in;
}



/************************************************
* out
************************************************/

int ocp_nlp_out_calculate_size(ocp_nlp_solver_config *config, ocp_nlp_dims *dims)
{
	// extract dims
    int N = dims->N;
	int *nx = dims->nx;
	int *nu = dims->nu;
	int *nb = dims->nb;
	int *ng = dims->ng;

    int size = sizeof(ocp_nlp_out);

	size += 3*(N+1)*sizeof(struct blasfeo_dvec); // ux lam
	size += 1*N*sizeof(struct blasfeo_dvec); // pi

    for (int ii = 0; ii < N; ii++)
    {
		size += 1*blasfeo_memsize_dvec(nu[ii]+nx[ii]); // ux
		size += 1*blasfeo_memsize_dvec(nx[ii+1]); // pi
		size += 2*blasfeo_memsize_dvec(2*nb[ii]+2*ng[ii]); // lam t
    }
	size += 1*blasfeo_memsize_dvec(nu[N]+nx[N]); // ux
	size += 2*blasfeo_memsize_dvec(2*nb[N]+2*ng[N]); // lam t

	size += 8; // initial align
	size += 8; // blasfeo_struct align
	size += 64; // blasfeo_mem align

//	make_int_multiple_of(64, &size);

    return size;
}



ocp_nlp_out *ocp_nlp_out_assign(ocp_nlp_solver_config *config, ocp_nlp_dims *dims, void *raw_memory)
{

	// extract sizes
    int N = dims->N;
	int *nx = dims->nx;
	int *nu = dims->nu;
	int *nb = dims->nb;
	int *ng = dims->ng;

    char *c_ptr = (char *) raw_memory;

	// initial align
	align_char_to(8, &c_ptr);

    ocp_nlp_out *out = (ocp_nlp_out *)c_ptr;
    c_ptr += sizeof(ocp_nlp_out);

	out->dims = dims;

	// blasfeo_struct align
	align_char_to(8, &c_ptr);

	// blasfeo_dvec_struct
	// ux
	assign_blasfeo_dvec_structs(N+1, &out->ux, &c_ptr);
	// pi
	assign_blasfeo_dvec_structs(N, &out->pi, &c_ptr);
	// lam
	assign_blasfeo_dvec_structs(N+1, &out->lam, &c_ptr);
	// t
	assign_blasfeo_dvec_structs(N+1, &out->t, &c_ptr);

	// blasfeo_mem align
	align_char_to(64, &c_ptr);

	// blasfeo_dvec
	// ux
    for (int ii = 0; ii <= N; ++ii)
		assign_blasfeo_dvec_mem(nu[ii]+nx[ii], out->ux+ii, &c_ptr);
	// pi
    for (int ii = 0; ii < N; ++ii)
		assign_blasfeo_dvec_mem(nx[ii+1], out->pi+ii, &c_ptr);
	// lam
    for (int ii = 0; ii <= N; ++ii)
		assign_blasfeo_dvec_mem(2*nb[ii]+2*ng[ii], out->lam+ii, &c_ptr);
	// t
    for (int ii = 0; ii <= N; ++ii)
		assign_blasfeo_dvec_mem(2*nb[ii]+2*ng[ii], out->t+ii, &c_ptr);

    assert((char *) raw_memory + ocp_nlp_out_calculate_size(config, dims) >= c_ptr);

    return out;
}



/************************************************
* memory
************************************************/

int ocp_nlp_memory_calculate_size(ocp_nlp_solver_config *config, ocp_nlp_dims *dims)
{
	// extract dims
    int N = dims->N;
	int *nx = dims->nx;
	int *nu = dims->nu;
	int *nb = dims->nb;
	int *ng = dims->ng;

    int size = sizeof(ocp_nlp_memory);

	size += 4*(N+1)*sizeof(struct blasfeo_dvec); // cost_grad dyn_adj ineq_fun ineq_adj
	size += 1*N*sizeof(struct blasfeo_dvec); // dyn_fun

	for (int ii = 0; ii < N; ii++)
	{
		size += 3*blasfeo_memsize_dvec(nu[ii]+nx[ii]); // cost_grad dyn_adj ineq_adj
		size += 1*blasfeo_memsize_dvec(nx[ii+1]); // dyn_fun
		size += 1*blasfeo_memsize_dvec(2*nb[ii]+2*ng[ii]); // ineq_fun
	}
	size += 3*blasfeo_memsize_dvec(nu[N]+nx[N]); // cost_grad dyn_adj ineq_adj
	size += 1*blasfeo_memsize_dvec(2*nb[N]+2*ng[N]); // ineq_fun

	size += 8; // initial align
	size += 8; // blasfeo_struct align
	size += 64; // blasfeo_mem align

//	make_int_multiple_of(64, &size);

	return size;

}



ocp_nlp_memory *ocp_nlp_memory_assign(ocp_nlp_solver_config *config, ocp_nlp_dims *dims, void *raw_memory)
{
    char *c_ptr = (char *) raw_memory;

	// extract sizes
    int N = dims->N;
	int *nx = dims->nx;
	int *nu = dims->nu;
	int *nb = dims->nb;
	int *ng = dims->ng;

	// initial align
	align_char_to(8, &c_ptr);

	// struct
    ocp_nlp_memory *mem = (ocp_nlp_memory *) c_ptr;
    c_ptr += sizeof(ocp_nlp_memory);

	// dims
	mem->dims = dims;

	// blasfeo_struct align
	align_char_to(8, &c_ptr);

	// cost_grad
	assign_blasfeo_dvec_structs(N+1, &mem->cost_grad, &c_ptr);
	// dyn_fun
	assign_blasfeo_dvec_structs(N, &mem->dyn_fun, &c_ptr);
	// dyn_adj
	assign_blasfeo_dvec_structs(N+1, &mem->dyn_adj, &c_ptr);
	// ineq_fun
	assign_blasfeo_dvec_structs(N+1, &mem->ineq_fun, &c_ptr);
	// ineq_adj
	assign_blasfeo_dvec_structs(N+1, &mem->ineq_adj, &c_ptr);

	// blasfeo_mem align
	align_char_to(64, &c_ptr);

	// cost_grad
	for (int ii = 0; ii <= N; ii++)
		assign_blasfeo_dvec_mem(nu[ii]+nx[ii], mem->cost_grad+ii, &c_ptr);
	// dyn_fun
	for (int ii = 0; ii < N; ii++)
		assign_blasfeo_dvec_mem(nx[ii+1], mem->dyn_fun+ii, &c_ptr);
	// dyn_adj
	for (int ii = 0; ii <= N; ii++)
		assign_blasfeo_dvec_mem(nu[ii]+nx[ii], mem->dyn_adj+ii, &c_ptr);
	// ineq_fun
	for (int ii = 0; ii <= N; ii++)
		assign_blasfeo_dvec_mem(2*nb[ii]+2*ng[ii], mem->ineq_fun+ii, &c_ptr);
	// ineq_adj
	for (int ii = 0; ii <= N; ii++)
		assign_blasfeo_dvec_mem(nu[ii]+nx[ii], mem->ineq_adj+ii, &c_ptr);

	return mem;

}



/************************************************
* residuals
************************************************/

int ocp_nlp_res_calculate_size(ocp_nlp_dims *dims)
{
	// extract dims
    int N = dims->N;
	int *nx = dims->nx;
	int *nu = dims->nu;
	int *nb = dims->nb;
	int *ng = dims->ng;

    int size = sizeof(ocp_nlp_res);

	size += 3*(N+1)*sizeof(struct blasfeo_dvec); // res_g res_d res_m
	size += 1*N*sizeof(struct blasfeo_dvec); // res_b

	for(int ii = 0; ii < N; ii++)
	{
		size += 1*blasfeo_memsize_dvec(nu[ii]+nx[ii]); // res_g
		size += 1*blasfeo_memsize_dvec(nx[ii+1]); // res_b
		size += 2*blasfeo_memsize_dvec(2*nb[ii]+2*ng[ii]); // res_d res_m
	}
	size += 1*blasfeo_memsize_dvec(nu[N]+nx[N]); // res_g
	size += 2*blasfeo_memsize_dvec(2*nb[N]+2*ng[N]); // res_d res_m

	size += 8; // initial align
	size += 8; // blasfeo_struct align
	size += 64; // blasfeo_mem align

//	make_int_multiple_of(64, &size);

	return size;

}



ocp_nlp_res *ocp_nlp_res_assign(ocp_nlp_dims *dims, void *raw_memory)
{
    char *c_ptr = (char *) raw_memory;

	// extract sizes
    int N = dims->N;
	int *nx = dims->nx;
	int *nu = dims->nu;
	int *nb = dims->nb;
	int *ng = dims->ng;

	// initial align
	align_char_to(8, &c_ptr);

	// struct
    ocp_nlp_res *res = (ocp_nlp_res *) c_ptr;
    c_ptr += sizeof(ocp_nlp_res);

	// dims
	res->dims = dims;

	// blasfeo_struct align
	align_char_to(8, &c_ptr);

	// res_g
	assign_blasfeo_dvec_structs(N+1, &res->res_g, &c_ptr);
	// res_b
	assign_blasfeo_dvec_structs(N, &res->res_b, &c_ptr);
	// res_d
	assign_blasfeo_dvec_structs(N+1, &res->res_d, &c_ptr);
	// res_m
	assign_blasfeo_dvec_structs(N+1, &res->res_m, &c_ptr);

	// blasfeo_mem align
	align_char_to(64, &c_ptr);

	// res_g
	for (int ii = 0; ii <= N; ii++)
		assign_blasfeo_dvec_mem(nu[ii]+nx[ii], res->res_g+ii, &c_ptr);
	// res_b
	for (int ii = 0; ii < N; ii++)
		assign_blasfeo_dvec_mem(nx[ii+1], res->res_b+ii, &c_ptr);
	// res_d
	for (int ii = 0; ii <= N; ii++)
		assign_blasfeo_dvec_mem(2*nb[ii]+2*ng[ii], res->res_d+ii, &c_ptr);
	// res_m
	for (int ii = 0; ii <= N; ii++)
		assign_blasfeo_dvec_mem(2*nb[ii]+2*ng[ii], res->res_m+ii, &c_ptr);

	res->memsize = ocp_nlp_res_calculate_size(dims);

	return res;

}



void ocp_nlp_res_compute(ocp_nlp_in *in, ocp_nlp_out *out, ocp_nlp_res *res, ocp_nlp_memory *mem) //, ocp_nlp_res_workspace *work)
{
	// extract dims
    int N = in->dims->N;
    int *nx = in->dims->nx;
    int *nu = in->dims->nu;
    int *nb = in->dims->nb;
    int *ng = in->dims->ng;

	double tmp_res;

	// res_g
	res->inf_norm_res_g = 0.0;
	for (int ii = 0; ii <= N; ii++)
	{
		blasfeo_daxpy(nu[ii]+nx[ii], -1.0, mem->dyn_adj+ii, 0, mem->cost_grad+ii, 0, res->res_g+ii, 0);
		blasfeo_daxpy(nu[ii]+nx[ii], -1.0, mem->ineq_adj+ii, 0, res->res_g+ii, 0, res->res_g+ii, 0);
		blasfeo_dvecnrm_inf(nu[ii]+nx[ii], res->res_g+ii, 0, &tmp_res);
		res->inf_norm_res_g = tmp_res>res->inf_norm_res_g ? tmp_res : res->inf_norm_res_g;
	}

	// res_b
	res->inf_norm_res_b = 0.0;
	for (int ii = 0; ii < N; ii++)
	{
		blasfeo_dveccp(nx[ii+1], mem->dyn_fun+ii, 0, res->res_b+ii, 0);
		blasfeo_dvecnrm_inf(nx[ii+1], res->res_b+ii, 0, &tmp_res);
		res->inf_norm_res_b = tmp_res>res->inf_norm_res_b ? tmp_res : res->inf_norm_res_b;
	}

	// res_d
	res->inf_norm_res_d = 0.0;
	for (int ii = 0; ii <= N; ii++)
	{
		blasfeo_daxpy(2*nb[ii]+2*ng[ii], 1.0, out->t+ii, 0, mem->ineq_fun+ii, 0, res->res_d+ii, 0);
		blasfeo_dvecnrm_inf(2*nb[ii]+2*ng[ii], res->res_d+ii, 0, &tmp_res);
		res->inf_norm_res_d = tmp_res>res->inf_norm_res_d ? tmp_res : res->inf_norm_res_d;
	}

	// res_m
	// TODO(giaf): add blasfeo_dvecmul
	res->inf_norm_res_m = 0.0;
	for (int ii = 0; ii <= N; ii++)
	{
		// tmp = blasfeo_dvecmuldot(2*nb[ii]+2*ng[ii], out->lam+ii, 0, out->t+ii, 0, res->res_m+ii, 0);
		blasfeo_dvecnrm_inf(2*nb[ii]+2*ng[ii], res->res_m+ii, 0, &tmp_res);
		res->inf_norm_res_m = tmp_res>res->inf_norm_res_m ? tmp_res : res->inf_norm_res_m;
	}

	return;

}

























/************************************************
* ???
************************************************/

int number_of_primal_vars(ocp_nlp_dims *dims)
{
    int num_vars = 0;
    for (int ii = 0; ii <= dims->N; ii++) {
        num_vars += dims->nx[ii] + dims->nu[ii];
    }
    return num_vars;
}



void cast_nlp_dims_to_qp_dims(ocp_qp_dims *qp_dims, ocp_nlp_dims *nlp_dims)
{
    qp_dims->N = nlp_dims->N;
    qp_dims->nx = nlp_dims->nx;
    qp_dims->nu = nlp_dims->nu;
    qp_dims->nb = nlp_dims->nb;
    qp_dims->nbx = nlp_dims->nbx;
    qp_dims->nbu = nlp_dims->nbu;
    qp_dims->ng = nlp_dims->ng;
    qp_dims->ns = nlp_dims->ns;

    // TODO(dimitris): probably redundant (can also remove hpipm header)
    qp_dims->memsize = d_memsize_ocp_qp_dim(qp_dims->N);
}


