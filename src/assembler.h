#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#ifdef __linux__
    #include <linux/limits.h>
    #define MAX_PATH PATH_MAX
#else
    // Linux and Windows are the only supported Operating Systems
    #ifndef WIN32
        #error "Unsuported Operating System"
    #endif
#endif


#define HLT     0x0
#define LDAI    0x1
#define LDAM    0x2
#define LDAB    0x3
#define LDARES  0x4
#define LDAFLAG 0x5
#define LDBA    0x6
#define ADD     0x7
#define LDPCA   0x8
#define LDPCNZA  0x10
#define LDMA    0x11
#define LDVMA   0x12


#define token uint16_t


char* read_file(char* path, size_t* file_len);

int write_file(char* path, size_t file_len, uint8_t* write_contents);

uint8_t* assemble(char* input_file, size_t* output_file_size);

int get_input_output_file_names(char* src_file, char* out_file, int argc, char** argv);

token* tokenize_input(char* input, size_t* token_amount);

bool is_number(const char* str);

#endif