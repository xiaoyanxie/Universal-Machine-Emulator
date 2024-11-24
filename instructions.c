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
#include "seq.h"
#include "uarray.h"
#include <stdbool.h>

typedef struct T *T;

/********** Function Declarations ********/

/* General purpose functions */
static inline uint32_t get_opcode(uint32_t word);
// static inline uint32_t get_ra(uint32_t word);
static inline uint32_t get_rb(uint32_t word);
static inline uint32_t get_rc(uint32_t word);
static inline uint32_t load_value(uint32_t word);

/* Driver Functions */
static inline void open_and_read(char *file_name, T memory, uint32_t file_size);

/* Memory Functions */
static inline T init_memory(uint32_t element_size, uint32_t segment_size, 
                     uint32_t file_size);
static inline void free_memory(T mem);
static inline uint32_t seg_new(T mem, uint32_t element_size, uint32_t segment_size);
static inline void seg_free(T mem, uint32_t id);
static inline uint32_t seg_at(T mem, uint32_t id, uint32_t offset);
static inline void seg_copy(T mem, uint32_t id);
static inline void seg_store(T mem, uint32_t id, uint32_t offset, uint32_t word);
static inline uint32_t seg_size(T mem, uint32_t id);
static inline uint32_t get_id(T mem);

static inline void instructions_driver(T mem);
static inline void open_and_read(char *file_name, T memory, uint32_t file_size);

struct T {
        Seq_T Segments_wrapper;
        Seq_T Unmapped_ID;
};

/* Instructions Functions */
static inline uint32_t conditional_move(uint32_t ra, uint32_t rb, uint32_t rc);
static inline uint32_t addition(uint32_t rb, uint32_t rc);
static inline uint32_t multiplication(uint32_t rb, uint32_t rc);
static inline uint32_t division(uint32_t rb, uint32_t rc);
static inline uint32_t bitwise_nand(uint32_t rb, uint32_t rc);
static inline uint32_t input();

/* Bitpack Functions */
// Except_T Bitpack_Overflow = { "Overflow packing bits" };
static inline uint64_t shl(uint64_t word, unsigned bits);
static inline uint64_t shr(uint64_t word, unsigned bits);
static inline uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb);
static inline uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value);
static inline bool Bitpack_fitsu(uint64_t n, unsigned width);


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
static inline void instructions_driver(T mem)
{
        /* initialize program counter */
        uint32_t counter = 0;

        /* initialize all registers to 0 */
        uint32_t regi[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        /* get first word */
        uint32_t word = seg_at(mem, 0, counter);

        /* get opcode of first word */
        uint32_t instr = get_opcode(word);

        /* get indexes a, b and c */
        uint32_t ra = Bitpack_getu(word, 3, 6);
        uint32_t rb = get_rb(word);
        uint32_t rc = get_rc(word);

        /* go though all instructions */
        while ((instr != 7)) {
 
                switch(instr)
                {
                        case 0:
                                regi[ra] = conditional_move(
                                        regi[ra], regi[rb], regi[rc]);
                                break;
                        case 1:
                                regi[ra] = seg_at(mem, regi[rb], regi[rc]);
                                break;
                        case 2:
                                seg_store(mem, regi[ra], regi[rb], regi[rc]);
                                break;
                        case 3:
                                regi[ra] = regi[rb] + regi[rc];
                                break;
                        case 4:
                                regi[ra] = multiplication(regi[rb], regi[rc]);
                                break;
                        case 5:
                                regi[ra] = division(regi[rb], regi[rc]);
                                break;
                        case 6:
                                regi[ra] = bitwise_nand(regi[rb], regi[rc]);
                                break;                              
                        case 8:
                                regi[rb] = seg_new(mem, 4, regi[rc]);
                                break;
                        case 9: 
                                seg_free(mem, regi[rc]);
                                break;
                        case 10:
                                printf("%c", regi[rc]);
                                break;
                        case 11:
                                regi[rc] = input();
                                break;
                        case 12: 
                                seg_copy(mem, regi[rb]);
                                counter = regi[rc] - 1;
                                break;
                        case 13:
                                regi[Bitpack_getu(word, 3, 25)] = load_value(word);
                                break;
                        case 7:
                                break;
                        // default:
                        //         fprintf(stderr, "Unknown opcode: %u\n", instr);
                        //         exit(1);

                }

                /* get next instruction */
                counter++;
                if (counter >= seg_size(mem, 0)) {
                        break;
                }
                word = seg_at(mem, 0, counter);
                instr = get_opcode(word);
                ra = Bitpack_getu(word, 3, 6);
                rb = Bitpack_getu(word, 3, 3);
                rc = Bitpack_getu(word, 3, 0);

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
// static inline uint32_t get_ra(uint32_t word)
// {
//         if (get_opcode(word) == 13) {
//                 return Bitpack_getu(word, 3, 25); 
//         }
//         return Bitpack_getu(word, 3, 6);
// }

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
static inline uint32_t get_rb(uint32_t word)
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
static inline uint32_t get_rc(uint32_t word)
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
static inline uint32_t load_value(uint32_t word)
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
        // assert(in <= 255);
        if (in == (uint32_t)EOF) {
                in = 0xffffffff;
        }
        return in;
}

int main(int argc, char *argv[])
{
        /* verify command line */
        if (argc != 2) {
                fprintf(stderr, "Incorrect number of arguments\n");
                return EXIT_FAILURE;
        }

        char *file_name = argv[1];
        
        /* retrieve infomation about the file using system call stat*/
        struct stat file_info;
        stat(file_name, &file_info);

        /* obtain file size */
        assert(file_info.st_size % 4 == 0);
        int file_size = file_info.st_size / 4;

        /* intialize memory module */
        T mem = init_memory(sizeof(uint32_t), file_size, 1);
        assert(mem != NULL);
        
        /* read file */
        open_and_read(file_name, mem, file_size);

        /* run instructions module */
        instructions_driver(mem);

        /* free all memory in memory module */
        free_memory(mem);
        
        return EXIT_SUCCESS;
}


static inline void open_and_read(char *file_name, T memory, uint32_t file_size)
{
        assert(memory != NULL);

        /* open and verify file */
        FILE *file = fopen(file_name, "rb");
        if (file == NULL) {
                fprintf(stderr, "%s: No such file or directory\n", file_name);;
                exit(EXIT_FAILURE);
        }

        assert(file_size > 0);
        for (uint32_t i = 0; i < file_size; i++) {
                uint32_t word = 0;
                /* read 4 bytes (1 word) from the file */
                for (int j = 0; j < 4; j++) {
                        int byte = fgetc(file);
                        word = Bitpack_newu(word, 8, 24 - (j * 8), byte);
                }
                /* store word into segment 0 */
                seg_store(memory, 0, i, word);
                
        }

        fclose(file);
}

static inline T init_memory(uint32_t element_size, uint32_t segment_size, 
        uint32_t num_segments)
{
        /* initialize memory struct */
        T mem = malloc(sizeof(struct T));
        assert(mem != NULL);

        /* 
         * initialize sequence of unmapped id 
         * note: since sequence expands dynamically, the initial size of 
         * sequence could be other than 100.
         */
        Seq_T unmapped_ID = Seq_new(100);
        assert(unmapped_ID != NULL);

        /* initialize sequence of segment wrapper */
        Seq_T mem_wrapper = Seq_new(num_segments);
        assert(mem_wrapper != NULL);

        /* initialize mapped segments, each as a UArray */
        for(uint32_t i = 0; i < num_segments; i++) {
                UArray_T segment = UArray_new(segment_size, element_size);
                assert(segment != NULL);
                
                Seq_addhi(mem_wrapper, segment);
        }
        
        /* return memory struct */
        mem->Segments_wrapper = mem_wrapper;
        mem->Unmapped_ID = unmapped_ID;
        return mem;
}

/********** seg_new ********
 *
 * Creates a new segment in memory.
 *
 * Parameters:
 *      T mem: Pointer to the memory struct.
 *      uint32_t element_size: Size of each element in the new segment.
 *      uint32_t segment_size: Number of elements in the new segment.
 *
 * Return:
 *      uint32_t: ID of the newly created segment.
 *
 * Expects:
 *      mem is not NULL.
 *      Memory is successfully allocated for UArray.
 *
 * Notes:
 *      Will CRE if memory struct is NULL.
 *      Will CRE if pointer to allocated memory is NULL.
 *      The segment ID may be reused from the unmapped IDs.
 *      Allocated memory for the segment is freed using seg_free().
 ************************/
static inline uint32_t seg_new(T mem, uint32_t element_size, uint32_t segment_size)
{
        assert(mem != NULL);

        UArray_T segment = UArray_new(segment_size, element_size);
        assert(segment != NULL);
        
        /* get id for new segment */
        uint32_t id = get_id(mem);

        /* if no unused id left, assign it a new one */
        if (id == 0) {
                Seq_addhi(mem->Segments_wrapper, segment);
                id = Seq_length(mem->Segments_wrapper) - 1;
        /* otherwise, reuse a previously used id */
        } else { 
                Seq_put(mem->Segments_wrapper, id, segment);
        }

        return id;
}

/********** seg_at ********
 *
 * Retrieves a word at a specific offset in a specific segment.
 *
 * Parameters:
 *      T mem: Pointer to the memory struct.
 *      uint32_t id: ID of the segment.
 *      uint32_t offset: Offset of the word in the segment.
 *
 * Return:
 *      uint32_t: The retrieved word, which is an instruction.
 *
 * Expects:
 *      mem is not NULL.
 *      Sequence of segments and sequence of IDs stored in mem are not NULL.
 *
 * Notes:
 *      Will CRE if memory struct or the sequences in it are NULL.
 ************************/
static inline uint32_t seg_at(T mem, uint32_t id, uint32_t offset)
{
        assert(mem != NULL);
        Seq_T wrapper_seq = mem->Segments_wrapper;
        assert(wrapper_seq != NULL);
        UArray_T segment = Seq_get(wrapper_seq, id);
        assert(segment != NULL);

        uint32_t instruction = *(uint32_t*)UArray_at(segment, offset);

        return instruction;
}

/********** seg_store ********
 *
 * Store a word at a specific offset in a segment.
 *
 * Parameters:
 *      T mem: Pointer to the memory struct.
 *      uint32_t id: ID of the segment.
 *      uint32_t offset: Offset in the segment.
 *      uint32_t word: Word to be stored.
 *
 * Return:
 *      void
 *
 * Expects:
 *      mem is not NULL.
 *      Sequence of segments and sequence of IDs stored in mem are not NULL.
 *
 * Notes:
 *      Will CRE if memory struct or the sequences in it are NULL.
 ************************/
static inline void seg_store(T mem, uint32_t id, uint32_t offset, uint32_t word)
{
        assert(mem != NULL);
        Seq_T wrapper = mem->Segments_wrapper;
        assert(wrapper != NULL);
        UArray_T segment = Seq_get(wrapper, id);
        assert(segment != NULL);

        *(uint32_t*)UArray_at(segment, offset) = word;
}

/********** seg_size ********
 *
 * Retrieve the size (number of elements) of a specific segment.
 *
 * Parameters:
 *      T mem: Pointer to the memory struct.
 *      uint32_t id: ID of the segment.
 *
 * Return:
 *      uint32_t: Number of elements in the segment.
 *
 * Expects:
 *      mem is not NULL.
 *      Sequence of segments and sequence of IDs stored in mem are not NULL.
 *
 * Notes:
 *      Will CRE if memory struct is NULL.
 ************************/
static inline uint32_t seg_size(T mem, uint32_t id)
{
        assert(mem != NULL);

        UArray_T segment = Seq_get(mem->Segments_wrapper, id);
        assert(segment != NULL);

        uint32_t size = UArray_length(segment);
        return size;
}

/********** seg_copy ********
 *
 * Copy a specific segment to segment 0.
 *
 * Parameters:
 *      T mem: Pointer to the memory struct.
 *      uint32_t id: ID of the segment to copy.
 *
 * Return:
 *      void
 *
 * Expects:
 *      mem is not NULL.
 *      Sequence of segments and sequence of IDs stored in mem are not NULL.
 *
 * Notes:
 *      Will CRE if memory struct or the sequences in it are NULL.
 *      Does nothing if id == 0.
 *      Overwrites segment 0 if it already exists.
 *      Frees memory in original segment 0 before putting in new segment.
 ************************/
static inline void seg_copy(T mem, uint32_t id)
{
        assert(mem != NULL);

        /* if ID is 0, directly return */
        if (id == 0) {
                return;
        }

        UArray_T segment = Seq_get(mem->Segments_wrapper, id);
        assert(segment != NULL);

        int length = UArray_length(segment);
        UArray_T copied_segment = UArray_copy(segment, length);

        /* free original segment 0 */
        seg_free(mem, 0);

        /* put in new segment 0 */
        Seq_put(mem->Segments_wrapper, 0, copied_segment);
}


static inline void seg_free(T mem, uint32_t id)
{
        /* retrive specified segment by id */
        assert(mem != NULL);
        Seq_T wrapper_seq = mem->Segments_wrapper;
        Seq_T unmapped_seq = mem->Unmapped_ID;
        assert(wrapper_seq != NULL);
        assert(unmapped_seq != NULL);

        UArray_T segment = Seq_put(wrapper_seq, id, NULL);
        
        /* free segment array and recycle id */
        if(segment != NULL) {
                UArray_free(&segment);
                Seq_addhi(unmapped_seq, (void *)(uintptr_t)id);
        }
}


static inline void free_memory(T mem)
{
        assert(mem != NULL);

        /* freeing sequence of segments */
        if (mem->Segments_wrapper != NULL) {
                int size = Seq_length(mem->Segments_wrapper);
                if (size != 0) {
                        for (int i = 0; i < size; i++) {
                                if (Seq_get(mem->Segments_wrapper, i) != NULL)
                                {
                                        UArray_T temp_arr = Seq_get(
                                                mem->Segments_wrapper, i);
                                        if (temp_arr != NULL) {
                                                UArray_free(&temp_arr);
                                        }
                                        
                                }
                        }
                }
                Seq_free(&mem->Segments_wrapper);
        }

        /* freeing sequence of unmapped id */
        if (mem->Unmapped_ID != NULL) {
                Seq_free(&mem->Unmapped_ID);
        }

        /* freeing memory struct */
        free(mem);
}

/********** get_id ********
 *
 * Retrieves an unused ID from the list of unmapped IDs, or return 0 if none 
 * exist.
 *
 * Parameters:
 *      T mem: Pointer to the memory struct.
 *
 * Return:
 *      uint32_t: ID of an unmapped segment, or 0 if no unused IDs exist.
 *
 * Expects:
 *      mem is not NULL.
 *      Sequence of IDs stored in mem are not NULL.
 ************************/
static inline uint32_t get_id(T mem)
{
        assert(mem != NULL);
        assert(mem->Unmapped_ID != NULL);

        /* return 0 if sequence of unused id is empty */
        if (Seq_length(mem->Unmapped_ID) == 0) {
                return 0;
        }

        /* otherwise, obtain an id from sequence then removes it from seq */
        uint32_t used_id = (uint32_t)(uintptr_t)Seq_remhi(mem->Unmapped_ID);
        return used_id;
}

static inline uint64_t shl(uint64_t word, unsigned bits)
{
        assert(bits <= 64);
        if (bits == 64)
                return 0;
        else
                return word << bits;
}

/*
 * shift R logical
 */
static inline uint64_t shr(uint64_t word, unsigned bits)
{
        assert(bits <= 64);
        if (bits == 64)
                return 0;
        else
                return word >> bits;
}

static inline uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        unsigned hi = lsb + width; /* one beyond the most significant bit */
        assert(hi <= 64);
        /* different type of right shift */
        return shr(shl(word, 64 - hi),
                   64 - width); 
}

static inline uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
        assert(width <= 64);
        unsigned hi = lsb + width; /* one beyond the most significant bit */
        assert(hi <= 64);
        assert (Bitpack_fitsu(value, width));
        return shl(shr(word, hi), hi)                 /* high part */
                | shr(shl(word, 64 - lsb), 64 - lsb)  /* low part  */
                | (value << lsb);                     /* new part  */
}

static inline bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= 64);
        /* thanks to Jai Karve and John Bryan  */
        /* clever shortcut instead of 2 shifts */
        return shr(n, width) == 0; 
}