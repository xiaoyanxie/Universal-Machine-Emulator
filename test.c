#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
//#include "memory.h"
#include "instructions.h"
#include "seq.h"
#include "uarray.h"

// void test_init_mem();
// void test_get_id();
// void test_seg_new();
// void test_seg_free();
// void test_seg_at_store();
// void test_seg_size();
// void test_seg_copy();
// void test_init_instructions();

typedef struct A A;

struct A
{
        uint32_t ele_size;
        uint32_t mem_num_ele;
        uint32_t unmap_num_ele;

        uint32_t mem_length;
        uint32_t unmap_length;
        uint32_t seg_length;

        uint32_t **mem_array;
        uint32_t *unmap_array;
        uint32_t *seg_0;
};

void print_struct(A mem);
void test_get_id(A mem);

int main()
{
        printf("-----test_init_mem-----\n");
        uint32_t file_size = 160;
        struct A mem;

        mem.ele_size = sizeof(uint32_t);
        mem.unmap_length = 400;
        mem.mem_length = 2000;
        mem.seg_length = file_size;
        // mem.unmap_num_ele = 2;
        
        uint32_t *unmapped_ID = calloc(mem.unmap_length, sizeof(uint32_t));
        assert(unmapped_ID != NULL);

        /* initialize sequence of segment wrapper */
        uint32_t **mem_wrapper = calloc(mem.mem_length, sizeof(uint32_t *));
        assert(mem_wrapper != NULL);
 
        uint32_t *segment = calloc(mem.seg_length, sizeof(uint32_t));
        // printf("segment length is %lu\n", sizeof(segment) / sizeof(segment[0]));
        assert(segment != NULL);
        // for (uint32_t i = 0; i < mem.seg_length; i++) {
        //         segment[i] = 10 * i;
        //         printf(" %d : %u\n", i, segment[i]);
        // }
        mem_wrapper[0] = segment;

        mem.mem_array = mem_wrapper;
        mem.unmap_array = unmapped_ID;
        mem.seg_0 = segment;
        mem.mem_num_ele = 1;
        mem.unmap_num_ele = 2;
        mem.unmap_array[0] = 3;
        mem.unmap_array[1] = 8;


        //print_struct(mem);
        printf("-----test_seg_new-----\n");
        segment = calloc(mem.seg_length, mem.ele_size);
        assert(segment != NULL);

        /* get id */
        uint32_t id;
        if (mem.unmap_num_ele == 0)
        {
                /*check mem array size*/
                if (mem.mem_num_ele == mem.mem_length - 1) {
                        uint32_t **new_mem_array = calloc(2 * mem.mem_length, sizeof(uint32_t *));
                        free(mem.mem_array);
                        mem.mem_array = new_mem_array;
                        mem.mem_length *= 2;
                }
                
                id = mem.mem_num_ele - 1;
        } else {
                id = (mem.unmap_array[mem.unmap_num_ele - 1]);
                mem.unmap_num_ele--;
        }

        mem.mem_array[id] = segment;
        mem.mem_num_ele++;
        printf("id is %u\n", id);
        printf("mem.mem_num_ele is %u\n", mem.mem_num_ele);
        
        printf("-----all tests complete-----\n");
}

void print_struct(A mem)
{
        printf("printing elements in struct A:\n");
        printf("mem_length is %u\n", mem.mem_length);
        printf("ele_size is %u\n", mem.ele_size);
        printf("unmap_length is %u\n", mem.unmap_length);
        printf("seg_length is %u\n", mem.seg_length);

        printf("first element in mem_array is %u\n", mem.mem_array[0][0]);
        printf("first element in unmap_array is %u\n", mem.unmap_array[0]);
        printf("first element in seg_0 is %u\n", mem.seg_0[0]);
}

// void test_init_mem() 
// {
//         uint32_t element_size = 4;
//         uint32_t segment_size = 4;
//         uint32_t num_segments = 2;
        
//         T mem = init_memory(element_size, segment_size, num_segments);
//         if (mem == NULL) {
//                 printf("Memory struct is NULL.\n");
//                 assert(0);
//         }

//         if (mem->Segments_wrapper == NULL) {
//                 printf("Sequence of segments is NULL.\n");
//                 assert(0);     
//         } else if (Seq_length(mem->Segments_wrapper) != (int)num_segments) {
//                 printf("Number of segments in Memory struct is %i, while file size is %i.\n", Seq_length(mem->Segments_wrapper), num_segments);
//                 assert(0);
//         }

//         if (mem->Unmapped_ID == NULL) {
//                 printf("Sequence of unmapped ID is NULL.\n");
//                 assert(0);     
//         } else if (Seq_length(mem->Unmapped_ID) != 0) {
//                 printf("Sequence of unmapped ID is not empty.\n");
//                 assert(0);
//         }

//         free_memory(mem);

// }

// void test_get_id()
// {
//         uint32_t element_size = 4;
//         uint32_t segment_size = 4;
//         uint32_t num_segments = 20000;
//         uint32_t identifier = 1600;
        
//         T mem = init_memory(element_size, segment_size, num_segments);
//         uint32_t id_1 = get_id(mem);
//         if (id_1 != 0) {
//                 printf("Stored ID is %u, but should be 0.\n", id_1);
//                 assert(0);
//         }
        
//         Seq_addhi(mem->Unmapped_ID, (void*)(uintptr_t)identifier);
//         uint32_t id_2 = get_id(mem);
//         if (id_2 != 1600) {
//                 printf("Stored ID is %u, but should be 1600.\n", id_2);
//                 assert(0);
//         }

//         free_memory(mem);
// }

void test_get_id(A mem)
{
        uint32_t used_id;
        if (mem.unmap_num_ele == 0)
        {
                used_id = mem.mem_num_ele - 1;
        } else {
                used_id = (mem.unmap_array[mem.unmap_num_ele - 1]);
                mem.unmap_num_ele--;
        }


        if (used_id != 0) {
                printf("Stored ID is %u, but should be 0.\n", used_id);
                assert(0);
        }
        
        // Seq_addhi(mem->Unmapped_ID, (void*)(uintptr_t)identifier);
        // uint32_t id_2 = get_id(mem);
        // if (id_2 != 1600) {
        //         printf("Stored ID is %u, but should be 1600.\n", id_2);
        //         assert(0);
        // }

        // free_memory(mem);
}

// void test_seg_new()
// {
//         uint32_t element_size = 4;
//         uint32_t segment_size = 10;
//         uint32_t num_segments = 2000;
//         uint32_t identifier = 1999;
        
//         T mem = init_memory(element_size, segment_size, num_segments);
//         uint32_t id_1 = seg_new(mem, element_size, segment_size);
//         if (id_1 != 2) {
//                 printf("New ID is %u, but should be 1.\n", id_1);
//                 assert(0);    
//         }

//        uint32_t id_2 = seg_new(mem, element_size, segment_size);
//         if (id_2 != 3) {
//                 printf("New ID is %u, but should be 2.\n", id_2);
//                 assert(0);    
//         }

//         Seq_addhi(mem->Unmapped_ID, (void*)(uintptr_t)identifier);

//         uint32_t id_3 = seg_new(mem, element_size, segment_size);
//         if (id_3 != 1) {
//                 printf("New ID is %u, but should be 3.\n", id_3);
//                 assert(0);    
//         }
//         free_memory(mem);
// }

// void test_seg_at_store() {
//         uint32_t element_size = 4;
//         uint32_t segment_size = 32;
//         uint32_t num_segments = 2;
//         uint32_t identifier = 1;
        
//         T mem = init_memory(element_size, segment_size, num_segments);
//         seg_store(mem, identifier, 0, 66);
//         uint32_t word = seg_at(mem, identifier, 0);
//         if (word != 66) {
//                 printf("Stored word is %u, but seg_at returns %u.\n", 66, word);
//                 assert(0);   
//         }
//         free_memory(mem);
// }


// void test_seg_free()
// {
//         uint32_t element_size = 4;
//         uint32_t segment_size = 1;
//         uint32_t num_segments = 2;
//         //uint32_t identifier = 1;
        
//         T mem = init_memory(element_size, segment_size, num_segments);
//         uint32_t id_1 = seg_new(mem, element_size, segment_size);
//         (void)id_1;

//         seg_free(mem, 2);

//         UArray_T segment  = Seq_get(mem->Segments_wrapper, 2);

//         if (segment != NULL) {
//                 printf("segment not NULL.\n");
//                 assert(0); 
//         }
//         free_memory(mem);
// }

// void test_seg_size()
// {
//         uint32_t element_size = 4;
//         uint32_t segment_size = 12;
//         uint32_t num_segments = 2;
//         //uint32_t identifier = 1;
        
//         T mem = init_memory(element_size, segment_size, num_segments);
//         uint32_t size = seg_size(mem, 1);

//         if (size != 12) {
//                 printf("Segment size is %u, but should be 12.\n", size);
//                 assert(0); 
//         }
//         free_memory(mem);
// }

// void test_seg_copy()
// {
//         uint32_t element_size = 4;
//         uint32_t segment_size = 12;
//         uint32_t num_segments = 2;
//         //uint32_t identifier = 1;
        
//         T mem = init_memory(element_size, segment_size, num_segments);
//         seg_store(mem, 1, 6, 66);
//         // uint32_t word_seg0 = seg_at(mem, 0, 6);
//         //printf("Word in segment 0 index 6 is %u.\n", word_seg0);
//         // uint32_t word_seg1 = seg_at(mem, 1, 6);
//         //printf("Word in segment 1 index 6 is %u.\n", word_seg1);

//         seg_copy(mem, 1);
//         uint32_t word_new0 = seg_at(mem, 0, 6);
//         uint32_t word_new1 = seg_at(mem, 1, 6);

//         if (word_new0 != 66) {
//                 printf("Word is %u, but should be 66.\n", word_new0);
//                 assert(0);
//         }
//         if (word_new1 != 66) {
//                 printf("Word is %u, but should be 66.\n", word_new1);
//                 assert(0);
//         }
//         free_memory(mem);
// }

// // void test_init_instructions()
// // {
// //         init_instructions();
// // }

