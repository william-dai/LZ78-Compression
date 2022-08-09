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
  bool inputCheck = false; // Check for input file
  bool outputCheck = false; // Check for outfile file
  char *in; // Input file if given
  char *out; // Output file if given
  int options = 0;
  while ((options = getopt(argc, argv, "i:o:v")) != -1) {
    switch (options) {
    case 'i':
      inputCheck = true; // Input file was given
      in = optarg;
      break;
    case 'o':
      outputCheck = true; // Outfile file was given
      out = optarg;
      break;
    case 'v':
      vCheck = true; // Statistics were requested
      break;
    default:
      printf("Please supply valid arguments.\n");
      break;
    }
  }

  if (inputCheck == true) {
    infile = open(in, O_RDONLY); // Set infile to specified input file
  }
  if (outputCheck == true) {
    outfile = open(out, O_WRONLY | O_CREAT | O_TRUNC,
        0644); // Set outfile to specified output file
  }

  // Create header
  struct stat info;
  fstat(infile, &info);
  FileHeader h = { 0, 0 };
  h.magic = MAGIC;
  h.protection = info.st_mode;
  fchmod(outfile, h.protection);
  write_header(outfile, &h);

  // Compression code
  TrieNode *root = trie_create();
  TrieNode *curr_node = root;
  TrieNode *prev_node = NULL;
  uint8_t curr_sym = 0;
  uint8_t prev_sym = 0;
  uint16_t next_code = START_CODE;
  while (read_sym(infile, &curr_sym) == true) {
    if (curr_node) {
      TrieNode *next_node = trie_step(curr_node, curr_sym);
      if (next_node != NULL) {
        prev_node = curr_node;
        curr_node = next_node;
      } else {
        buffer_pair(outfile, curr_node->code, curr_sym, bitlength(next_code));
        curr_node->children[curr_sym] = trie_node_create(next_code);
        curr_node = root;
        next_code += 1;
      }
    }
    if (next_code == MAX_CODE) {
      trie_reset(root);
      curr_node = root;
      next_code = START_CODE;
    }
    prev_sym = curr_sym;
  }
  if (curr_node != root) {
    buffer_pair(outfile, prev_node->code, prev_sym, bitlength(next_code));
    next_code = (next_code + 1) % MAX_CODE;
  }
  buffer_pair(outfile, STOP_CODE, 0, bitlength(next_code));
  flush_pairs(outfile);

  if (vCheck == true) { // Print statistics
    printf("\nCompressed file size: %u bytes\n", comp);
    printf("Uncompressed file size: %u bytes\n", uncomp);
    printf("Compression ratio: %.2f%%\n",
        (100 * (1 - ((float)comp / (float)uncomp))));
  }

  close(infile);
  close(outfile);
  trie_delete(root);
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
