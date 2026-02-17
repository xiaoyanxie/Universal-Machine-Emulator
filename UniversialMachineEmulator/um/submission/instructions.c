/*
 *      instructions.c
 *
 *      Assignment: CS 40 HW6 - um
 *      Author:     Xiaoyan Xie (xxie05) and Rene Zhao (nzhao05)
 *      Date:       11/19/24
 *      
 *      Summary: This file has the implementation of um's instruction module,
 *      which manages instruction operations on the eight register values. 
 *      The functions in instruction.c calls functions from memory.c for 
 *      memory-related instructions, and the main instruction driver is called 
 *      in driver.c.
 */ 

#include "instructions.h"

/********** Function Declarations ********/

/* General purpose functions */
uint32_t get_opcode(uint32_t word);
uint32_t get_ra(uint32_t word);
uint32_t get_rb(uint32_t word);
uint32_t get_rc(uint32_t word);
uint32_t load_value(uint32_t word);


/******** Inline operations ********/
static inline uint32_t conditional_move(uint32_t ra, uint32_t rb, uint32_t rc);
static inline uint32_t addition(uint32_t rb, uint32_t rc);
static inline uint32_t multiplication(uint32_t rb, uint32_t rc);
static inline uint32_t division(uint32_t rb, uint32_t rc);
static inline uint32_t bitwise_nand(uint32_t rb, uint32_t rc);
static inline uint32_t input();


/********** Type Declarations ********/

/* Enumeration for opcodes */
typedef enum Um_opcode {
        CMOV, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/********** instructions_driver ********
 *
 * Executes the Universal Machine program by interpreting instructions
 * stored in memory, managing registers, and updating the program counter.
 *
 * Parameters:
 *      T mem: The memory struct containing program segments.
 *
 * Return:
 *      None.
 *
 * Expects:
 *      - The program counter does not exceed the size of segment 0.
 *      - Memory is correctly initialized and accessible.
 *
 * Notes:
 *      - CRE occurs if the instruction doesn't belong to the implemented
 *        instructions.
 *
 ************************/
void instructions_driver(T mem)
{
        /* initialize program counter */
        uint32_t counter = 0;

        /* initialize all registers to 0 */
        uint32_t regi[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        /* get first word */
        uint32_t word = seg_at(mem, 0, counter);

        /* get opcode of first word */
        Um_opcode instr = get_opcode(word);

        /* get indexes a, b and c */
        uint32_t ra = get_ra(word);
        uint32_t rb = get_rb(word);
        uint32_t rc = get_rc(word);

        /* go though all instructions */
        while ((instr != HALT)) {
 
                switch(instr)
                {
                        case CMOV:
                                regi[ra] = conditional_move(
                                        regi[ra], regi[rb], regi[rc]);
                                break;
                        case SLOAD:
                                regi[ra] = seg_at(mem, regi[rb], regi[rc]);
                                break;
                        case SSTORE:
                                seg_store(mem, regi[ra], regi[rb], regi[rc]);
                                break;
                        case ADD:
                                regi[ra] = addition(regi[rb], regi[rc]);
                                break;
                        case MUL:
                                regi[ra] = multiplication(regi[rb], regi[rc]);
                                break;
                        case DIV:
                                regi[ra] = division(regi[rb], regi[rc]);
                                break;
                        case NAND:
                                regi[ra] = bitwise_nand(regi[rb], regi[rc]);
                                break;                              
                        case ACTIVATE:
                                regi[rb] = seg_new(mem, 4, regi[rc]);
                                break;
                        case INACTIVATE: 
                                seg_free(mem, regi[rc]);
                                break;
                        case OUT:
                                printf("%c", regi[rc]);
                                break;
                        case IN:
                                regi[rc] = input();
                                break;
                        case LOADP: 
                                seg_copy(mem, regi[rb]);
                                counter = regi[rc] - 1;
                                break;
                        case LV:
                                regi[ra] = load_value(word);
                                break;
                        case HALT:
                                break;
                        default:
                                fprintf(stderr, "Unknown opcode: %u\n", instr);
                                exit(1);

                }

                /* get next instruction */
                counter++;
                if (counter >= seg_size(mem, 0)) {
                        break;
                }
                word = seg_at(mem, 0, counter);
                instr = get_opcode(word);
                ra = get_ra(word);
                rb = get_rb(word);
                rc = get_rc(word);

        }
        
}

/********** get_opcode ********
 *
 * Extracts the opcode from a 32-bit instruction word.
 *
 * Parameters:
 *      uint32_t instruction: The 32-bit instruction word.
 *
 * Return:
 *      uint32_t: The extracted opcode.
 *
 * Expects:
 *      - Opcode must be within the valid range [0, 13].
 *
 * Notes:
 *      - CRE occurs if the opcode is bigger than 13.
 *
 ************************/
uint32_t get_opcode(uint32_t word)
{
        uint32_t opcode = Bitpack_getu(word, 4, 28);
        assert(opcode < 14);

        return opcode;
}

/********** get_ra ********
 *
 * Extracts the register a field from a 32-bit instruction word.
 *
 * Parameters:
 *      uint32_t instruction: The 32-bit instruction word.
 *
 * Return:
 *      uint32_t: The extracted register a field.
 *
 * Expects:
 *      - None.
 *
 * Notes:
 *      - None.
 *
 ************************/
uint32_t get_ra(uint32_t word)
{
        if (get_opcode(word) == 13) {
                return Bitpack_getu(word, 3, 25); 
        }
        return Bitpack_getu(word, 3, 6);
}

/********** get_rb ********
 *
 * Extracts the register b field from a 32-bit instruction word.
 *
 * Parameters:
 *      uint32_t instruction: The 32-bit instruction word.
 *
 * Return:
 *      uint32_t: The extracted register b field.
 *
 * Expects:
 *      - None.
 *
 * Notes:
 *      - None.
 *
 ************************/
uint32_t get_rb(uint32_t word)
{
        return Bitpack_getu(word, 3, 3);
}

/********** get_rc ********
 *
 * Extracts the register c field from a 32-bit instruction word.
 *
 * Parameters:
 *      uint32_t instruction: The 32-bit instruction word.
 *
 * Return:
 *      uint32_t: The extracted register c field.
 *
 * Expects:
 *      - None.
 *
 * Notes:
 *      - None.
 *
 ************************/
uint32_t get_rc(uint32_t word)
{
        return Bitpack_getu(word, 3, 0);
}

/********** load_value ********
 *
 * Extracts the value from the lower 25 bits of a 32-bit instruction word.
 *
 * Parameters:
 *      uint32_t instruction: The 32-bit instruction word.
 *
 * Return:
 *      uint32_t: The extracted 25-bit value.
 *
 * Expects:
 *      None.
 *
 * Notes:
 *      - None.
 *************************/
uint32_t load_value(uint32_t word)
{
        uint32_t value = Bitpack_getu(word, 25, 0);
        return value;
}

/********** conditional_move ********
 *
 * Performs a conditional move: if the value in register c is non-zero,returns
 * the value in register b; otherwise, returns value in register a.
 *
 * Parameters:
 *      uint32_t regA: Current value in register A.
 *      uint32_t regB: Value in register B.
 *      uint32_t regC: Value in register C.
 *
 * Return:
 *      uint32_t: The updated value for register A.
 *
 *  Expects:
 *      None.
 *
 * Notes:
 *      - None.
 ************************/
static inline uint32_t conditional_move(uint32_t ra, uint32_t rb, uint32_t rc)
{
        if (rc != 0) {
                return rb;
        }
        return ra;
}

/********** addition ********
 *
 * Adds two unsigned 32-bit integers and returns the result.
 *
 * Parameters:
 *      uint32_t rb: value in register b.
 *      uint32_t rc: value in register c.
 *
 * Return:
 *      uint32_t: The sum of the two operands.
 *
 * Expects:
 *      - None.
 *
 * Notes:
 *      - None.
 *
 ************************/
static inline uint32_t addition(uint32_t rb, uint32_t rc)
{
        uint32_t result = rb + rc;
        return result;
}

/********** multiplication ********
 *
 * Multiplies two unsigned 32-bit integers and returns the result.
 *
 * Parameters:
 *      uint32_t rb: value in register b.
 *      uint32_t rc: value in register c.
 *
 * Return:
 *      uint32_t: The product of the two operands.
 *
 * Expects:
 *      - None
 *
 * Notes:
 *      - None
 ************************/
static inline uint32_t multiplication(uint32_t rb, uint32_t rc)
{
        uint32_t result = rb * rc;
        return result;
}

/********** division ********
 *
 * Divides one unsigned 32-bit integer by another and returns the result.
 *
 * Parameters:
 *      uint32_t rb: The dividend (value in register b).
 *      uint32_t rc: The divisor (value in register c).
 *
 * Return:
 *      uint32_t: The quotient of the division.
 *
 * Expects:
 *      - rc (the divisor) must not be 0.
 *
 * Notes:
 *      - CRE occurs if rc is 0.
 *
 ************************/
static inline uint32_t division(uint32_t rb, uint32_t rc)
{
        assert(rc != 0);

        uint32_t result = rb / rc;
        return result;
}

/********** bitwise_nand ********
 *
 * Performs a bitwise NAND operation on two unsigned 32-bit integers and
 * returns the result.
 *
 * Parameters:
 *      uint32_t rb: value in register b.
 *      uint32_t rc: value in register c.
 *
 * Return:
 *      uint32_t: The result of ~(rb & rc).
 *
 * Expects:
 *      - None.

 ************************/
static inline uint32_t bitwise_nand(uint32_t rb, uint32_t rc)
{
        uint32_t result = ~(rb & rc);
        return result;
}

/********** input ********
 *
 * Reads a single byte from stdin and returns it as an unsigned 32-bit 
 * integer. If EOF is reached, returns a word with all bits set 
 * (0xFFFFFFFF).
 *
 * Parameters:
 *      None.
 *
 * Return:
 *      uint32_t: The input byte or EOF marker.
 *
 * Expects:
 *      - Input byte must be within the range [0, 255].
 *
 * Notes:
 *      - CRE occurs if the input byte is outside the range [0, 255].
 *
 ************************/
static inline uint32_t input()
{
        uint32_t in = fgetc(stdin);
        assert(in <= 255);
        if (in == (uint32_t)EOF) {
                in = 0xffffffff;
        }
        return in;
}