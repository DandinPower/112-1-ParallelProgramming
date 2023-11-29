# Define hosts and target file
HOSTS=("pp2" "pp3" "pp4" "pp5")
TARGET="hello.cc"
SOURCE_DIR="../src"
CURRENT_DIR=$(pwd)
HOME_DIR=$HOME
SUB_DIR=${CURRENT_DIR#$HOME_DIR/}

# Create a hosts file
printf "%s\n" "${HOSTS[@]}" > hosts

# Compile executable
cp $SOURCE_DIR/$TARGET .
mpicxx ./$TARGET -o mpi_executable

# Create directories in parallel on each host
parallel-ssh -h hosts -i "mkdir -p ~/$SUB_DIR"

# Copy the executable to each host in parallel
parallel-scp -h hosts -r mpi_executable ~/$SUB_DIR

# (Optional) Insert commands here that use the copied files on remote hosts
mpirun -np 16 --hostfile hosts ~/$SUB_DIR/mpi_executable

# Clean up files on remote hosts in parallel
parallel-ssh -h hosts -i "rm -rf ~/$SUB_DIR"

# Local clean up
rm $TARGET mpi_executable hosts
