/* Related header */
#include "helpers.h"

/* Headers */
#include <stdio.h>    /// `size_t`.
#include <stdlib.h>   /// `free()`.
#include <windows.h>  /// Console text coloring.
#include "../debug.h" /// Error message printing.


/* Predef */

void   free_ptr_arr(void **const target, const size_t len);
char** read_file_by_line(const char *const path, const size_t target_lines);
void   textcolor(const unsigned short int color_code);


/* Body */


void free_ptr_arr(void **const target, const size_t len)
{
    if (target == NULL)
        return;
    
    for (size_t i = 0; i < len; i++)
    {
        free(target[i]);
        target[i] = NULL;
    }

    free(target);
    return;
}


char** read_file_by_line(const char *const path, const size_t target_lines)
{
    FILE* data_file = fopen(path, "r");
    if (data_file == NULL)
    {
        print_error("`read_file_by_line()`: couldnt't open data file");
        return NULL;
    }

    char** result = malloc(target_lines * sizeof(char*));
    if (result == NULL)
    {
        print_error("`read_file_by_line()`: couldnt't allocate memory for result object");
        fclose(data_file);
        data_file = NULL;
        return NULL;
    }
    for (size_t i = 0; i < target_lines; i++)
    {
        result[i] = malloc(64 * sizeof(char));
        if (result[i] == NULL)
        {
            print_error("`read_file_by_line()`: couldnt't allocate memory for result lines");
            for (size_t j = 0; j < i; j++)
            {
                free(result[j]);
                result[j] = NULL;
            }
            free(result);
            fclose(data_file);
            data_file = NULL;
            return (result = NULL);
        }
    }

    for (size_t i = 0; i < target_lines; i++)
    {
        if (fgets(result[i], 64, data_file) == NULL)
        {
            print_error("`read_file_by_line()`: premature EOF");
            for (size_t j = 0; j < target_lines; j++)
            {
                free(result[j]);
                result[j] = NULL;
            }
            free(result);
            fclose(data_file);
            data_file = NULL;
            return (result = NULL);
        }
        else
            result[i][strcspn(result[i], "\n")] = 0; /// `\n` sanitization
    }

    fclose(data_file);
    return result;
}


void textcolor(const unsigned short int color_code)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_code);
    return;
}