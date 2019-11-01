#
# Copyright 2019 Gianluca Frison, Dimitris Kouzoupis, Robin Verschueren,
# Andrea Zanelli, Niels van Duijkeren, Jonathan Frey, Tommaso Sartor,
# Branimir Novoselnik, Rien Quirynen, Rezart Qelibari, Dang Doan,
# Jonas Koenemann, Yutao Chen, Tobias Schöls, Jonas Schlagenhauf, Moritz Diehl
#
# This file is part of acados.
#
# The 2-Clause BSD License
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.;
#

from acados_template import *
import acados_template as at
from export_ode_model import *
import numpy as np
import scipy.linalg
from ctypes import *

FORMULATION = 'NLS' # 'LS'
# FORMULATION = 'LS' # 'LS'

# create render arguments
ocp = acados_ocp_nlp()

# export model 
model = export_ode_model()

# set model_name 
ocp.model = model

Tf = 2.0
nx = model.x.size()[0]
nu = model.u.size()[0]
ny = nx + nu
ny_e = nx
N = 50

# set ocp_nlp_dimensions
nlp_dims     = ocp.dims
nlp_dims.nx  = nx 
nlp_dims.ny  = ny 
nlp_dims.ny_e = ny_e 
nlp_dims.nbx = 0
nlp_dims.nbu = nu 
nlp_dims.nu  = model.u.size()[0]
nlp_dims.N   = N

# set weighting matrices
nlp_cost = ocp.cost

if FORMULATION == 'LS':
    nlp_cost.cost_type = 'LINEAR_LS'
    nlp_cost.cost_type_e = 'LINEAR_LS'
elif FORMULATION == 'NLS':
    nlp_cost.cost_type = 'NONLINEAR_LS'
    nlp_cost.cost_type_e = 'NONLINEAR_LS'
else:
    raise Exception('Unknown FORMULATION. Possible values are \'LS\' and \'NLS\'.')

Q = np.eye(4)
Q[0,0] = 1e0
Q[1,1] = 1e2
Q[2,2] = 1e-3
Q[3,3] = 1e-2

R = np.eye(1)
R[0,0] = 1e0

if FORMULATION == 'NLS':
    nlp_cost.W = scipy.linalg.block_diag(R, Q) 
else:
    nlp_cost.W = scipy.linalg.block_diag(Q, R) 

nlp_cost.W_e = Q 

# TODO(andrea): avoid this when using 'NLS'
Vx = np.zeros((ny, nx))
Vx[0,0] = 1.0
Vx[1,1] = 1.0
Vx[2,2] = 1.0
Vx[3,3] = 1.0

nlp_cost.Vx = Vx

Vu = np.zeros((ny, nu))
Vu[4,0] = 1.0
nlp_cost.Vu = Vu


Vx_e = np.zeros((ny_e, nx))
Vx_e[0,0] = 1.0
Vx_e[1,1] = 1.0
Vx_e[2,2] = 1.0
Vx_e[3,3] = 1.0

nlp_cost.Vx_e = Vx_e
if FORMULATION == 'NLS':
    x = SX.sym('x', 4, 1)
    u = SX.sym('u', 1, 1)
    ocp.cost_r.expr = vertcat(u, x) 
    ocp.cost_r.x = x 
    ocp.cost_r.u = u 
    ocp.cost_r.name = 'lin_res' 
    ocp.cost_r.ny = nx + nu 

    ocp.cost_r_e.expr = x
    ocp.cost_r_e.x = x 
    ocp.cost_r_e.name = 'lin_res' 
    ocp.cost_r_e.ny = nx 


nlp_cost.yref  = np.zeros((ny, ))
nlp_cost.yref_e = np.zeros((ny_e, ))

# setting bounds
Fmax = 2.0
nlp_con = ocp.constraints
nlp_con.lbu = np.array([-Fmax])
nlp_con.ubu = np.array([+Fmax])
nlp_con.x0 = np.array([0.0, 3.14, 0.0, 0.0])
# nlp_con.x0 = np.array([0.0, 0.5, 0.0, 0.0])
nlp_con.idxbu = np.array([0])

# set QP solver
# ocp.solver_options.qp_solver = 'PARTIAL_CONDENSING_HPIPM'
ocp.solver_options.qp_solver = 'FULL_CONDENSING_QPOASES'
ocp.solver_options.hessian_approx = 'GAUSS_NEWTON'
ocp.solver_options.integrator_type = 'ERK'

# set prediction horizon
ocp.solver_options.tf = Tf
# ocp.solver_options.nlp_solver_type = 'SQP'
ocp.solver_options.nlp_solver_type = 'SQP_RTI'

# set header path
ocp.acados_include_path  = '../../../../include'
ocp.acados_lib_path      = '../../../../lib'

acados_solver = generate_solver(ocp, json_file = 'acados_ocp.json')

Nsim = 100

simX = np.ndarray((Nsim, nx))
simU = np.ndarray((Nsim, nu))

for i in range(Nsim):
    status = acados_solver.solve()

    if status != 0:
        raise Exception('acados returned status {}. Exiting.'.format(status))

    # get solution
    x0 = acados_solver.get(0, "x")
    u0 = acados_solver.get(0, "u")
    
    for j in range(nx):
        simX[i,j] = x0[j]

    for j in range(nu):
        simU[i,j] = u0[j]
    
    # update initial condition
    x0 = acados_solver.get(1, "x")

    acados_solver.set(0, "lbx", x0)
    acados_solver.set(0, "ubx", x0)

    # update reference
    for j in range(N):
        acados_solver.set(j, "yref", np.array([0, 0, 0, 0, 0]))
    acados_solver.set(N, "yref", np.array([0, 0, 0, 0]))

# plot results
import matplotlib
import matplotlib.pyplot as plt
t = np.linspace(0.0, Tf/N, Nsim)
plt.subplot(2, 1, 1)
plt.step(t, simU, color='r')
plt.title('closed-loop simulation')
plt.ylabel('u')
plt.xlabel('t')
plt.grid(True)
plt.subplot(2, 1, 2)
plt.plot(t, simX[:,1])
plt.ylabel('theta')
plt.xlabel('t')
plt.grid(True)
# avoid plotting when running on Travis
if os.environ.get('ACADOS_ON_TRAVIS') is None: 
    plt.show()

