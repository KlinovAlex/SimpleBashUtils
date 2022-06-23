#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <regex.h>
int main(int argc, char *argv[]);
void change_arg(int argc, char *argv[], char *e_arg);
char *check_args(char *optarg, char *e_arg);
char *check_args_from_file(char *optarg, char *e_arg, int *f_fl);
char *insert_e_arg(char *new_arg, char *e_arg);
void grep_file(char *path, char *e_arg, int cnt_files,
            int i_fl, int v_fl, int c_fl, int l_fl, int n_fl, int h_fl, int s_fl, int o_fl, int f_fl);
int change_reg_number(int i_fl);
void print_result(char *current_string, char *print_file,
        int cnt_files, int string_count, int size, int n_fl, int h_fl);
char *substring(char *str, int start, int end);