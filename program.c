#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef int integer;
typedef uint8_t unsigned_eight_bit_integer;
typedef uint32_t unsigned_thirty_two_bit_integer;

typedef struct { unsigned_thirty_two_bit_integer words[44]; } expanded_key;

typedef struct { unsigned_eight_bit_integer matrix[4][4]; } matrix;

typedef struct { unsigned_eight_bit_integer bytes[16]; } block;

static const unsigned_thirty_two_bit_integer lookup_table[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static const unsigned_thirty_two_bit_integer round_constants[11] = {
    0x00000000U,
    0x01000000U,
    0x02000000U,
    0x04000000U,
    0x08000000U,
    0x10000000U,
    0x20000000U,
    0x40000000U,
    0x80000000U,
    0x1b000000U,
    0x36000000U
};

static matrix substitute_bytes(matrix state)
{
    for (integer row = 0; row < 4; row++)
        for (integer column = 0; column < 4; column++)
            state.matrix[row][column] = lookup_table[state.matrix[row][column]];
    
    return state;
}

static matrix shift_rows(matrix state)
{
    unsigned_eight_bit_integer byte_placeholder;

    byte_placeholder    = state.matrix[1][0];
    state.matrix[1][0]  = state.matrix[1][1];
    state.matrix[1][1]  = state.matrix[1][2];
    state.matrix[1][2]  = state.matrix[1][3];
    state.matrix[1][3]  = byte_placeholder;

    byte_placeholder    = state.matrix[2][0];
    state.matrix[2][0]  = state.matrix[2][2];
    state.matrix[2][2]  = byte_placeholder;
    byte_placeholder    = state.matrix[2][1];
    state.matrix[2][1]  = state.matrix[2][3];
    state.matrix[2][3]  = byte_placeholder;

    byte_placeholder    = state.matrix[3][3];
    state.matrix[3][3]  = state.matrix[3][2];
    state.matrix[3][2]  = state.matrix[3][1];
    state.matrix[3][1]  = state.matrix[3][0];
    state.matrix[3][0]  = byte_placeholder;

    return state;
}

static inline unsigned_eight_bit_integer double_byte(unsigned_eight_bit_integer byte)
{
    return (byte << 1) ^ ((byte & 0x80) ? 0x1B : 0x00);
}

static matrix mix_columns(matrix state)
{
    for (integer column = 0; column < 4; column++)
    {
        unsigned_eight_bit_integer first_byte = state.matrix[0][column];
        unsigned_eight_bit_integer second_byte = state.matrix[1][column];
        unsigned_eight_bit_integer third_byte = state.matrix[2][column];
        unsigned_eight_bit_integer fourth_byte = state.matrix[3][column];
        
        unsigned_eight_bit_integer columnar_parity  = first_byte ^ second_byte ^ third_byte ^ fourth_byte;
        unsigned_eight_bit_integer intermediate_term;

        intermediate_term = first_byte ^ second_byte;
        intermediate_term = double_byte(intermediate_term);
        state.matrix[0][column] ^= columnar_parity ^ intermediate_term;
        
        intermediate_term = second_byte ^ third_byte;
        intermediate_term = double_byte(intermediate_term);
        state.matrix[1][column] ^= columnar_parity ^ intermediate_term;
        
        intermediate_term = third_byte ^ fourth_byte;
        intermediate_term = double_byte(intermediate_term);
        state.matrix[2][column] ^= columnar_parity ^ intermediate_term;
        
        
        intermediate_term = fourth_byte ^ first_byte;
        intermediate_term = double_byte(intermediate_term);
        state.matrix[3][column] ^= columnar_parity ^ intermediate_term;
    }
    
    return state; 
}

static matrix add_round_key(matrix state, expanded_key round_keys, integer round)
{
    for (integer column = 0; column < 4; column++)
    {
        unsigned_thirty_two_bit_integer word = round_keys.words[round * 4 + column];
        
        state.matrix[0][column] ^= (word >> 24) & 0xFF;
        state.matrix[1][column] ^= (word >> 16) & 0xFF;
        state.matrix[2][column] ^= (word >>  8) & 0xFF;
        state.matrix[3][column] ^=  word        & 0xFF;
    }
    
    return state;
}

static inline unsigned_thirty_two_bit_integer rotate_word_leftward(unsigned_thirty_two_bit_integer word)
{
    return (word << 8) | (word >> 24);
}

static unsigned_thirty_two_bit_integer substitute_word(unsigned_thirty_two_bit_integer word) {
    return ((unsigned_thirty_two_bit_integer) lookup_table[(word >> 24) & 0xFF] << 24) |
           ((unsigned_thirty_two_bit_integer) lookup_table[(word >> 16) & 0xFF] << 16) |
           ((unsigned_thirty_two_bit_integer) lookup_table[(word >>  8) & 0xFF] <<  8) |
           ((unsigned_thirty_two_bit_integer) lookup_table[(word      ) & 0xFF]      );
}

static expanded_key expand_key(const unsigned_eight_bit_integer key[16])
{
    expanded_key round_keys;
    
    integer i;
    unsigned_thirty_two_bit_integer last_word;

    for (i = 0; i < 4; i++)
    {
        round_keys.words[i] = ((unsigned_thirty_two_bit_integer) key[4 * i    ] << 24) |
                              ((unsigned_thirty_two_bit_integer) key[4 * i + 1] << 16) |
                              ((unsigned_thirty_two_bit_integer) key[4 * i + 2] <<  8) |
                              ((unsigned_thirty_two_bit_integer) key[4 * i + 3]      );
    }

    for (i = 4; i < 44; i++)
    {
        last_word = round_keys.words[i - 1];
        
        if (i % 4 == 0)
            last_word = substitute_word(rotate_word_leftward(last_word)) ^ round_constants[i / 4];
        
        round_keys.words[i] = round_keys.words[i - 4] ^ last_word;
    }
    
    return round_keys;
}

static void print_round_keys(const unsigned_thirty_two_bit_integer word[44])
{
    for (integer round = 0; round < 11; round++)
    {
        printf("Round %2d Key:", round);
        
        for (integer column = 0; column < 4; column++)
            printf(" %08x", word[round * 4 + column]);
        
        printf("\n");
    }
}

static void print_state(const matrix state, integer round)
{
    printf("Round %2d State:\n", round);
    
    for (integer row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            printf("%02x ", state.matrix[row][col]);
        printf("\n");
    }
    
    printf("\n");
}

static block encrypt_block(block clear_text, expanded_key round_keys)
{
    matrix state;
    integer row, column;

    for (row = 0; row < 4; row++)
        for (column = 0; column < 4; column++)
            state.matrix[row][column] = clear_text.bytes[column * 4 + row];

    state = add_round_key(state, round_keys, 0);

    for (integer round = 1; round < 10; round++)
    {
        state = substitute_bytes(state);
        state = shift_rows(state);
        state = mix_columns(state);
        state = add_round_key(state, round_keys, round);
        print_state(state, round);
    }

    state = substitute_bytes(state);
    state = shift_rows(state);
    state = add_round_key(state, round_keys, 10);
    print_state(state, 10);

    block encrypted_block;
    
    for (row = 0; row < 4; row++)
        for (column = 0; column < 4; column++)
            encrypted_block.bytes[column * 4 + row] = state.matrix[row][column];

    return encrypted_block;
}

static void read_key_from_user(unsigned_eight_bit_integer key[16])
{
    char buffer[128];
    
    printf("Enter a 16 (sixteen) or fewer ASCII character key string: ");
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        fprintf(stderr, "Error reading key string input.\n");
        return;
    }
    
    buffer[strcspn(buffer, "\n")] = '\0';
    integer length = strlen(buffer);
    
    for (integer i = 0; i < 16; i++) {
        if (i < (integer)length)
            key[i] = (unsigned_eight_bit_integer)buffer[i];
        else
            key[i] = '\0';
    }
}

block read_clear_text_block(void)
{
    char input[256];
    block clear_text;

    printf("Enter clear text to be encrypted: ");
    fgets(input, sizeof(input), stdin);

    integer length = strcspn(input, "\n");
    input[length] = '\0';

    for (integer i = 0; i < 16; i++)
    {
        if (i < length)
            clear_text.bytes[i] = (unsigned_eight_bit_integer)input[i];
        else
            clear_text.bytes[i] = 0x00;
    }

    return clear_text;
}

integer main(void)
{
    unsigned_eight_bit_integer key[16];
    
    read_key_from_user(key);

    expanded_key round_keys = expand_key(key);
    
    print_round_keys(round_keys.words);
    printf("\n");
    
    block clear_text = read_clear_text_block();
    printf("\n");

    block cipher_text = encrypt_block(clear_text, round_keys);
    
    printf("Cipher text (hexadecimal representation): ");

    for (integer i = 0; i < 16; i++)
        printf("%02x", cipher_text.bytes[i]);
        
    printf("\n");
    
    return 0;
}
