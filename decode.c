#include "code.h"
#include "io.h"
#include "trie.h"
#include "word.h"
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

// Function to check bit length of an int with bit shifting
int bitlength(int i);

// extern variables for compression statistics
uint32_t comp = 0;
uint32_t uncomp = 0;

int main(int argc, char **argv) {
  int infile = STDIN_FILENO; // stdin
  int outfile = STDOUT_FILENO; // stdout
  bool vCheck = false; // Check for statistics
  int options = 0;
  while ((options = getopt(argc, argv, "i:o:v")) != -1) {
    switch (options) {
    case 'i':
      infile = open(optarg, O_RDONLY); // Input file was given
      break;
    case 'o':
      outfile = open(
          optarg, O_WRONLY | O_CREAT | O_TRUNC, 0644); // Output file was given
      break;
    case 'v':
      vCheck = true; // Statistics were requested
      break;
    default:
      printf("Please supply valid arguments.\n");
    }
  }

  // Read header
  FileHeader h = { 0, 0 };
  read_header(infile, &h);
  if (h.magic != MAGIC) { // Exit if decode number is incorrect
    return 0;
  }
  fchmod(outfile, h.protection);

  // Decompression code
  WordTable *table = wt_create();
  uint8_t curr_sym = 0;
  uint16_t curr_code = 0;
  uint16_t next_code = START_CODE;
  while (
      read_pair(infile, &curr_code, &curr_sym, bitlength(next_code)) == true) {
    table[next_code] = word_append_sym(table[curr_code], curr_sym);
    buffer_word(outfile, table[next_code]);
    next_code += 1;
    if (next_code == MAX_CODE) {
      wt_reset(table);
      next_code = START_CODE;
    }
  }
  flush_words(outfile);

  if (vCheck == true) { // Print statistics
    printf("Compressed file size: %u bytes\n", uncomp);
    printf("Uncompressed file size: %u bytes\n", comp);
    printf("Compression ratio: %.2f%%\n",
        (100 * (1 - ((float)uncomp / (float)comp))));
  }

  close(infile);
  close(outfile);
  return 0;
}

int bitlength(int i) {
  int inc = 0;
  if (i < 1) {
    return 0;
  }
  while (i != 0) {
    i >>= 1;
    inc++;
  }
  return inc;
}
