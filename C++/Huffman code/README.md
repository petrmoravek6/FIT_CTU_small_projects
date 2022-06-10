# Huffman code decompression
The task was to create a set of C++ functions that can decompress an input file with Huffman code. Huffman code is a data compression principle that uses the statistical properties of stored data. In typical files, different values (eg characters) are represented with a very uneven frequency. For example, spaces are very common in typical text, while the 'ř' character will be less frequent. The Huffman code analyzes the frequency of occurrence of individual characters and assigns codes to individual characters according to frequencies. The codes vary in length, typically 1 to tens of bits. Frequent characters get shorter codes, infrequent characters get longer codes. This leads to saving a lot of memory.

**You can find more about this task in the "details.pdf" file.**

## Commands
- **make (make compile)** - compiles the program
- **make run** - compiles the program and runs it
- **make clean** - cleans all temporary data and binary files 
