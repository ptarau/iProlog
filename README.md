## License: Apache 2.0

tested with Java 8, developed with IntelliJ and cmd line
assumes SWI-Prolog 7.x for compiling from .pl to .pl.nl files

##usage:

* go.sh add
* go.sh perms
* go.sh queens

or

* go.sh <any pure Prolog program in directory progs>

with Prolog files assumed having suffix .pl 

### see

* doc/paper.pdf

and [tutorial at VMSS'16](https://www.youtube.com/watch?v=SRYAMt8iQSw&t=82s)

for motivations and justification of implementation choices

a swi prolog script first compiles the code to a pl.nl
file, than Main calls stuff in Engine which loads it in memory
and runs it

### primitive types:

* int
* ground
* var (U+V)
* ref
* array

see main code (~1000lines) in Engine.java

* pl2nl.pro compiles a .pl file to its .nl equivalent, ready to run by
the java based runtime system

* natint.pro emulates its work by compiling back .nl files to Prolog clauses


### todo:

* design a self-contained compiler
* faster runtime:
    memory efficiency - eg recursive loop, LCO
    code to be mem_copied with ptrs to var/ref cells to be relocated?

* more thoughts?

- no symbol tables:

- a symbol is just a ground array of ints

- instead of a symbol table we would have a "ground cache"

- when a non-ground compound tries to unify with a ground, the   ground is expanded to the heap

- when a ground unifies with a ground - it's just handle equality
when a var unifies with a ground it just points to it - as if it were a constant


Enjoy,

Paul Tarau

August 2017


