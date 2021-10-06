Ruchit Patel

---------------------
DESCRIPTION

- A modified version of Hamming(7,4) error correcting code (ECC), Hamming(8,4), is implemented using the matrix method.
- The lab can produce four executables: Encode, Decode, Entropy (source code given), Error (source code given).
- Common Arguments for encoder and decoder:    -h (prints help message), 
		            -i (specifies input file (default:stdin)), 
		            -o (specifies output file (default:stdout)), 
- Arguments specific to decoder:
			    -v (Prints decoding statistics) 
- Arguments for the error program:
			    -e (Specify the error rate (double value)) 

---------------------
FILES

1. encode.c
- This source file contains the main method and the implementation for the encoder. 

2. decode.c
- This source file contains the main method and the implementation for the decoder. 

3. error.c
- This source file (provided) contains the main method and the implementation for the error program. 

4. entropy.c 
- This source file (provided) contains the main method and the implementation for the entropy program. 

5. bv.h
- This header file declares the BitVector abstract data structure and the methods to manipulate it.

6. bv.c
- This source file implements the methods declared in bv.h to work with a BitVector.

7. bm.h
- This header file declares the BitMatrix abstract data structure and the methods to manipulate it.

8. bm.c
- This source file implements the methods declared in bv.h to work with a BitMatrix.

9. hamming.h 
- This header file declares the methods related with the hamming interface and also an enum for return status.

10. hamming.c
-  This source file implements the methods declared in hamming.h (implementation of hamming interface).

11. Makefile

- This is a Makefile that can be used with the make utility to build the executables.

12. DESIGN.pdf 

- This PDF explains the design for this lab. It includes a brief description of the lab and pseudocode alongwith implementation description. 

13. WRITEUP.pdf

- This PDF contains the writeup/analysis of graphs for the relations between hamming encoded files, entropy, decoded files, etc.


---------------------
INSTRUCTIONS

With make:
1. Keep the Makefile in the same directory as all other files. 

2. Execute “make” or “make all” in terminal in order to produce the all four (encode, decode, error, entropy) executables.

3. Execute "make x" where x is either encode, decode, error, or entropy to build the respective executables.

4. Run encode or decode executables with their respective arguments to encode or decode a file. Use the error or entropy programs to introduce errors or measure entropy of a file respectively. The program would run as described in the description and the DESIGN.pdf based on the arguments. 

5. In order to scan-build the source file, run “make scan-build” in the terminal.

6. In order to clean up (remove object and executable files), run “make clean” in the terminal.

7. In order to format files, run “make format” in the terminal.

This is a part of a lab designed by Prof. Darrell Long.
