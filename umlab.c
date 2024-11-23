/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program. *32bit int 
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

typedef enum Um_register { r0 = 0, r1,


 r2, r3, r4, r5, r6, r7 } Um_register;



/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction instr = 0;
        instr = Bitpack_newu(instr, 4, 28, op);
        instr = Bitpack_newu(instr, 3, 6, ra);
        instr = Bitpack_newu(instr, 3, 3, rb);
        instr = Bitpack_newu(instr, 3, 0, rc);
        return instr;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction instr = 0;
        uint32_t op = 13;
        instr = Bitpack_newu(instr, 4, 28, op);
        instr = Bitpack_newu(instr, 3, 25, ra);
        instr = Bitpack_newu(instr, 25, 0, val);
        return instr;   
}       

/* Functions for working with streams */
static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}

/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}

Um_instruction division(Um_register a, Um_register b, Um_register c)
{
        return three_register(DIV, a, b, c);
}

Um_instruction multiplication(Um_register a, Um_register b, Um_register c) 
{
        return three_register(MUL, a, b, c);
}

Um_instruction conditionalMove(Um_register a, Um_register b, Um_register c) 
{
        return three_register(CMOV, a, b, c);
}

Um_instruction activate(Um_register a, Um_register b, Um_register c) {
        return three_register(ACTIVATE, a, b, c);
}

Um_instruction inactivate(Um_register a, Um_register b, Um_register c) {
        return three_register(INACTIVATE, a, b, c);
}

Um_instruction sload(Um_register a, Um_register b, Um_register c) {
        return three_register(SLOAD, a, b, c);
}

Um_instruction sstore(Um_register a, Um_register b, Um_register c) {
        return three_register(SSTORE, a, b, c);
}

Um_instruction nand(Um_register a, Um_register b, Um_register c) {
        return three_register(NAND, a, b, c);
}

Um_instruction input(Um_register c) {
        return three_register(IN, 0, 0, c);
}



/* ------------Unit tests for the UM------------- */

void build_halt_test(Seq_T s)
{
        append(s, halt());
}

void addition_test(Seq_T s)
{
        append(s, loadval(r1, 30));
        append(s, loadval(r2, 35));              /*65*/
        append(s, add(r3, r1, r2));
        append(s, output(r3));
        append(s, halt());
}

extern void division_test(Seq_T s)
{
        append(s, loadval(r1, 0));
        append(s, loadval(r2, 200));
        append(s, loadval(r3, 2));              /* 200/2 */
        append(s, division(r1, r2, r3));
        append(s, output(r1));
        append(s, halt());
}

extern void conditional_move_test(Seq_T s)
{
        append(s, loadval(r1, 2));
        append(s, loadval(r2, 60));
        append(s, loadval(r3, 1));
        append(s, conditionalMove(r1, r2, r3));
        append(s, output(r1));
        append(s, halt());
} 

extern void multip_test(Seq_T s)
{
        append(s, loadval(r1, 0));
        append(s, loadval(r2, 50));
        append(s, loadval(r3, 2));
        append(s, multiplication(r1, r2, r3));
        append(s, output(r1));
        append(s, halt());
}

extern void nand_test(Seq_T s) {  
        append(s, loadval(r2, 1));
        append(s, loadval(r3, 1));
        append(s, three_register(NAND, r1, r2, r3));
        //append(s, output(r1));                    
        append(s, halt()); 
}

extern void input_test(Seq_T s)
{
        append(s, loadval(r1, 99));
        append(s, three_register(IN, 0, 0, r1));
        append(s, output(r1));
        append(s, halt());
}

void segload_store_test(Seq_T s) {
        append(s, loadval(r1, 0));                /* index */
        append(s, loadval(r2, 1));                /* offset */
        append(s, loadval(r3, 99));               /* Value */

        /* Map segment of size 2 */
        append(s, loadval(r4, 2));
        append(s, three_register(ACTIVATE, r1, r0, r4));

        /* Store */
        append(s, three_register(SSTORE, r1, r2, r3));

        /* Load value back into r5 */
        append(s, three_register(SLOAD, r5, r1, r2));
        append(s, output(r5));                    /* Output: 99 */
        append(s, halt());
}

void segload_store_large_test(Seq_T s) {
        append(s, loadval(r1, 0));                /* index */
        append(s, loadval(r2, 1));                /* offset */
        append(s, loadval(r3, 99));               /* Value */

        append(s, loadval(r4, 0));                /* index */
        append(s, loadval(r5, 1));                /* offset */
        append(s, loadval(r6, 99));               /* Value */

        append(s, loadval(r7, 'a'));

        append(s, three_register(ACTIVATE, r1, r0, r7)); 
        append(s, three_register(SSTORE, r0, r2, r3));
        append(s, three_register(SLOAD, r5, r0, r2));

        append(s, three_register(ACTIVATE, r1, r3, r6)); 
        append(s, three_register(SSTORE, r3, r2, 7)); 
        append(s, three_register(SSTORE, r3, r2, r7)); 
        append(s, three_register(SLOAD, r5, r3, r2)); 
        append(s, output(r5));                    /* Output: 99 */
        append(s, halt());
}

void map_test(Seq_T s) {
        append(s, loadval(r1, 0));         
        append(s, loadval(r2, 10));        
        append(s, three_register(ACTIVATE, r1, r0, r2));
        append(s, output(r2));                         

        append(s, halt());
}

void build_add_halt_test(Seq_T stream)
{
        append(stream, add(r1, r2, r3));
        append(stream, halt());
}

void test_50_million_instr_no_output(Seq_T stream) 
{
        const uint32_t iterations = 50 * 1000 * 1000 / 5; 

        /* Initialization */
        append(stream, loadval(r1, 10));      /* r1 = 10 */
        append(stream, loadval(r2, 20));      /* r2 = 20 */
        append(stream, loadval(r3, 5));       /* r3 = 5 */
        append(stream, loadval(r4, 8));       /* r6 = 8 (map size) */
        append(stream, loadval(r5, 0));       /* r7 = 0 (segment ID) */

        /* Map a segment of size 8 for segment operations */
        append(stream, three_register(ACTIVATE, r5, r0, r4)); 

        for (uint32_t i = 0; i < iterations; i++) {
                /* Perform arithmetic operations */
                append(stream, three_register(ADD, r1, r1, r2));  
                append(stream, three_register(MUL, r3, r3, r1));  
                // append(stream, three_register(NAND, r4, r4, r1));
                append(stream, three_register(SLOAD, r3, r5, r4));

                
                append(stream, three_register(SSTORE, r5, r4, r3));
                append(stream, three_register(SLOAD, r3, r5, r4));   
                }   

        append(stream, three_register(HALT, 0, 0, 0));
}