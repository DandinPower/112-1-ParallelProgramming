# Define hosts and target file
HOSTS=("pp2" "pp3" "pp4" "pp5")
SLOTS=1

# My implementation target
TARGET1="main.cc"
TARGET2="matrix.cc"
MAKEFILE_TARGET="Makefile"
SOURCE_DIR="../src"

DATASET_DIR="../data-set"

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
make

# Define datasets and their corresponding answers
# Initialize empty arrays
DATASETS=()
ANSWERS=()
# Generate dataset and answer names
for i in {1..10}; do
  DATASETS+=("data1_$i" "data2_$i")
  ANSWERS+=("ans1_$i" "ans2_$i")
done

# Loop over each dataset
for i in "${!DATASETS[@]}"; do
  DATASET_TARGET=$DATASET_DIR/"${DATASETS[$i]}"
  DATASET_ANSWER=$DATASET_DIR/"${ANSWERS[$i]}"
  echo "Run Test $DATASET_TARGET"

  # Create directories in parallel on each host
  parallel-ssh -h hosts -i "mkdir -p ~/$SUB_DIR"
  # Copy the executable to each host in parallel
  parallel-scp -h hosts -r matmul ~/$SUB_DIR

  mpirun -np 4 --hostfile mpi_hosts matmul < $DATASET_TARGET > result.log

  # Extracting the matrix from the output log
  # Assuming the matrix lines do not contain the word 'MPI'
  grep -v 'MPI' result.log > extracted_matrix
  grep 'MPI' result.log > time.log

  # Comparing the extracted matrix with the answer file
  diff extracted_matrix $DATASET_ANSWER

  # Check if diff returned a non-zero exit status
  if [ $? -ne 0 ]; then
    echo "Error: Test $DATASET_TARGET failed. Stopping."
    exit 1
  fi

  # Clean up files on remote hosts in parallel
  parallel-ssh -h hosts -i "rm -rf ~/$SUB_DIR"

done

# Local clean up
rm $TARGET1 $TARGET2 $MAKEFILE_TARGET
rm matmul hosts result.log extracted_matrix
rm mpi_hosts
# rm time.log
