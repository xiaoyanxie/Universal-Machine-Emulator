/**************************************************************  
*  
*     Program:    UM
*     Assignment: CS 40 HW06 - UM
*     Authors:    Xiaoyan and Rene
*     Date:       11/18/2024
*     Summary:    UM is an emulator which interprets and executes a 
*                 sequence of instructions encoded in a 32-bit binary format. 
*                 The emulator simulates memory, registers, and instruction 
*                 processing to replicate the behavior of a physical 
*                 computational machine.
*
*************************************************************/



/***************************************************************    
*                       Acknowledgement
***************************************************************/

TA Darya and Bill hleped us a lot during Um design and implementation.

We referred stackoverflow when we decided to use switch instead of if-else for
instruction driver.
https://stackoverflow.com/questions/767821/is-else-if-faster-than-switch-case

We referred stackoverflow when we tried to retrieve the size of input file.
https://stackoverflow.com/questions/8236/how-do-you-determine-the-size-of-a-
file-in-c

We referred to hex and binary converter websties.
https://www.rapidtables.com/convert/number/hex-dec-bin-converter.html



/***************************************************************    
*                         TODOs  
***************************************************************/
We implemented all the funconalities required. The program runs smoothly
with no error. Midmark costs 12 seconds to run.



/***************************************************************    
*             Significant departures from design
***************************************************************/
Every part of our implementation is in line with our design. 


/***************************************************************    
*                       Architecture
***************************************************************/
Modules and abstractions:
1.  Memory Management Module: 
This handles segmented memory, providing 
functions to create, delete, access, and copy memory segments. It also 
manages segment IDs and recycles unused IDs, keeping the details of memory 
organization hidden from other parts of the system.

Abstraction and secrets: 
- Represents segmented memory as a collection of arrays.
- Abstracts low-level details of memory allocation, resizing, and 
recycling of segment IDs, which are the secrets that other modules don't 
know about.
- Provides a clear interface for creating, accessing, modifying, 
and freeing memory segments.

Interaction:
Memory module provides interface for instructions module to execute certain
instructions. It also stores the program in segment 0, which is read from 
the file in driver module.

2. Instruction Execution Module: 
This module decodes and runs instructions. 
It works with registers, extracts instruction fields (like opcode and 
registers ra, rb, rc), and performs basic operations like arithmetic and 
conditional moves. It uses memory and updates the program's state using a 
program counter and registers.

Abstraction and secrets: 
- Encapsulates the execution logic for each opcode, including arithmetic, 
logic, memory operations, and control flow to make them secrets.
- Hides the complexity of extracting fields like opcodes and register values 
from binary instructions, which other modules don't know about.
- Provides a clean interface to execute individual instructions without 
exposing the underlying representation.

Interaction: 
Instructions module interacts with both memory module and 
driver module. The instruction driver is called in driver module to execute
instructions, part of which triggers the interactions between instructions 
module and memory module, processing the instruction by manipulating
the memory.

3. Driver Module: 
This module ties everything together. It validates and 
loads input files, sets up memory, and controls the execution loop by 
working with the memory and instruction modules.

Abstraction and secrets:
- Serves as the central control unit for the system.
- Abstracts the process of loading a program into memory, managing 
the execution loop, and integrating the memory and instruction modules.

Interaction:
Diver module is the entry point of the Um program. It stores the
instructions into memory and control the instructions module for execution.
It also responsable for freeing memory.
Memory operations are abstracted and utilized by both the driver and 
instruction modules, ensuring clean separation of responsibilities while 
maintaining tight integration for program execution.

/***************************************************************    
*                   50 million instructions test
***************************************************************/

It takes approximately 17 seconds for our program to perform 50 million
instructions. 

We figured this out by writing a unit test which contains 50 million
instructions and ran cpu-limited command to test at which time limit we
exceeds. After trying out a few different times, we found out that our
program would not exceed 17 seconds but would exceed 16 seconds. 


/***************************************************************    
*                       Unit tests
***************************************************************/

+-----------------------+------------------------+------------------+--------+
| Test Name             | Purpose                | Instructions     | Output |
+-----------------------+------------------------+------------------+--------+
| test_halt             | test halt individually | halt             | none   |
+-----------------------+------------------------+------------------+--------+
| test_add-halt         | test addition and      | load_val,        | none   |
|                       | then halt              | addition,        |        |
|                       |                        | halt             |        |
+-----------------------+------------------------+------------------+--------+
| test_addition         | test outputing         | load_val,        | A      |
|                       | result of addition:    | addition,        |        |
|                       | 30 + 35                | output,          |        |
|                       |                        | halt             |        |
+-----------------------+------------------------+------------------+--------+
| test_division         | test outputting        | load_val,        | d      |
|                       | result of division:    | division,        |        |
|                       | 200 / 2                | output,          |        |
|                       |                        | halt             |        |
+-----------------------+------------------------+------------------+--------+
| test_multiplication   | test outputting        | load_val,        | d      |
|                       | result of              | multiplication,  |        |
|                       | multiplication:        | output,          |        |
|                       | 50 * 2                 | halt             |        |
+-----------------------+------------------------+------------------+--------+
| test_nand             | test nand:             | load_val,        | none   |
|                       | 0 nand 0               | nand,            |        |
|                       |                        | halt             |        |
+-----------------------+------------------------+------------------+--------+
| test_input            | test outputting        | load_val,        | c      |
|                       | result of input "c"    | input,           |        |
|                       |                        | output,          |        |
|                       |                        | halt             |        |
+-----------------------+------------------------+------------------+--------+
| test_conditional_move | test cv when c is      | load_val,        | <      |
|                       | not 0                  | conditional_mov, |        |
|                       |                        | output,          |        |
|                       |                        | halt             |        |
+-----------------------+------------------------+------------------+--------+
| test_map              | test map               | load_val,        | \n     |
|                       |                        | map,             |        |
|                       |                        | output,          |        |
|                       |                        | halt             |        |
+-----------------------+------------------------+------------------+--------+
| test_segload_store    | test segmented load    | load_val,        | c      |
|                       | and segment store      | map,             |        |
|                       |                        | seg_store,       |        |
|                       |                        | seg_load,        |        |
|                       |                        | output,          |        |
|                       |                        | halt             |        |
+-----------------------+------------------------+------------------+--------+
| test_segload_large    | test segmented load    | load_val,        | a      |
|                       | and segmented store    | map,             |        |
|                       | with more registers    | seg_store,       |        |
|                       | involved by repeatedly | seg_load,        |        |
|                       | calling seg_load       | output,          |        |
|                       | and seg_store          | halt             |        |
+-----------------------+------------------------+------------------+--------+
| test_50_million       | test when there        | load_val,        | none   |
|                       | are 50 million         | map,             |        |
|                       | instructions           | addition,        |        |
|                       |                        | multiplication,  |        |
|                       |                        | seg_store,       |        |
|                       |                        | seg_load,        |        |
|                       |                        | load_program,    |        |
|                       |                        | halt             |        |
+-----------------------+------------------------+------------------+--------+


/***************************************************************    
*                   Time
***************************************************************/
Time for analyzing: 4h 
Time for preparing design: 5h
Time for problem solving: 19h
Total: 28h

