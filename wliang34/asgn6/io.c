#include "code.h"
#include <stdlib.h>
#include <stdio.h>
#include "io.h"
#include "endian.h"
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BLOCK 4096 // 4KB blocks.
#define MAGIC 0xBAADBAAC // Unique encoder/decoder magic number.

//To calculate the file size
uint64_t total_syms = 0; // To count the symbols processed.
uint64_t total_bits = 0; // To count the bits processed.

static uint8_t buf_pair[BLOCK]; // buffer for pair
static int buf_pair_pos = 0; // index in buf_pair
static uint8_t bit_left = 8; // left bit in the byte
static int buf_pair_len = 0; // valid size for read_pair

static uint8_t buf_sym[BLOCK]; // buffer for sym
static int buf_sym_pos = 0; // index for buf_sym
static int buf_sym_size = 0; // valid size

//clear the buffer, set all to 0
static inline void buf_clear(void) {
    for (int i = 0; i < BLOCK; i++) {
        buf_pair[i] = 0;
    }
}

int read_bytes(int infile, uint8_t *buf, int to_read) {
    int total_read = 0; // sum of bytes it read
    int num_read;
    uint8_t *buf_ptr = buf; // make a copy

    while (to_read > 0) {
        num_read = read(infile, buf_ptr, to_read);
        if (num_read == -1) {
            // Error occurred, return -1 to signal failure
            return -1;
        } else if (num_read == 0) { // Optional
            // End of file reached
            break;
        } else {
            // Move the buffer pointer to resume reading
            buf_ptr += num_read;
            total_read += num_read;
            to_read -= num_read;
        }
    }
    return total_read;
}

int write_bytes(int outfile, uint8_t *buf, int to_write) {
    int total_write = 0; //sum of bytes it writes
    int num_write;
    uint8_t *buf_ptr = buf; // make a copy

    while (to_write > 0) {
        num_write = write(outfile, buf_ptr, to_write);
        if (num_write == -1) {
            // Error occurred, return -1 to signal failure
            return -1;
        } else if (num_write == 0) {
            // End of file reached
            break;
        } else {
            // Move the buffer pointer to resume writing
            buf_ptr += num_write;
            total_write += num_write;
            to_write -= num_write;
        }
    }
    return total_write;
}

void read_header(int infile, FileHeader *header) {
    total_bits += 8 * read_bytes(infile, (uint8_t *) header, sizeof(FileHeader));
    if (big_endian()) { // check endianess
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    } //check magic number
    if (header->magic != 0xBAADBAAC) {
        fprintf(stderr, "Error: Unmatched magic number.\n");
        exit(1);
    }
}

void write_header(int outfile, FileHeader *header) {
    if (big_endian()) { // check endianess
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    }
    total_bits += 8 * write_bytes(outfile, (uint8_t *) header, sizeof(FileHeader));
}

void write_pair(int outfile, uint16_t code, uint8_t sym, int bitlen) {
    uint8_t sym_bit = 8; // nbit of symbol=8
    uint8_t bit_to_write; // what will be insert into the array
    while (bitlen > 0) {
        bit_to_write = code & 1; // check last bit
        buf_pair[buf_pair_pos] |= (bit_to_write << (8 - bit_left)); // insert
        code >>= 1;
        bitlen -= 1; //left bit to be proceessed
        bit_left -= 1; // left bit in the byte
        if (bit_left == 0) // end of byte
        {
            buf_pair_pos += 1;
            bit_left = 8;
        }

        if (buf_pair_pos == BLOCK) //end of block
        {
            total_bits += 8 * write_bytes(outfile, buf_pair, BLOCK);
            buf_clear(); // clear
            buf_pair_pos = 0; //index = 0
        }
    }

    while (sym_bit > 0) { // similar rationale as write code
        bit_to_write = sym & 1;
        buf_pair[buf_pair_pos] |= (bit_to_write << (8 - bit_left));
        sym >>= 1;
        sym_bit -= 1;
        bit_left -= 1;
        if (bit_left == 0) {
            buf_pair_pos += 1;
            bit_left = 8;
        }
        if (buf_pair_pos == BLOCK) {
            total_bits += 8 * write_bytes(outfile, buf_pair, BLOCK);
            buf_clear();
            buf_pair_pos = 0;
        }
    }
}

void flush_pairs(int outfile) {
    //printf("left:%d\n", bit_left);
    if (bit_left == 8) {
        total_bits += 8 * write_bytes(outfile, buf_pair, buf_pair_pos);
    } else {
        total_bits += 8 * write_bytes(outfile, buf_pair, buf_pair_pos + 1);
    }
    buf_pair_pos = 0;
    bit_left = 8; //important, this is for read!
}

bool read_pair(int infile, uint16_t *code, uint8_t *sym, int bitlen) {
    // If buf is empty, read a block from file
    // printf("bitlen: %d bit_left: %d, pos: %d sym: %d, code: %d\n", bitlen, bit_left, buf_pair_pos, *sym, *code);
    if (buf_pair_pos >= buf_pair_len) {
        buf_pair_len = read_bytes(infile, buf_pair, BLOCK);
        total_bits += 8 * buf_pair_len;
        buf_pair_pos = 0;
    }
    // Nothing in the buffer to read
    if (buf_pair_len == 0) {
        return false;
    }

    *code = 0;
    uint16_t bit_to_write;
    int bit_processed = 0; // how many bits have been written into buf
    while (bitlen > bit_processed) {
        bit_to_write = ((buf_pair[buf_pair_pos] >> (8 - bit_left)) & 1) << bit_processed;
        *code |= bit_to_write; //insert
        bit_processed += 1;
        bit_left -= 1;
        if (bit_left == 0) { //will go to new byte
            buf_pair_pos += 1;
            bit_left = 8;
        }
        if (buf_pair_pos == buf_pair_len) {
            buf_clear();
            buf_pair_len = read_bytes(infile, buf_pair, BLOCK);
            total_bits += 8 * buf_pair_len;
            buf_pair_pos = 0; // new buffer
        }
    }

    *sym = 0;
    int sym_bit_processed = 0;
    while (sym_bit_processed < 8) {
        bit_to_write = ((buf_pair[buf_pair_pos] >> (8 - bit_left)) & 1) << sym_bit_processed;
        *sym |= bit_to_write;
        sym_bit_processed += 1;
        bit_left -= 1;
        if (bit_left == 0) {
            buf_pair_pos += 1;
            bit_left = 8;
        }
        if (buf_pair_pos == buf_pair_len) {
            buf_clear();
            buf_pair_len = read_bytes(infile, buf_pair, BLOCK);
            buf_pair_pos = 0;
            total_bits += 8 * buf_pair_len;
        }
    }
    if (*code == STOP_CODE) {
        return false;
    }
    return true;
}

bool read_sym(int infile, uint8_t *sym) {
    // If we've reached the end of the buffer, read in another block of data
    if (buf_sym_pos >= buf_sym_size) {
        buf_sym_size = read_bytes(infile, buf_sym, sizeof(buf_sym));
        buf_sym_pos = 0;
    }

    // If we've reached the end of the file, return false
    if (buf_sym_size == 0) {
        return false;
    }

    // Read a single symbol from the buffer and update buffer position
    *sym = buf_sym[buf_sym_pos++];
    total_syms += 1;
    return true;
}

// write word into outfile
void write_word(int outfile, Word *w) {
    for (uint32_t i = 0; i < w->len; ++i) {
        buf_sym[buf_sym_pos] = w->syms[i];
        buf_sym_pos += 1;
        if (buf_sym_pos >= BLOCK) {
            total_syms += write_bytes(outfile, buf_sym, BLOCK);
            //buf_clear(buf_sym);
            buf_sym_pos = 0;
        }
    }
}

void flush_words(int outfile) {
    // write out the rest in the buffer
    total_syms += write_bytes(outfile, buf_sym, buf_sym_pos);
    buf_sym_pos = 0;
}
