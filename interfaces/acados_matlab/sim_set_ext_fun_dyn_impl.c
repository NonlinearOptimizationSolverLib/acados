// system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// acados
#include "acados/utils/external_function_generic.h"
#include "acados_c/external_function_interface.h"
// mex
#include "mex.h"



// casadi functions for the model
#include "sim_model.h"



void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
	{

//	mexPrintf("\nin sim_impl_ext_fun_create\n");

	// sizeof(long long) == sizeof(void *) = 64 !!!
	long long *ptr;



	/* RHS */

	// model

	int np = 0;
	if(mxGetField( prhs[1], 0, "dim_np" )!=NULL) // TODO bool
		{
		np = mxGetScalar( mxGetField( prhs[1], 0, "dim_np" ) );
		}
	// TODO bool instead !!!
	char *param_f = mxArrayToString( mxGetField( prhs[1], 0, "dyn_param_f" ) );


	// opts

//	bool sens_forw = mxGetScalar( mxGetField( prhs[0], 0, "sens_forw" ) );
//	mexPrintf("\n%d\n", sens_forw);
	char *method = mxArrayToString( mxGetField( prhs[2], 0, "method" ) );
//	mexPrintf("\n%s\n", method);



	/* LHS */

	// field names of output struct
	char *fieldnames[6];
	fieldnames[0] = (char*)mxMalloc(50);
	fieldnames[1] = (char*)mxMalloc(50);
	fieldnames[2] = (char*)mxMalloc(50);
	fieldnames[3] = (char*)mxMalloc(50);
	fieldnames[4] = (char*)mxMalloc(50);
	fieldnames[5] = (char*)mxMalloc(50);

	memcpy(fieldnames[0],"dyn_expl_ode_fun",sizeof("dyn_expl_ode_fun"));
	memcpy(fieldnames[1],"dyn_expl_vde_for",sizeof("dyn_expl_vde_for"));
	memcpy(fieldnames[2],"dyn_expl_vde_adj",sizeof("dyn_expl_vde_adj"));
	memcpy(fieldnames[3],"dyn_impl_ode_fun",sizeof("dyn_impl_ode_fun"));
	memcpy(fieldnames[4],"dyn_impl_ode_fun_jac_x_xdot",sizeof("dyn_impl_ode_fun_jac_x_xdot"));
	memcpy(fieldnames[5],"dyn_impl_ode_jac_x_xdot_u",sizeof("dyn_impl_ode_jac_x_xdot_u"));

	// create output struct
	plhs[0] = mxCreateStructMatrix(1, 1, 6, (const char **) fieldnames);

	mxFree( fieldnames[0] );
	mxFree( fieldnames[1] );
	mxFree( fieldnames[2] );
	mxFree( fieldnames[3] );
	mxFree( fieldnames[4] );
	mxFree( fieldnames[5] );

	// copy fields
	mxArray *tmp_mat;
	//
	tmp_mat = mxGetField(prhs[0], 0, "dyn_expl_ode_fun");
	if(tmp_mat!=NULL && mxGetM(tmp_mat)>0)
		{
		mxSetField(plhs[0], 0, "dyn_expl_ode_fun", tmp_mat );
		}
	//
	tmp_mat = mxGetField(prhs[0], 0, "dyn_expl_vde_for");
	if(tmp_mat!=NULL && mxGetM(tmp_mat)>0)
		{
		mxSetField(plhs[0], 0, "dyn_expl_vde_for", tmp_mat );
		}
	//
	tmp_mat = mxGetField(prhs[0], 0, "dyn_expl_vde_for");
	if(tmp_mat!=NULL && mxGetM(tmp_mat)>0)
		{
		mxSetField(plhs[0], 0, "dyn_expl_vde_for", tmp_mat );
		}
	//
	tmp_mat = mxGetField(prhs[0], 0, "dyn_impl_ode_fun");
	if(tmp_mat!=NULL && mxGetM(tmp_mat)>0)
		{
		mxSetField(plhs[0], 0, "dyn_impl_ode_fun", tmp_mat );
		}
	//
	tmp_mat = mxGetField(prhs[0], 0, "dyn_impl_ode_fun_jac_x_xdot");
	if(tmp_mat!=NULL && mxGetM(tmp_mat)>0)
		{
		mxSetField(plhs[0], 0, "dyn_impl_ode_fun_jac_x_xdot", tmp_mat );
		}
	//
	tmp_mat = mxGetField(prhs[0], 0, "dyn_impl_ode_jac_x_xdot_u");
	if(tmp_mat!=NULL && mxGetM(tmp_mat)>0)
		{
		mxSetField(plhs[0], 0, "dyn_impl_ode_jac_x_xdot_u", tmp_mat );
		}



	/* populate new fields */

	external_function_casadi *ext_fun_ptr;
	external_function_param_casadi *ext_fun_param_ptr;

	// TODO templetize the casadi function names !!!
	if(!strcmp(method, "irk"))
		{
		if(!strcmp(param_f, "true")) // TODO bool
			{
			// impl_ode_fun
			ext_fun_param_ptr = (external_function_param_casadi *) malloc(1*sizeof(external_function_param_casadi));
			external_function_param_casadi_set_fun(ext_fun_param_ptr, &sim_model_dyn_impl_ode_fun);
			external_function_param_casadi_set_work(ext_fun_param_ptr, &sim_model_dyn_impl_ode_fun_work);
			external_function_param_casadi_set_sparsity_in(ext_fun_param_ptr, &sim_model_dyn_impl_ode_fun_sparsity_in);
			external_function_param_casadi_set_sparsity_out(ext_fun_param_ptr, &sim_model_dyn_impl_ode_fun_sparsity_out);
			external_function_param_casadi_set_n_in(ext_fun_param_ptr, &sim_model_dyn_impl_ode_fun_n_in);
			external_function_param_casadi_set_n_out(ext_fun_param_ptr, &sim_model_dyn_impl_ode_fun_n_out);
			external_function_param_casadi_create(ext_fun_param_ptr, np);
			// populate output struct
			mxArray *impl_ode_fun_mat  = mxCreateNumericMatrix(1, 1, mxINT64_CLASS, mxREAL);
			ptr = mxGetData(impl_ode_fun_mat);
			ptr[0] = (long long) ext_fun_param_ptr;
			mxSetField(plhs[0], 0, "dyn_impl_ode_fun", impl_ode_fun_mat);

			// impl_ode_fun_jac_x_xdot
			ext_fun_param_ptr = (external_function_param_casadi *) malloc(1*sizeof(external_function_param_casadi));
			external_function_param_casadi_set_fun(ext_fun_param_ptr, &sim_model_dyn_impl_ode_fun_jac_x_xdot);
			external_function_param_casadi_set_work(ext_fun_param_ptr, &sim_model_dyn_impl_ode_fun_jac_x_xdot_work);
			external_function_param_casadi_set_sparsity_in(ext_fun_param_ptr, &sim_model_dyn_impl_ode_fun_jac_x_xdot_sparsity_in);
			external_function_param_casadi_set_sparsity_out(ext_fun_param_ptr, &sim_model_dyn_impl_ode_fun_jac_x_xdot_sparsity_out);
			external_function_param_casadi_set_n_in(ext_fun_param_ptr, &sim_model_dyn_impl_ode_fun_jac_x_xdot_n_in);
			external_function_param_casadi_set_n_out(ext_fun_param_ptr, &sim_model_dyn_impl_ode_fun_jac_x_xdot_n_out);
			external_function_param_casadi_create(ext_fun_param_ptr, np);
			// populate output struct
			mxArray *impl_ode_fun_jac_x_xdot_mat  = mxCreateNumericMatrix(1, 1, mxINT64_CLASS, mxREAL);
			ptr = mxGetData(impl_ode_fun_jac_x_xdot_mat);
			ptr[0] = (long long) ext_fun_param_ptr;
			mxSetField(plhs[0], 0, "dyn_impl_ode_fun_jac_x_xdot", impl_ode_fun_jac_x_xdot_mat);

			// impl_ode_jac_x_xdot_u
			ext_fun_param_ptr = (external_function_param_casadi *) malloc(1*sizeof(external_function_param_casadi));
			external_function_param_casadi_set_fun(ext_fun_param_ptr, &sim_model_dyn_impl_ode_jac_x_xdot_u);
			external_function_param_casadi_set_work(ext_fun_param_ptr, &sim_model_dyn_impl_ode_jac_x_xdot_u_work);
			external_function_param_casadi_set_sparsity_in(ext_fun_param_ptr, &sim_model_dyn_impl_ode_jac_x_xdot_u_sparsity_in);
			external_function_param_casadi_set_sparsity_out(ext_fun_param_ptr, &sim_model_dyn_impl_ode_jac_x_xdot_u_sparsity_out);
			external_function_param_casadi_set_n_in(ext_fun_param_ptr, &sim_model_dyn_impl_ode_jac_x_xdot_u_n_in);
			external_function_param_casadi_set_n_out(ext_fun_param_ptr, &sim_model_dyn_impl_ode_jac_x_xdot_u_n_out);
			external_function_param_casadi_create(ext_fun_param_ptr, np);
			// populate output struct
			mxArray *impl_ode_jac_x_xdot_u_mat  = mxCreateNumericMatrix(1, 1, mxINT64_CLASS, mxREAL);
			ptr = mxGetData(impl_ode_jac_x_xdot_u_mat);
			ptr[0] = (long long) ext_fun_param_ptr;
			mxSetField(plhs[0], 0, "dyn_impl_ode_jac_x_xdot_u", impl_ode_jac_x_xdot_u_mat);
			}
		else
			{
			// impl_ode_fun
			ext_fun_ptr = (external_function_casadi *) malloc(1*sizeof(external_function_casadi));
			external_function_casadi_set_fun(ext_fun_ptr, &sim_model_dyn_impl_ode_fun);
			external_function_casadi_set_work(ext_fun_ptr, &sim_model_dyn_impl_ode_fun_work);
			external_function_casadi_set_sparsity_in(ext_fun_ptr, &sim_model_dyn_impl_ode_fun_sparsity_in);
			external_function_casadi_set_sparsity_out(ext_fun_ptr, &sim_model_dyn_impl_ode_fun_sparsity_out);
			external_function_casadi_set_n_in(ext_fun_ptr, &sim_model_dyn_impl_ode_fun_n_in);
			external_function_casadi_set_n_out(ext_fun_ptr, &sim_model_dyn_impl_ode_fun_n_out);
			external_function_casadi_create(ext_fun_ptr);
			// populate output struct
			mxArray *impl_ode_fun_mat  = mxCreateNumericMatrix(1, 1, mxINT64_CLASS, mxREAL);
			ptr = mxGetData(impl_ode_fun_mat);
			ptr[0] = (long long) ext_fun_ptr;
			mxSetField(plhs[0], 0, "dyn_impl_ode_fun", impl_ode_fun_mat);

			// impl_ode_fun_jac_x_xdot
			ext_fun_ptr = (external_function_casadi *) malloc(1*sizeof(external_function_casadi));
			external_function_casadi_set_fun(ext_fun_ptr, &sim_model_dyn_impl_ode_fun_jac_x_xdot);
			external_function_casadi_set_work(ext_fun_ptr, &sim_model_dyn_impl_ode_fun_jac_x_xdot_work);
			external_function_casadi_set_sparsity_in(ext_fun_ptr, &sim_model_dyn_impl_ode_fun_jac_x_xdot_sparsity_in);
			external_function_casadi_set_sparsity_out(ext_fun_ptr, &sim_model_dyn_impl_ode_fun_jac_x_xdot_sparsity_out);
			external_function_casadi_set_n_in(ext_fun_ptr, &sim_model_dyn_impl_ode_fun_jac_x_xdot_n_in);
			external_function_casadi_set_n_out(ext_fun_ptr, &sim_model_dyn_impl_ode_fun_jac_x_xdot_n_out);
			external_function_casadi_create(ext_fun_ptr);
			// populate output struct
			mxArray *impl_ode_fun_jac_x_xdot_mat  = mxCreateNumericMatrix(1, 1, mxINT64_CLASS, mxREAL);
			ptr = mxGetData(impl_ode_fun_jac_x_xdot_mat);
			ptr[0] = (long long) ext_fun_ptr;
			mxSetField(plhs[0], 0, "dyn_impl_ode_fun_jac_x_xdot", impl_ode_fun_jac_x_xdot_mat);

			// impl_ode_jac_x_xdot_u
			ext_fun_ptr = (external_function_casadi *) malloc(1*sizeof(external_function_casadi));
			external_function_casadi_set_fun(ext_fun_ptr, &sim_model_dyn_impl_ode_jac_x_xdot_u);
			external_function_casadi_set_work(ext_fun_ptr, &sim_model_dyn_impl_ode_jac_x_xdot_u_work);
			external_function_casadi_set_sparsity_in(ext_fun_ptr, &sim_model_dyn_impl_ode_jac_x_xdot_u_sparsity_in);
			external_function_casadi_set_sparsity_out(ext_fun_ptr, &sim_model_dyn_impl_ode_jac_x_xdot_u_sparsity_out);
			external_function_casadi_set_n_in(ext_fun_ptr, &sim_model_dyn_impl_ode_jac_x_xdot_u_n_in);
			external_function_casadi_set_n_out(ext_fun_ptr, &sim_model_dyn_impl_ode_jac_x_xdot_u_n_out);
			external_function_casadi_create(ext_fun_ptr);
			// populate output struct
			mxArray *impl_ode_jac_x_xdot_u_mat  = mxCreateNumericMatrix(1, 1, mxINT64_CLASS, mxREAL);
			ptr = mxGetData(impl_ode_jac_x_xdot_u_mat);
			ptr[0] = (long long) ext_fun_ptr;
			mxSetField(plhs[0], 0, "dyn_impl_ode_jac_x_xdot_u", impl_ode_jac_x_xdot_u_mat);
			}
		}
	else
		{
		mexPrintf("\nsim_set_ext_fun_dyn_impl: method not supported %s\n", method);
		return;
		}
	
	return;

	}


