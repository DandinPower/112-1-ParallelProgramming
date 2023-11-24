rm -f HW3_312581029.zip
rm -f *.c
rm -f *.cpp

cp ../part1/cg_impl.c .
cp ../part2/breadth_first_search/bfs.cpp .
cp ../part2/page_rank/page_rank.cpp .

zip HW3_312581029.zip *.c *.cpp