/*
 *      conversions.h
 *
 *      Assignment: CS 40 HW6 - um
 *      Author:     Xiaoyan Xie (xxie05) and Rene Zhao (nzhao05)
 *      Date:       11/19/24
 *      
 *      Summary: This file has the interface of memory.c. It contains functions
 *      for initializing and freeing memory. It allows users to operate on 
 *      segments such as adding and freeing a specific segment, as well as 
 *      operate on a specific word in a specific segment such as accessing and 
 *      storing a word. It also contains a getter of the size of a segment.
 */ 

#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "seq.h"
#include "uarray.h"

typedef struct T *T;


/* initialization and freeing of main memory */
extern T init_memory(uint32_t element_size, uint32_t segment_size, 
                     uint32_t file_size);
extern void free_memory(T mem);

/* operations on memory segments */
extern uint32_t seg_new(T mem, uint32_t element_size, uint32_t segment_size);
extern void seg_free(T mem, uint32_t id);
extern uint32_t seg_at(T mem, uint32_t id, uint32_t offset);
extern void seg_copy(T mem, uint32_t id);
extern void seg_store(T mem, uint32_t id, uint32_t offset, uint32_t word);
extern uint32_t seg_size(T mem, uint32_t id);

#undef MEMORY_H
#endif
