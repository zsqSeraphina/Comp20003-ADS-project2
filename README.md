# Comp30023-ADS-project2
>This is the [second project](https://github.com/zsqSeraphina/Comp20003-ADS-project2/blob/master/assignment2-1.pdf) of the Comp20003 19 sm2(14.5/15)


Introduction
============
This project is aim to use DFS and Dijkstra to write an ai so that the pacman can find a best direction for each move.

Implementation
==============
>Note: only the ai.c, ai.h and some part of the pacman.c, pacman.h were written by me, the other parts were given base code.

### pacman.c #
>Added some [code](https://github.com/zsqSeraphina/Comp20003-ADS-project2/blob/master/pacman.c#L488) to print information to the output file\
>Added a [clock](https://github.com/zsqSeraphina/Comp20003-ADS-project2/blob/master/pacman.c#L74) to count the time of each play.

### ai.c #
>The [ai](https://github.com/zsqSeraphina/Comp20003-ADS-project2/blob/master/ai.c) uses a [heuristic](https://github.com/zsqSeraphina/Comp20003-ADS-project2/blob/master/ai.c#L83) to count a "mark" for each direction when choosing each move, so that can make a best move.\
>It finds all the possible moves and store them in a queue, then pop each move and count the "mark" for it and its possibly consequent moves, and find a best move from all the possible moves.\
>It has a "max" and a "avg" propagation which means let the ai to find out a move with overall the highest mark, or find out a move that averages the "mark" counted for each possibly move and find a move with highest averaged mark

### Experimentation #
>This project goes with a [experimentation](https://github.com/zsqSeraphina/Comp20003-ADS-project2/blob/master/ads_assmt2-Experimentation.pdf) which shows the results in some different situations and the outcomes.
