# Concurrent Prime Number Finder
 A Prime Number Finder, with workers that execute concurrently using syscalls.

## Information
This is a low level prime number finder for specific ranges. It uses syscalls to create processes, manage existing ones and help coordinate processes which work for the same goal, concurrently in a hierarchy. The program finds all primes in a range (m, n) with the help of a flexible process tree. The process hierarchy is created usign the fork() syscall. Nodes on different levels of the hierarchy execute different and independent executables that communicate with each other with pipes and signals. 

The "master" executable ``` myprime ``` is the one that acts as the root of the hierarchy and dynamically creates internal nodes and after all the leaf-nodes. The leaf-nodes are the ones responsible for the search of prime numbers in specific number ranges that they have been assigned. The internal nodes are the ones composing all the primes in a sorted list. The internal nodes also communicate with the root and their children through pipes, to create the final sorted list. Each level of the hierarchy is made up of children that get overwritten with different executables (using the exec*() syscall) based on their role. Also, every child, before finishing execution, sends a USR1 signal at the root.

## Using the finder

### Compilation

Enter the ``` make ``` command while in the */Concurrent-Prime-Number-Finder/* directory.

### Execution 

Type ``` ./myprime ``` while in the project's directory specifying the needed arguments.

### Command line arguments
You can give the following arguments in the command line:

``` -l <lb> ``` : lb is the minimum number of the range.

``` -u <ub> ``` : ub is the maximum number of the range.

``` -w <numOfChildren> ``` : numOfChildren is the number of children each level creates.

> You can use ``` ./make run ``` for a faster run.

## About
- This is a fifth semester assignment, by professor [A. Delis](https://www.alexdelis.eu/), for the Department of Informatics and Telecommunications, UoA. It was a part of the Operating Systems course and was given as the second assignment.
