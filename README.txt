Project Partners:
  Abid Azad       (NetID: aa2177)
  Rahul Hegde     (NetID: rah248)
  Alden Lipiarski (NetID: ajl302)
  Dan Lee         (NetID: djl288)
  Shaun Goda      (NetID: sg1563)

General program usage format:
  ./programName L H PN
  where: 
    L = # of integers to generate in text file 
    H = # of keys a user wants to find
    PN = # of processes used to find H keys and 
         requested metrics for max and average of 
         the numbers in the text file

Sample Test Cases:
  ./singleProcess 10000 12 2
  ./dfs 10000 12 2
  ./bfs 10000 12 2
  ./bfsSignal 10000 12 2

For last testcase argument for bfsSignal and bfs:
  - DON'T put a process number (PN) lower than 1
  - DON'T use an H value of 0

For all testcases, please ensure that the L argument is greater than or equal to 10,000

The input file will ALWAYS be keys.txt and
The output file will ALWAYS be output.txt

(both input.txt and output.txt will be generated if they don't exist)
(both input.txt and output.txt will be overwritten if they do exist)