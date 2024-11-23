#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"

extern void Um_write_sequence(FILE *output, Seq_T instructions);
void build_halt_test(Seq_T s);
extern void build_add_halt_test(Seq_T instructions);

extern void addition_test(Seq_T s);
extern void division_test(Seq_T s);
extern void conditional_move_test(Seq_T s);
extern void multip_test(Seq_T s);
extern void nand_test(Seq_T s);

extern void input_test(Seq_T s);
extern void load_test(Seq_T s);
extern void segload_store_test(Seq_T s);
extern void segload_store_large_test(Seq_T s);
extern void map_test(Seq_T s);
void test_50_million_instr_no_output(Seq_T stream);


/* The array `tests` contains all unit tests for the lab. */

static struct test_info {
        const char *name;
        const char *test_input;          /* NULL means no input needed */
        const char *expected_output;
        /* writes instructions into sequence */
        void (*build_test)(Seq_T stream);
} tests[] = {
        { "test_halt", NULL, "", build_halt_test },
        { "test_add-halt", NULL, "", build_add_halt_test },
        { "test_addition", NULL, "A", addition_test },
        { "test_division", NULL, "d", division_test },
        { "test_multiplication", NULL, "d", multip_test },
        { "test_nand", NULL, NULL, nand_test },
        { "test_input", "c", "c", input_test },
        { "test_conditional_move", NULL, "<", conditional_move_test },
        { "test_50_million", NULL, NULL, test_50_million_instr_no_output },
        { "test_segload_store", NULL, "c", segload_store_test },/*pass*/
        { "test_segload_large", NULL, "a", segload_store_large_test },
        { "test_map", NULL, "\n", map_test }
};

  
#define NTESTS (sizeof(tests)/sizeof(tests[0]))

/*
 * open file 'path' for writing, then free the pathname;
 * if anything fails, checked runtime error
 */
static FILE *open_and_free_pathname(char *path);

/*
 * if contents is NULL or empty, remove the given 'path', 
 * otherwise write 'contents' into 'path'.  Either way, free 'path'.
 */
static void write_or_remove_file(char *path, const char *contents);

static void write_test_files(struct test_info *test);


int main (int argc, char *argv[])
{
        bool failed = false;
        if (argc == 1)
                for (unsigned i = 0; i < NTESTS; i++) {
                        printf("***** Writing test '%s'.\n", tests[i].name);
                        write_test_files(&tests[i]);
                }
        else
                for (int j = 1; j < argc; j++) {
                        bool tested = false;
                        for (unsigned i = 0; i < NTESTS; i++)
                                if (!strcmp(tests[i].name, argv[j])) {
                                        tested = true;
                                        write_test_files(&tests[i]);
                                }
                        if (!tested) {
                                failed = true;
                                fprintf(stderr,
                                        "***** No test named %s *****\n",
                                        argv[j]);
                        }
                }
        return failed; /* failed nonzero == exit nonzero == failure */
}


static void write_test_files(struct test_info *test)
{
        FILE *binary = open_and_free_pathname(Fmt_string("%s.um", test->name));
        Seq_T instructions = Seq_new(0);
        test->build_test(instructions);
        Um_write_sequence(binary, instructions);
        Seq_free(&instructions);
        fclose(binary);

        write_or_remove_file(Fmt_string("%s.0", test->name),
                             test->test_input);
        write_or_remove_file(Fmt_string("%s.1", test->name),
                             test->expected_output);
}


static void write_or_remove_file(char *path, const char *contents)
{
        if (contents == NULL || *contents == '\0') {
                remove(path);
        } else {
                FILE *input = fopen(path, "wb");
                assert(input != NULL);

                fputs(contents, input);
                fclose(input);
        }
        free(path);
}


static FILE *open_and_free_pathname(char *path)
{
        FILE *fp = fopen(path, "wb");
        assert(fp != NULL);

        free(path);
        return fp;
}
