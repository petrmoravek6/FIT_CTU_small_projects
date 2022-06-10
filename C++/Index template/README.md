# Sequence indexing
The task was to create a class template that implements a generic index.

The implemented CIndex class gets a collection of elements with a sequence when creating parameters. It will index this sequence of elements. The sequence can be:
- string - character string (C++ string of elements of char type),
- vector<T> - values vector (some value T),
- list<T> - list of values (some value T).

We want to be able to search in that indexed sequence. We enter a sequence of elements (of the same type) and we want to find out if the indexed sequence contains it somewhere. The result will be a set of positions where the searched sequence is found.

To make the indexing class more general, it is enhanced by the ability to specify a custom comparator (optional second generic parameter). Therefore, the search does not have to be performed on an exact match; The comparator has the form that the comparators have in STL: it will be a function, functor or C ++ 11 lambda expression, which for a pair of elements of type T in the sequence decides which is smaller in the required comparison. If the comparator is not specified, the operator <valid for stored element values is used to compare elements.

**You can find more about this task in the "details.pdf" file. (IN CZECH)**\
An example of is showed in the main function as list of asserts.

## Commands
- **make (make compile)** - compiles the program
- **make run** - compiles the program and runs it
- **make clean** - cleans all temporary data and binary files 
