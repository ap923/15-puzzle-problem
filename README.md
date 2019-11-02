# 15-puzzle-problem
 C program to solve the 15-puzzle problem using the A* search algorithm.

 For more information, please
read the wiki page of 15-puzzle problem at https://en.wikipedia.org/wiki/15_puzzle, and the wiki
page of A* search at https://en.wikipedia.org/wiki/A*_search_algorithm.

program can solve the problem using a single thread or using 4 threads, depending on the
first argument (argv[1]). The program uses a single thread if argv[1] is "-s", or 4 threads if argv[1]
is "-m". The initial layout of the tiles is also provided in the command line as arguments by listing
tile indexes in a row-major order (refer to this page https://en.wikipedia.org/wiki/Row-
_and_column-major_order if you need to understand what is row-major order). 
