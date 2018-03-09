#!/bin/bash
#@ energy_policy_tag=NONE
#@ minimize_time_to_solution=yes

#-- Intel MPI: ------------
#@ job_type=MPICH
#--------------------------

#@ class=test

#@ node=<<NUM_NODES>>
#@ tasks_per_node=<<NUM_PROCS>>

#@ node_usage=not_shared

# not needed for class general:
#@ island_count=1

#@ wall_clock_limit=0:30:00
#@ job_name=mst.equal
#@ network.MPI=sn_all,not_shared,us
#@ initialdir=$(home)
#@ output=./logs/$(job_name)/job.n<<NUM_NODES>>p<<NUM_PROCS>>t<<NUM_THREADS>>.$(schedd_host).$(jobid).out
#@ error=./logs/$(job_name)/job.n<<NUM_NODES>>p<<NUM_PROCS>>t<<NUM_THREADS>>.$(schedd_host).$(jobid).err
#@ notification=never
#@ notify_user=stefan.effenberger@nm.ifi.lmu.de
#@ queue

#-- MPI environment settings: --
#export MP_CSS_INTERRUPT=yes
# export MP_USE_BULK_XFER=yes
# export MP_BULK_MIN_MSG_SIZE=4096

# Increases communication latency but allows overlap of
# communication and computation:
# export MPICH_ASYNC_PROGRESS=1

# For status messages (like pinning) from Intel MPI:
# export I_MPI_DEBUG=4

# export MV2_USE_RDMA_CM=1
# export MV2_USE_IWARP_MODE=1
# export MV2_RDMA_CM_MAX_PORT=65535
# export MV2_VBUF_TOTAL_SIZE=9216
# export I_MPI_DEVICE=rdma:OpenIB-iwarp
# export I_MPI_RENDEZVOUS_RDMA_WRITE=1
# export I_MPI_RDMA_USE_EVD_FALLBACK=1
# export I_MPI_RDMA_TRANSLATION_CACHE=1
# export I_MPI_DAPL_CHECK_MAX_RDMA_SIZE=1

# Process pinning: Pin unit $u to CPU $u:
# export I_MPI_PIN=1
# export I_MPI_PIN_PROCESSOR_LIST=0-27
#-------------------------------

#-- MKL environment settings: --
# export MP_SINGLE_THREAD=yes

# For hybrid MPI+OpenMP jobs:
# export MP_SINGLE_THREAD=no
# export OMP_NUM_THREADS=<<NUM_THREADS>>
# export MP_TASK_AFFINITY=core:$OMP_NUM_THREADS
#-------------------------------
# Increases communication latency but allows overlap of
# communication and computation:
export MPICH_ASYNC_PROGRESS=0

# Enable RDMA:
export MP_CSS_INTERRUPT=yes
export MP_USE_BULK_XFER=yes
export MP_BULK_MIN_MSG_SIZE=4096

# Shared memory settings:
export MP_SHARED_MEMORY=yes
export MP_SINGLE_THREAD=no

# Early Arrival:
export MP_BUFFER_MEM=1024M
export MP_EAGER_LIMIT=32768
# export MP_EAGER_LIMIT_LOCAL=1024

# For status messages (like pinning):
export I_MPI_DEBUG=4

# Process pinning: Pin unit $u to CPU $u: (IBM MPI):
export MP_PE_AFFINITY=yes
export MP_TASK_AFFINITY=cpu
export MP_CPU_BIND_LIST=0-27
export MP_PROC=<<NUM_PROCS>>
export MP_WAIT_MODE=nopoll

export DASH_ENABLE_TRACE=1

# Setup environment:
. /etc/profile
. /etc/profile.d/modules.sh

#-- Intel MPI: ------------
module unload mpi.ibm
module load mpi.intel/5.1
# For hybrid MPI+OpenMP jobs:
# module load mpi_pinning/hybrid_blocked
mpiexec -n `echo "$((<<NUM_PROCS>> * <<NUM_NODES>>))"` \
        ./mst_eq 100000 100000
#--------------------------
memory_usage
