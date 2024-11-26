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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include "seq.h"
#include "uarray.h"

typedef struct A A;

/********** Function Declarations ********/

/* General purpose functions */
static inline uint32_t get_opcode(uint32_t word);


/* Memory Functions */
// static inline T init_memory(uint32_t element_size, uint32_t segment_size,
//                             uint32_t file_size);
static inline void free_memory(A mem);
static inline uint32_t seg_new(A mem, uint32_t element_size, uint32_t segment_size);
static inline void seg_free(A mem, uint32_t id);
static inline uint32_t seg_at(A mem, uint32_t id, uint32_t offset);
static inline void seg_copy(A mem, uint32_t id);
static inline void seg_store(A mem, uint32_t id, uint32_t offset, uint32_t word);
// static inline uint32_t seg_size(T mem, uint32_t id);
// static inline uint32_t get_id(T mem);

/* Array Functions */

static inline void instructions_driver(A mem);

struct A
{
        uint32_t ele_size; /* size of element = 4 */
        uint32_t mem_num_ele; /* number of segments present? */
        uint32_t unmap_num_ele; /* number of unmapped ids present */

        uint32_t mem_length; /* initialized number of segments */
        uint32_t unmap_length; /* initialized number of unmapped ids in unmap array */

        uint32_t **mem_array; /* array of segments of instructions */
        uint32_t *unmap_array; /* unmapped ids */
};

static inline uint32_t input();

/* Bitpack Functions */

static inline uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb);
static inline uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                                    uint64_t value);

static inline void instructions_driver(A mem)
{
        /* initialize program counter */
        uint32_t counter = 1;

        /* initialize all registers to 0 */
        uint32_t regi[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        /* get first word */
        uint32_t word = mem.mem_array[0][counter];

        uint32_t instr = get_opcode(word);
        uint32_t size = mem.mem_array[0][0];
        /* go though all instructions */
        while ((instr != 7))
        {

                switch (instr)
                {
                case 0: /*Conditional move*/
                {
                        uint32_t reg_a = Bitpack_getu(word, 3, 6);
                        uint32_t reg_b = Bitpack_getu(word, 3, 3);
                        uint32_t reg_c = Bitpack_getu(word, 3, 0);
                        // regi[reg_a] = conditional_move(reg_a, reg_b, reg_c);
                        if (regi[reg_c] != 0)
                        {
                                regi[reg_a] = regi[reg_b];
                        }
                        break;
                }

                case 1: /*Segmented load*/
                {
                        uint32_t reg_a = Bitpack_getu(word, 3, 6);
                        uint32_t reg_b = Bitpack_getu(word, 3, 3);
                        uint32_t reg_c = Bitpack_getu(word, 3, 0);
                        regi[reg_a] = seg_at(mem, regi[reg_b], regi[reg_c]);
                        break;
                }

                case 2: /*Segmented store*/
                {
                        uint32_t reg_a = Bitpack_getu(word, 3, 6);
                        uint32_t reg_b = Bitpack_getu(word, 3, 3);
                        uint32_t reg_c = Bitpack_getu(word, 3, 0);
                        seg_store(mem, regi[reg_a], regi[reg_b], regi[reg_c]);
                        break;
                }

                case 3: /*Addition*/
                {
                        uint32_t reg_a = Bitpack_getu(word, 3, 6);
                        uint32_t reg_b = Bitpack_getu(word, 3, 3);
                        uint32_t reg_c = Bitpack_getu(word, 3, 0);
                        regi[reg_a] = regi[reg_b] + regi[reg_c];
                        break;
                }

                case 4: /*Multiplication*/
                {
                        uint32_t reg_a = Bitpack_getu(word, 3, 6);
                        uint32_t reg_b = Bitpack_getu(word, 3, 3);
                        uint32_t reg_c = Bitpack_getu(word, 3, 0);
                        regi[reg_a] = regi[reg_b] * regi[reg_c];
                        break;
                }

                case 5: /* division */
                {
                        uint32_t reg_a = Bitpack_getu(word, 3, 6);
                        uint32_t reg_b = Bitpack_getu(word, 3, 3);
                        uint32_t reg_c = Bitpack_getu(word, 3, 0);
                        assert(regi[reg_c] != 0);
                        regi[reg_a] = regi[reg_b] / regi[reg_c];
                        break;
                }

                case 6: /*NAND*/
                {
                        uint32_t reg_a = Bitpack_getu(word, 3, 6);
                        uint32_t reg_b = Bitpack_getu(word, 3, 3);
                        uint32_t reg_c = Bitpack_getu(word, 3, 0);
                        regi[reg_a] = ~(regi[reg_b] & regi[reg_c]);
                        break;
                }

                case 8: /*map*/
                {
                        uint32_t reg_b = Bitpack_getu(word, 3, 3);
                        uint32_t reg_c = Bitpack_getu(word, 3, 0);
                        regi[reg_b] = seg_new(mem, sizeof(uint32_t), regi[reg_c]);
                        break;
                }

                case 9: /*unmap*/
                {
                        uint32_t reg_c = Bitpack_getu(word, 3, 0);
                        seg_free(mem, regi[reg_c]);
                        break;
                }

                case 10: /*output*/
                {
                        uint32_t reg_c = Bitpack_getu(word, 3, 0);
                        assert(regi[reg_c] <= 255);
                        printf("%c", regi[reg_c]);
                        break;
                }

                case 11: /*input*/
                {
                        uint32_t reg_c = Bitpack_getu(word, 3, 0);
                        regi[reg_c] = input();
                        break;
                }

                case 12: /*loadp*/
                {
                        uint32_t reg_b = Bitpack_getu(word, 3, 3);
                        uint32_t reg_c = Bitpack_getu(word, 3, 0);
                        if (regi[reg_b] != 0)
                        {
                                seg_copy(mem, regi[reg_b]);
                        }
                        counter = regi[reg_c]; 
                        //size = seg_size(mem, regi[reg_b]);
                        size = mem.mem_array[0][0];
                        break;
                }

                case 13: /*load val*/
                {
                        uint32_t reg_a = Bitpack_getu(word, 3, 25);
                        regi[reg_a] = Bitpack_getu(word, 25, 0);
                        break;
                }
                case 7:
                        break;
                }

                /* get next instruction */
                counter++;

                if (counter > size)
                {
                        break;
                }
                word = mem.mem_array[0][counter];
                instr = get_opcode(word);
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
static inline uint32_t get_opcode(uint32_t word)
{
        uint32_t opcode = Bitpack_getu(word, 4, 28);
        // assert(opcode < 14);

        return opcode;
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
        // assert(in <= 255);
        if (in == (uint32_t)EOF)
        {
                in = 0xffffffff;
        }
        return in;
}

int main(int argc, char *argv[])
{
        /* verify command line */
        if (argc != 2)
        {
                fprintf(stderr, "Incorrect number of arguments\n");
                return EXIT_FAILURE;
        }

        char *file_name = argv[1];

        /* retrieve infomation about the file using system call stat*/
        struct stat file_info;
        stat(file_name, &file_info);
        int file_size = file_info.st_size / 4;

        /* intialize memory module */
        A mem;

        mem.ele_size = sizeof(uint32_t);
        mem.unmap_length = 400;
        mem.mem_length = 2000;
        
        uint32_t *unmapped_ID = calloc(mem.unmap_length, sizeof(uint32_t));
        assert(unmapped_ID != NULL);
        uint32_t **mem_wrapper = calloc(mem.mem_length, sizeof(uint32_t*));
        assert(mem_wrapper != NULL);
        uint32_t *segment = calloc(file_size + 1, sizeof(uint32_t));
        assert(segment != NULL);

        mem_wrapper[0] = segment;
        segment[0] = file_size;

        mem.mem_array = mem_wrapper;
        mem.unmap_array = unmapped_ID;
        mem.mem_num_ele = 1;
        mem.unmap_num_ele = 0;

        /* open_and_read */
        FILE *file = fopen(file_name, "rb");
        if (file == NULL) {
                fprintf(stderr, "%s: No such file or directory\n", file_name);
                exit(EXIT_FAILURE);
        }

        for (uint32_t i = 0; i < (uint32_t)file_size; i++) {
                uint32_t word = 0;
                uint32_t byte;
                byte = fgetc(file);
                word |= byte << 24;
                byte = fgetc(file);
                word |= byte << 16;
                byte = fgetc(file);
                word |= byte << 8;
                byte = fgetc(file);
                word |= byte;

                /* store word into segment 0 */
                seg_store(mem, 0, i, word);
        }

        fclose(file);

        /* run instructions module */
        instructions_driver(mem);

        /* free all memory in memory module */
        free_memory(mem);

        return EXIT_SUCCESS;
}

static inline uint32_t seg_new(A mem, uint32_t element_size, uint32_t segment_size)
{
        uint32_t *segment = calloc(segment_size + 1, element_size);
        assert(segment != NULL);

        /* get id */
        uint32_t id;
        if (mem.unmap_num_ele == 0)
        {
                /*check mem array size*/
                if (mem.mem_num_ele == mem.mem_length) {
                        uint32_t **new_mem_array = calloc(2 * mem.mem_length, sizeof(uint32_t *));
                        assert(new_mem_array != NULL);
                        mem.mem_array = new_mem_array;
                        mem.mem_length *= 2;
                        
                        uint32_t size = mem.mem_length;
                        for (uint32_t i = 0; i < size; i++) {
                                new_mem_array[i] = mem.mem_array[i];
                        }
                        free(mem.mem_array);
                        mem.mem_array = new_mem_array;
                }
                
                id = mem.mem_num_ele - 1;
        } else {
                id = (mem.unmap_array[mem.unmap_num_ele - 1]);
                mem.unmap_num_ele--;
        }

        mem.mem_array[id] = segment;
        segment[0] = segment_size;
        mem.mem_num_ele++;
        
        return id;
}


static inline uint32_t seg_at(A mem, uint32_t id, uint32_t offset)
{
        assert(id < mem.mem_length);
        //printf("given offset in seg_at is %u and stored segment length is %u\n", offset, mem.mem_array[id][0]);
        assert(offset < mem.mem_array[id][0]);
        uint32_t instruction = mem.mem_array[id][offset + 1];
        //assert(instruction != NULL);
        return instruction;
}

static inline void seg_store(A mem, uint32_t id, uint32_t offset, uint32_t word)
{
        assert(id < mem.mem_length);
        assert(offset < mem.mem_array[id][0]);
        mem.mem_array[id][offset + 1] = word;
}

static inline void seg_copy(A mem, uint32_t id)
{
        assert(id < mem.mem_length);

        /* if ID is 0, directly return */
        if (id == 0) {
                return;
        }
        
        uint32_t *old_array = mem.mem_array[id];
        assert(old_array != NULL);

        uint32_t length = mem.mem_array[id][0];

        uint32_t *new_array = calloc(length + 1, sizeof(uint32_t));
        assert(new_array != NULL);
        memcpy(new_array, old_array, (length + 1) * sizeof(uint32_t));

        /* free original segment 0 */
        uint32_t *seg0 = mem.mem_array[0];
        if (seg0 != NULL) {
                free(seg0); 
        }

        /* put in new segment 0 */
        mem.mem_array[0] = new_array;
}

static inline void seg_free(A mem, uint32_t id)
{
        assert(id < mem.mem_length);

        uint32_t *segment = mem.mem_array[id];
        mem.mem_array[id] = NULL;
        if (segment != NULL) {
                free(segment);
        }

        /* recycle id */
        if (mem.unmap_num_ele == mem.unmap_length) {
                uint32_t *new_unmap_array = calloc(2 * mem.unmap_length, sizeof(uint32_t));
                assert(new_unmap_array != NULL);
                mem.unmap_array = new_unmap_array;
                mem.unmap_length *= 2;
                
                uint32_t size = mem.unmap_length;
                for (uint32_t i = 0; i < size; i++) {
                        new_unmap_array[i] = mem.unmap_array[i];
                }
                free(mem.unmap_array);
                mem.unmap_array = new_unmap_array;
        }
        mem.unmap_array[mem.unmap_num_ele] = id;
        mem.unmap_num_ele++;
        mem.mem_num_ele--;
}

static inline void free_memory(A mem)
{

        /* freeing arrays of segments */
        uint32_t size = mem.mem_length;
        for (uint32_t i = 0; i < size; i++) {
                uint32_t *segment = mem.mem_array[i];
                if (segment != NULL) {
                        free(segment);
                }
        }

        /* freeing array of unmapped id */
        if (mem.unmap_array != NULL) {
                free(mem.unmap_array);
        }

        /* freeing memory array */
        free(mem.mem_array);
}


static inline uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb) 
{
        uint64_t mask = ~0;
        uint64_t new_mask = mask >> (64 - width) << lsb;
        return ((word & new_mask) >> lsb);
}

static inline uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value) {
    // assert(width <= 64);
    unsigned hi = lsb + width; /* one beyond the most significant bit */
    // assert(hi <= 64);

    // assert((value >> width) == 0); 

    return ((word >> hi) << hi)              /* high part */
           | ((word << (64 - lsb)) >> (64 - lsb)) /* low part */
           | (value << lsb);                     /* new part */
}




