## License: Apache 2.0

* tested with Java 8, developed with IntelliJ and cmd line

* it assumes SWI-Prolog 7.x for compiling from .pl to .pl.nl files

## usage:

* go.sh add
* go.sh perms
* go.sh queens

or

* go.sh _\< any pure Prolog program in directory prog \>_

with Prolog files assumed having suffix .pl 

### see

* doc/paper.pdf (_A Hitchhiker's Guide to Reinventing a Prolog Machine_)

and 

* [tutorial at VMSS'16](https://www.youtube.com/watch?v=SRYAMt8iQSw&t=82s)

for some motivations for this and justification of implementation choices

a swi prolog script first compiles the code to a pl.nl
file, than Main calls stuff in Engine which loads it in memory
and runs it

## Notes

### primitive types:

* int
* ground
* var (U+V)
* ref
* array

see main code (~1000lines) in __Engine.java__

* __pl2nl.pl__ compiles a .pl file to its .nl equivalent, ready to run by
the java based runtime system

* _natint.pl_ emulates its work by compiling back .nl files to Prolog clauses


### todo:

* design a self-contained compiler
* convert to C for more effective benchmarking against C-based Prolog systems
* faster runtime:
    memory efficiency - eg recursive loop, LCO
    code to be mem_copied with ptrs to var/ref cells to be relocated?

### some out of the box thoughts:

- no symbol tables: a symbol is just a (small) ground array of ints, and instead of a symbol table we would have a "ground cache" - that helps with better memory usege and also speed

- when a non-ground compound tries to unify with a ground, the   ground term is expanded to the heap

- when a ground unifies with a ground - it's just pointer equality and when a var unifies with a ground, it just points to it - as if it were a constant

Enjoy,

Paul Tarau

August 2017


