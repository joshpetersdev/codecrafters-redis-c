#ifndef RESP_H
#define RESP_H

struct Command{
    char *name;
    char **args;
    int arg_count;
};

struct Command parse_resp(char cmd[]);
char* encode_bulk_string();
char* encode_simple_string();
char* encode_error();

#endif