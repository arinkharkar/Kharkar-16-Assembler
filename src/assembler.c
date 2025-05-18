#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include "assembler.h"



int main(int argc, char** argv) {
    if (argc < 2) {
        printf("USAGE: k16-assembler -i [source-file.k16] -o [output-file]\n");
        return 1;
    }
    char source_file_name[MAX_PATH];
    char output_file_name[MAX_PATH];

    if (get_input_output_file_names(source_file_name, output_file_name, argc, argv)) return 1;

    // The buffers for each file
    size_t input_file_len;
    char* input_file = read_file(source_file_name, &input_file_len);
    if (!input_file) {
        fprintf(stderr, "Error Opening %s\n", source_file_name);
        return 1;
    }
    // to ensure the file is a valid C-string, null terminate the file buffer
    input_file[input_file_len] = '\0';

    size_t output_file_size = 0;
    uint8_t* machine_code = assemble(input_file, &output_file_size);
    free(input_file);
    if (!machine_code) {
        fprintf(stderr, "Error Assembling\n");
        return 1;
    }
    
    if (write_file(output_file_name, output_file_size, machine_code)) {
        fprintf(stderr, "Error writing to %s\n", output_file_name);
        return 1;
    }
    free(machine_code);
    return 0;
}

int get_input_output_file_names(char* src_file, char* out_file, int argc, char** argv) {
    bool src_file_set = false;
    bool out_file_set = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-o")) {
            if (i+1 >= argc) {
                fprintf(stderr, "ERROR:\nNo File Given after -o\n");
                return 1;
            }
            strcpy(out_file, argv[i+1]);
            out_file_set = true;
            i++;
        } else if (!strcmp(argv[i], "-i")) {
            if (i+1 >= argc) {
                fprintf(stderr, "ERROR:\nNo File Given after -i\n");
                return 1;
            }
            strcpy(src_file, argv[i+1]);
            src_file_set = true;
            i++;
        } else {
            fprintf(stderr, "ERROR:\nUnrecognized Keyword: %s\n", argv[i]);
            return 1;
        }
    }

    if (src_file_set == false || out_file_set == false) {
        fprintf(stderr, "USAGE: k16-assembler -i [source-file.k16] -o [output-file]\n");
        return 1;
    }

    return 0;
}

char* read_file(char* path, size_t* file_len) {
    FILE* file = fopen(path, "r");
    if (!file) return NULL;
    fseek (file, 0, SEEK_END);
    size_t length = ftell (file);
    *file_len = length;
    fseek (file, 0, SEEK_SET);
    uint8_t* buffer = malloc (length);
    if (!buffer) return NULL;
    fread (buffer, 1, length, file);
    fclose (file);
    return (char*)buffer;
}

int write_file(char* path, size_t file_len, uint8_t* write_contents) {
    FILE* file = fopen(path, "wb");
    if (!file) return 1;
    
    if (fwrite(write_contents, 1, file_len, file) != file_len) return 1;
    fclose(file);
    return 0;
}

uint8_t* assemble(char* input_buf, size_t* output_file_size) { 
    size_t token_count = 0;
    token* tokens = tokenize_input(input_buf, &token_count);
    // used to free tokens
    void* tokens_orig = tokens;
    uint16_t* output_file_buf = malloc(sizeof(uint16_t)*(token_count+1));
    if (!tokens) { exit(1); }
    *output_file_size = token_count*sizeof(uint16_t);
    // This assembler is very simple, go through each instruction and translate it directly to the OPCODE
    for (int i = 0; i < token_count; i++) {
        output_file_buf[i] = *tokens;            
        tokens++;
    }
    free(tokens_orig);
    return (uint8_t*)output_file_buf;
}

// Essentially a lexer, turns the input file into a vector of tokens
token* tokenize_input(char* input, size_t* token_amount) {
    size_t max_tokens = 1;
    token* token_vec = malloc(sizeof(token)*max_tokens);
    if (!token_vec) {
        fprintf(stderr, "Error initializing token_vec, insuficient heap\n");
        return NULL;
    }
    size_t token_amt = 0;
    char* cur = input;
    #define MAX_TOKEN_LENGTH 16
    char current_tok[MAX_TOKEN_LENGTH] = {0};
    int i = 0;
    while(true) {
        if (*cur == ' ' || *cur == '\n' || *cur == '\0') {
            // if we have reached 50 tokens, realloc tokens to twice the size it was previously
            if (token_amt + 1 == max_tokens) {
                max_tokens *= 2;
                token_vec = realloc(token_vec, sizeof(token)*max_tokens);
                if (!token_vec) {
                    fprintf(stderr, "Error reinitializing token_vec, insuficient heap\n");
                    return NULL;
                }
            }

            if (!strcmp(current_tok, "HLT")) {
                token_vec[token_amt] = HLT;
                i = 0;
                token_amt++;
                // overwrite the token to 0
                memset(current_tok, 0, MAX_TOKEN_LENGTH);
            } else if (!strcmp(current_tok, "LDAI")) {
                token_vec[token_amt] = LDAI;
                i = 0;
                token_amt++;
                // overwrite the token to 0
                memset(current_tok, 0, MAX_TOKEN_LENGTH);
            } else if (!strcmp(current_tok, "LDAM")) {
                token_vec[token_amt] = LDAM;
                i = 0;
                token_amt++;
                // overwrite the token to 0
                memset(current_tok, 0, MAX_TOKEN_LENGTH);
            } else if (!strcmp(current_tok, "LDARES")) {
                token_vec[token_amt] = LDARES;
                i = 0;
                token_amt++;
                // overwrite the token to 0
                memset(current_tok, 0, MAX_TOKEN_LENGTH);
            } else if (!strcmp(current_tok, "LDAFLAG")) {
                token_vec[token_amt] = LDAFLAG;
                i = 0;
                token_amt++;
                // overwrite the token to 0
                memset(current_tok, 0, MAX_TOKEN_LENGTH);
            } else if (!strcmp(current_tok, "LDBA")) {
                token_vec[token_amt] = LDBA;
                i = 0;
                token_amt++;
                // overwrite the token to 0
                memset(current_tok, 0, MAX_TOKEN_LENGTH);
            } else if (!strcmp(current_tok, "ADD")) {
                token_vec[token_amt] = ADD;
                i = 0;
                token_amt++;
                // overwrite the token to 0
                memset(current_tok, 0, MAX_TOKEN_LENGTH);
            } else if (!strcmp(current_tok, "LDPCA")) {
                token_vec[token_amt] = LDPCA;
                i = 0;
                token_amt++;
                // overwrite the token to 0
                memset(current_tok, 0, MAX_TOKEN_LENGTH);
            } else if (!strcmp(current_tok, "LDPCNZA")) {
                token_vec[token_amt] = LDPCNZA;
                i = 0;
                token_amt++;
                // overwrite the token to 0
                memset(current_tok, 0, MAX_TOKEN_LENGTH);
            } else if (!strcmp(current_tok, "LDMA")) {
                token_vec[token_amt] = LDMA;
                i = 0;
                token_amt++;
                // overwrite the token to 0
                memset(current_tok, 0, MAX_TOKEN_LENGTH);
            } else if (!strcmp(current_tok, "LDVMA")) {
                token_vec[token_amt] = LDVMA;
                i = 0;
                token_amt++;
                // overwrite the token to 0
                memset(current_tok, 0, MAX_TOKEN_LENGTH);
            // If the current token isnt empty (which could happen if the last line had an extra \n char), then we have an undefined token
            } else if(strlen(current_tok) != 0) {
                // first, since PUSH Imm16 takes an immediate value in, check if this unknown token is an immediate and that the previous token is a PUSHI
                if (is_number(current_tok) && token_amt > 0 && (token_vec[token_amt - 1] == LDAI || token_vec[token_amt - 1] == LDAM || token_vec[token_amt - 1] == LDMA || token_vec[token_amt - 1] == LDVMA)) {
                    int value = atoi(current_tok);
                    token_vec[token_amt] = value;

                    i = 0;
                    token_amt++;
                    // overwrite the token to 0
                    memset(current_tok, 0, MAX_TOKEN_LENGTH);
                } else {
                    fprintf(stderr, "ERROR: Unrecognized Token: %s\n", current_tok);
                    exit(1);
                }
            }

            // if we reached the end of the file
            if (*cur == '\0') break;
        } else {
            current_tok[i] = *cur;
            i++;
        }     
        cur++;  
    }
    *token_amount = token_amt;
    return token_vec;
}


bool is_number(const char* str) {
    char* s = (char*)str;
    while (*(++s) != '\0') {
        if (*s < '0' || *s > '9')
            return false;
    }
    return true;
}