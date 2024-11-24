
/*
 *      driver.c
 *
 *      Assignment: CS 40 HW6 - um
 *      Author:     Xiaoyan Xie (xxie05) and Rene Zhao (nzhao05)
 *      Date:       11/19/24
 *      
 *      Summary: This file represents the emulator and contains the 
 *      implementation of the main driver for the universal machine. It is
 *      responsible for I/O, initialization, and memory freeing in the um.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include "instructions.h"
#include <sys/stat.h>

/********** Function Declaration ********/
void open_and_read(char *file_name, T memory, uint32_t file_size);

/********** main ********
 *
 * Entry point for the Universal Machine emulator. Calls functions to 
 * initialize memory, read the input file, execute instructions, and
 * free memory before exiting.
 *
 * Parameters:
 *      int argc:     Number of command arguments.
 *      char *argv[]: Array of arguments, where the second argument represents
                      input file name.
 *
 * Return:
 *      int: Exit status of the program.
 *
 * Expects:
 *      argc == 2 (exactly one file name is provided as an argument).
 *      argv[1] is a valid file name of a binary file.
 *
 * Notes:
 *      Exits with EXIT_FAILURE if the command line violates contract or 
 *      input file is invalid or cannot be read.
 ************************/
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


void open_and_read(char *file_name, T memory, uint32_t file_size)
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

