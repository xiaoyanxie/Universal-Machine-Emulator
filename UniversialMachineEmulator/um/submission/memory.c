/*
 *      memory.c
 *
 *      Assignment: CS 40 HW6 - um
 *      Author:     Xiaoyan Xie (xxie05) and Rene Zhao (nzhao05)
 *      Date:       11/19/24
 *      
 *      Summary: This file has the implementation of um's memory module, which
 *      serves as the basis for the instructions module and driver module. The
 *      functions in this file are called by instructions.c to perform certain
 *      instructions and driver.c to initialize and free main memory.
 */ 


#include "memory.h"

/********** Function Declaration ********/
uint32_t get_id(T mem);

/********** T ********
 * This struct contains a sequence of segments and a sequence of ids that does
 * not correspond to a mapped sequence.
 ************************/
struct T {
        Seq_T Segments_wrapper;
        Seq_T Unmapped_ID;
};


/********** init_memory ********
 *
 * Initializes a memory struct that contains unused ids and mapped segments.
 *
 * Parameters:
 *      uint32_t element_size: Size of each element in the segments.
 *      uint32_t segment_size: Number of words in each segment.
 *      uint32_t num_segments: Number of segments to initialize.
 *
 * Return: 
 *      T : Pointer to the initialized memory struct.
 *
 * Expects:
 *      Memory is successfully allocated for struct, sequences, and UArray.
 * 
 * Notes:
 *      Will CRE if pointer to allocated memory is NULL.
 *      Memory allocated in this function are freed in free_memory().
 ************************/
T init_memory(uint32_t element_size, uint32_t segment_size, 
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
uint32_t seg_new(T mem, uint32_t element_size, uint32_t segment_size)
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
uint32_t seg_at(T mem, uint32_t id, uint32_t offset)
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
void seg_store(T mem, uint32_t id, uint32_t offset, uint32_t word)
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
uint32_t seg_size(T mem, uint32_t id)
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
void seg_copy(T mem, uint32_t id)
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

/********** seg_free ********
 *
 * Frees a segment and add its ID to the list of unmapped IDs.
 *
 * Parameters:
 *      T mem: Pointer to the memory struct.
 *      uint32_t id: ID of the segment to free.
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
void seg_free(T mem, uint32_t id)
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

/********** free_memory ********
 *
 * Frees all segments, unmapped IDs, and the memory struct itself.
 *
 * Parameters:
 *      T mem: Pointer to the memory struct.
 *
 * Return:
 *      void
 *
 * Expects:
 *      mem is not NULL.
 *
 * Notes:
 *      Will CRE if memory struct is NULL.
 *      Checks if allocated memory is NULL or data stucture is empty before 
 *      freeing.
 ************************/
void free_memory(T mem)
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
uint32_t get_id(T mem)
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
