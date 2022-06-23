#include "s21_grep.h"
int main(int argc, char *argv[]) {
    char e_arg[500] = "\0";
    change_arg(argc, argv, e_arg);
    return 0;
}

void change_arg(int argc, char *argv[], char *e_arg) {
    int e_fl = 0, i_fl = 0, v_fl = 0, c_fl = 0,
        l_fl = 0, n_fl = 0, h_fl = 0, s_fl = 0, o_fl = 0, f_fl = 0;
    const char* short_options = "e:ivclnhsof:";
    int rez = 0;

    while ((rez = getopt(argc, argv, short_options)) != -1) {
        switch (rez) {
            case 'e':
                e_fl = 1;
                e_arg = check_args(optarg, e_arg);
                break;
            case 'n':
                n_fl++;
                break;
            case 'i':
                i_fl++;
                break;
            case 'v':
                v_fl++;
                break;
            case 'c':
                c_fl++;
                break;
            case 'l':
                l_fl++;
                break;
            case 'h':
                h_fl++;
                break;
            case 's':
                s_fl++;
                break;
            case 'o':
                o_fl++;
                break;
            case 'f':
                f_fl = 1;
                e_arg = check_args_from_file(optarg, e_arg, &f_fl);
                break;
        }
    }
    if (e_fl == 0 && f_fl == 0) {
        strcpy(e_arg, argv[optind]);
        optind++;
    }

    if (v_fl != 0 && o_fl != 0) {
        o_fl = 0;
    }
    if (optind == 1) {
        strcpy(e_arg, argv[optind]);
        optind++;
    }
    if (f_fl != 0 && o_fl != 0) { o_fl = 0; }
    int cnt_files = argc - optind;
    for (int i = optind; i < argc; i++) {
        grep_file(argv[i], e_arg, cnt_files,
            i_fl, v_fl, c_fl, l_fl, n_fl, h_fl, s_fl, o_fl, f_fl);
    }
}

char *check_args(char *optarg, char *e_arg) {
    if (optarg && strlen(e_arg) == 0) {
        strcpy(e_arg, optarg);
    } else if (optarg) {
        e_arg = insert_e_arg(optarg, e_arg);
    }
    return e_arg;
}

char *check_args_from_file(char *optarg, char *e_arg, int *f_fl) {
    FILE *file = fopen(optarg, "r");
    char *current_line_from_file = NULL;

    if (!file) {
        printf("grep: %s: %s\n", optarg, strerror(errno));
    } else {
        ssize_t size_f = 0;
        size_t byte_size_f = 0;
        size_f = getline(&current_line_from_file, &byte_size_f, file);
        while (size_f >= 0) {
            if ((int)current_line_from_file[0] == 10) {
                *f_fl = 2;
            } else if ((int)current_line_from_file[strlen(current_line_from_file)-1] == 10) {
                current_line_from_file[strlen(current_line_from_file)-1] = '\0';
            }

            e_arg = check_args(current_line_from_file, e_arg);
            size_f = getline(&current_line_from_file, &byte_size_f, file);
        }
    }
    free(current_line_from_file);
    fclose(file);

    return e_arg;
}

char *insert_e_arg(char *new_arg, char *e_arg) {
    strncat(e_arg, "|", 2);
    strncat(e_arg, new_arg, strlen(new_arg));
    return e_arg;
}

void grep_file(char *path, char *e_arg, int cnt_files,
            int i_fl, int v_fl, int c_fl, int l_fl, int n_fl, int h_fl, int s_fl, int o_fl, int f_fl) {
    FILE *file = fopen(path, "r");
    char *current_string = NULL;
    char *print_file = "";
    char *char_colon = "";
    int string_count = 1;
    int reg_number = change_reg_number(i_fl);
    int cnt_match = 0, cnt_doesnt_match = 0;

        if (cnt_files > 1) {
        char_colon = ":";
        print_file = strdup(path);
    }
    // s
    if (!file && s_fl == 0) {
        printf("grep: %s: %s\n", path, strerror(errno));
    } else if (file) {
        ssize_t size = 0;
        size_t byte_size = 0;
        size = getline(&current_string, &byte_size, file);
        while (size >= 0) {
            regex_t reegex;
            regmatch_t pm[4026];
            if (regcomp(&reegex, e_arg, reg_number) == 0) {
                int result = regexec(&reegex, current_string, 4026, pm, 0);
                if (f_fl == 2 || result == 0) {
                    cnt_match++;
                    // o
                    if (o_fl != 0) {
                        int x = 0;
                        char *part_match = substring(current_string, pm[x].rm_so, pm[x].rm_eo);
                        if (c_fl == 0 && l_fl == 0) {
                            print_result(part_match, print_file, cnt_files, string_count, size, n_fl, h_fl);
                        }
                        char *match_substr = substring(current_string, pm[0].rm_eo, strlen(current_string));
                        strcpy(current_string, match_substr);
                        free(part_match);
                        free(match_substr);
                    }
                    // e n
                    if (o_fl == 0 && v_fl == 0 && c_fl == 0 && l_fl == 0) {
                        print_result(current_string, print_file, cnt_files, string_count, size, n_fl, h_fl);
                    }
                // v
                } else {
                    cnt_doesnt_match++;
                    if (o_fl != 0) { size = 0; }
                    if (o_fl == 0 && v_fl != 0 && c_fl == 0 && l_fl == 0) {
                        print_result(current_string, print_file, cnt_files, string_count, size, n_fl, h_fl);
                    }
                }
            }
            // o
            if (o_fl == 0 || size == 0) {
                size = getline(&current_string, &byte_size, file);
                string_count++;
            }
            regfree(&reegex);
        }
    }
    free(current_string);

    // c
    if (c_fl != 0) {
        if (l_fl != 0) {
            if (cnt_doesnt_match != 0) { cnt_doesnt_match = 1; }
            if (cnt_match != 0) { cnt_match = 1; }
        }
        char *c_res = "";
        if (cnt_files > 1) {
            c_res = path;
        }
        // v
        if (v_fl != 0) {
            printf("%s%s%.0f\n", c_res, char_colon, (double)cnt_doesnt_match);
        } else {
            printf("%s%s%.0f\n", c_res, char_colon, (double)cnt_match);
        }
    }
    // l
    if (l_fl != 0 && ((cnt_match > 0 && v_fl == 0) || (cnt_doesnt_match > 0 && v_fl != 0))) {
        printf("%s\n", path);
    }

    if (cnt_files > 1) { free(print_file); }
    fclose(file);
}

int change_reg_number(int i_fl) {
    int result = REG_EXTENDED;
    if (i_fl != 0) {
        result = REG_ICASE;
    }
    return result;
}

void print_result(char *current_string, char *print_file,
        int cnt_files, int string_count, int size, int n_fl, int h_fl) {
    static int last_current_string = 0;
    char *last_char = "";

    if ((int)current_string[size-1] != 10) { last_char = "\n"; }

    if (n_fl != 0 && last_current_string != string_count) {
        if (cnt_files > 1) {
            printf("%s:%.0f:%s%s", print_file, (double)string_count, current_string, last_char);
        } else {
            printf("%s%.0f:%s%s", print_file, (double)string_count, current_string, last_char);
        }
    } else if (last_current_string == string_count) {
        printf("%s%s", current_string, last_char);
    } else if (h_fl == 0) {
        if (cnt_files > 1) {
            printf("%s:%s%s", print_file, current_string, last_char);
        } else {
            printf("%s%s%s", print_file, current_string, last_char);
        }
    // h
    } else {
        printf("%s%s", current_string, last_char);
    }
    last_current_string = string_count;
}

char *substring(char *str, int start, int end) {
    int len_new = end - start;
    char *new_line = (char *)malloc(sizeof(char) * (len_new + 1));
    if (NULL == new_line) { exit(0); }
    new_line[len_new] = '\0';
    strncpy(new_line, str + start, len_new);
    return new_line;
}
