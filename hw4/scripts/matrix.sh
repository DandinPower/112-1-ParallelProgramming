# Define hosts and target file
HOSTS=("pp2" "pp3" "pp4" "pp5")
SLOTS=1

# My implementation target
TARGET1="main.cc"
TARGET2="matrix.cc"
MAKEFILE_TARGET="Makefile"
SOURCE_DIR="../src"

DATASET_DIR="../data-set"
DATASET_TARGET="data2_6"
DATASET_ANSWER="ans2_6"

# Some setting for synchronize different node
CURRENT_DIR=$(pwd)
HOME_DIR=$HOME
SUB_DIR=${CURRENT_DIR#$HOME_DIR/}

# Create a hosts file
printf "%s\n" "${HOSTS[@]}" > hosts
# Create a mpi_hosts file with slots
for i in "${!HOSTS[@]}"; do
  printf "%s slots=%s\n" "${HOSTS[$i]}" "$SLOTS" >> mpi_hosts
done

# Compile executable
cp $SOURCE_DIR/$TARGET1 .
cp $SOURCE_DIR/$TARGET2 .
cp $SOURCE_DIR/$MAKEFILE_TARGET .
cp $DATASET_DIR/$DATASET_TARGET .
cp $DATASET_DIR/$DATASET_ANSWER .
make

# Create directories in parallel on each host
parallel-ssh -h hosts -i "mkdir -p ~/$SUB_DIR"

# Copy the executable to each host in parallel
parallel-scp -h hosts -r matmul ~/$SUB_DIR

# (Optional) Insert commands here that use the copied files on remote hosts
mpirun -np 4 --hostfile mpi_hosts matmul < $DATASET_TARGET > result.log

# Extracting the matrix from the output log
# Assuming the matrix lines do not contain the word 'MPI'
grep -v 'MPI' result.log > extracted_matrix
grep 'MPI' result.log > time.log

# Comparing the extracted matrix with the answer file
diff extracted_matrix $DATASET_ANSWER

# Clean up files on remote hosts in parallel
parallel-ssh -h hosts -i "rm -rf ~/$SUB_DIR"

# Local clean up
rm $TARGET1 $TARGET2 $MAKEFILE_TARGET $DATASET_TARGET $DATASET_ANSWER
rm matmul hosts result.log extracted_matrix
rm mpi_hosts
# rm time.log
