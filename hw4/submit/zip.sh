rm -f HW4_312581029.zip
rm -rf part1 
rm -rf part2 
mkdir part1
mkdir part2

cp ../src/hello.cc part1
cp ../src/pi_block_linear.cc part1
cp ../src/pi_block_tree.cc part1
cp ../src/pi_gather.cc part1
cp ../src/pi_nonblock_linear.cc part1
cp ../src/pi_reduce.cc part1
cp ../src/pi_one_side.cc part1

cp ../src/Makefile part2
cp ../src/main.cc part2
cp ../src/matrix.cc part2

zip HW4_312581029.zip part1/* part2/*