/*
 *      instructions.h
 *
 *      Assignment: CS 40 HW6 - um
 *      Author:     Xiaoyan Xie (xxie05) and Rene Zhao (nzhao05)
 *      Date:       11/19/24
 *      
 *      Summary: This file has the interface of instructions.c. Through this
 *      interface, users are allowed to call the main instructions driver, 
 *      which goes through and executes every instruction provided in file.
 */ 

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "bitpack.h"

typedef struct load_val *load_val;

/* initialization of instructions module */
extern void instructions_driver(T mem);

#undef INSTRUCTIONS_H
#endif