# Define hosts and target file
HOSTS=("pp2" "pp3" "pp4" "pp5")

# My implementation target
TARGET="pi_gather.cc"
SOURCE_DIR="../src"

# TA implementation
TA_DIR="../ta_ref"
TA_TARGET="pi_gather"

# Some setting for synchronize different node
CURRENT_DIR=$(pwd)
HOME_DIR=$HOME
SUB_DIR=${CURRENT_DIR#$HOME_DIR/}

# Create a hosts file
printf "%s\n" "${HOSTS[@]}" > hosts

# Compile executable
cp $SOURCE_DIR/$TARGET .
cp $TA_DIR/$TA_TARGET .
mpicxx ./$TARGET -o mpi_executable

# Create directories in parallel on each host
parallel-ssh -h hosts -i "mkdir -p ~/$SUB_DIR"

# Copy the executable to each host in parallel
parallel-scp -h hosts -r mpi_executable $TA_TARGET ~/$SUB_DIR

# (Optional) Insert commands here that use the copied files on remote hosts
mpirun -np 4 --hostfile hosts mpi_executable 1000000000

# Run TA
mpirun -np 4 --hostfile hosts $TA_TARGET 1000000000

# Clean up files on remote hosts in parallel
parallel-ssh -h hosts -i "rm -rf ~/$SUB_DIR"

# Local clean up
rm $TARGET $TA_TARGET mpi_executable hosts
