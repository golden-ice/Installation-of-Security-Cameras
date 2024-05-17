# Installation-of-Security-Cameras
A series of optimization problems related to Vertex Cover

## S1_Streets:
### How to run the program
 
`python3 S1_Streets/streets.py` 

The input comprises lines each of which specifies a command. There are 4 kinds of commands.
(1) add a street, (2) modify a street, (3) remove a street, and, (4) generate a graph. Here is an
example of how the  program would work.

### Sample Input
add "Weber Street" (2,-1) (2,2) (5,5) (5,6) (3,8)  
add "King Street S" (4,2) (4,8)  
add "Davenport Road" (1,4) (5,8)  
gg  
V = {  
&nbsp;&nbsp;1: (2,2)  
&nbsp;&nbsp;2: (4,2)  
&nbsp;&nbsp;3: (4,4)  
&nbsp;&nbsp;4: (5,5)  
&nbsp;&nbsp;5: (1,4)  
&nbsp;&nbsp;6: (4,7)  
&nbsp;&nbsp;7: (5,6)  
&nbsp;&nbsp;8: (5,8)  
&nbsp;&nbsp;9: (3,8)  
&nbsp;&nbsp;10: (4,8)
}  
E = {
&nbsp;&nbsp;<1,3>,  
&nbsp;&nbsp;<2,3>,  
&nbsp;&nbsp;<3,4>,  
&nbsp;&nbsp;<3,6>,  
&nbsp;&nbsp;<7,6>,  
&nbsp;&nbsp;<6,5>,  
&nbsp;&nbsp;<9,6>,  
&nbsp;&nbsp;<6,8>,  
&nbsp;&nbsp;<6,10>  
}  
mod "Weber Street" (2,1) (2,2)  
gg  
V = {  
&nbsp;&nbsp;2: (4,2)  
&nbsp;&nbsp;5: (1,4)  
&nbsp;&nbsp;6: (4,7)  
&nbsp;&nbsp;8: (5,8)  
&nbsp;&nbsp;10: (4,8)  
}  
E = {  
&nbsp;&nbsp;<2,6>,  
&nbsp;&nbsp;<6,5>,  
&nbsp;&nbsp;<6,8>,  
&nbsp;&nbsp;<6,10>  
}  
rm "King Street S"  
gg  
V = {  
}  
E = {  
}  


## S2_Find_shortest_paths:
### How to run the program

First, compile the program：  
`cd S2_Find_shortest_paths && mkdir build && cd build && cmake ../ && make`

Then, run the program:  
`./find-shortest-paths`  

### Sample Run
Assume that your executable is called find-shortest-paths. In the following, `$`  is the command-prompt.  
```bash
$ ./find-shortest-paths  
```

V 15  
E {<2,6>,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>}  
s 2 10  
2-8-10  
V 5  
E {<1,3>,<3,2>,<3,4>,<4,5>,<5,2>}  
s 5 1  
5-2-3-1

## S3_streets_and_shortest_paths:

### Sample Run
You should name the driver’s executable streets_and_shortest_paths. In the following, we assume that `$` is the
shell command-prompt.
```bash
$ cd S3_Streets_and_shortest_paths && mkdir build
$ cd build
$ cmake ../
$ make install
$ cd ./run/bin
$ ./streets_and_shortest_paths -s 5 -n 4 -l 5
```
V 8  
E {<1,3>,<1,4>,<1,5>,<2,4>,<5,8>,<6,3>,<6,7>}  
s 3 5  
3-1-5  

In the above, the lines “V = ...”, “E = ...”, and “3-1-5” are output. The
input the user provided to the program via stdin is “s 3 5”.

### Random input generator
The random input generator rgen generates random street specifications as input for the
Python script. It takes four command-line arguments. All are optional.

- -s k — where k is an integer ≥ 2. The number of streets should be a random integer in [2, k].
If this option is not specified, you should use a default of k = 10; that is, the number of streets
should be a random integer in [2, 10].   
- -n k — where k is an integer ≥ 1. The number of line-segments in each street should be a
random integer in [1, k]. Default: k = 5.
- -l k — where k is an integer ≥ 5. Your process should wait a random number w seconds,
where w is in [5, k] before generating the next (random) input. Default: k = 5.
- -c k — where k is an integer ≥ 1. Your process should generate (x, y) coordinates such that
every x and y value is in the range [−k, k]. For example, if k

## S4_find_vertex_cover:

### How to run the program

First, compile minisat SAT Solver:

```bash
$ cd S4_find_verte_cover
$ cd minisat && mkdir build && cd build && cmake ../ && make
```

### Sample Run
```bash
$ cd ../..
$ mkdir build && cd build && cmake ../ && make
$ ./find-vertex-cover
```

V 5  
E {<1,5>,<5,2>,<1,4>,<4,5>,<4,3>,<2,4>}  
4 5  

The lines starting with V and E are the inputs to the program, and the last line is the output.
Note that the minimum-sized vertex cover is not necessarily unique.

## S5_algo_compare:

The program has following characteristics:

- It is multithreaded. Four threads are involved: one for I/O, and one each for the
different approaches to solve the minimum vertex cover problem.

- Three algorithms are involved to solve the MIN-VERTEX-COVER problem:

  1. The approach derived from S4_find_vertex_cover, which used the SAT Solver to search the exact MIN-VERTEX-COVER. We will call this algorithm CNF-SAT-VC.

  2. Pick a vertex of highest degree (most incident edges). Add it to your vertex cover and throw away all edges incident on that vertex. Repeat till no edges remain. We will call this algorithm APPROX-VC-1.

  3. Pick an edge ⟨u, v⟩, and add both u and v to your vertex cover. Throw away all edges attached to u and v. Repeat till no edges remain. We will call this algorithm APPROX-VC-2.

### Inputs
For the input, your can refer the files under /input. 

### Output
Given a graph as input, the program should output the vertex cover computed by each approach
in sorted order. That is, give the following input:

V 5  
E {<3,2>,<3,1>,<3,4>,<2,5>,<5,4>}  

The output from your program should be:

CNF-SAT-VC: 3,5  
APPROX-VC-1: 3,5  
APPROX-VC-2: 1,3,4,5

That is, the name of the algorithm, followed by a colon ’:’, a single space, and then the computed
result as a sorted sequence of vertices, separated by commas.

### How to run the program

First, compile minisat SAT Solver:

```bash
$ cd S5_algo_compare
$ cd minisat && mkdir build && cd build && cmake ../ && make
```

### Sample Run
```bash
$ cd ../..
$ mkdir build && cd build && cmake ../ && make
$ ./algo-compare
```
