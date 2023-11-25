# Define hosts and target file
HOSTS=("pp2" "pp3" "pp4" "pp5")
TARGET="hello.cc"
CURRENT_DIR=$(pwd)
HOME_DIR=$HOME
SUB_DIR=${CURRENT_DIR#$HOME_DIR/}

# Function to copy files to remote host
copy_to_host() {
    local host=$1
    ssh "$host" "mkdir -p ~/$SUB_DIR"
    scp mpi_executable "$host:~/$SUB_DIR"
}

# Function to clean up files on remote host
cleanup_on_host() {
    local host=$1
    ssh "$host" "rm -rf ~/$SUB_DIR"
}

# Create hosts file
touch hosts
for host in "${HOSTS[@]}"; do
    echo "$host" >> hosts
done

# Compile executable
cp ../src/$TARGET .
mpicxx ./$TARGET -o mpi_executable

# Copy the executable to each host
for host in "${HOSTS[@]}"; do
    copy_to_host "$host"
done

# (Optional) Insert commands here that use the copied files on remote hosts
mpirun -np 16 --hostfile hosts mpi_executable

# Clean up
for host in "${HOSTS[@]}"; do
    cleanup_on_host "$host"
done

rm $TARGET mpi_executable hosts