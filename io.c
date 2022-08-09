#include "io.h"
#include "code.h"

// Symbol buffer and index used to track symbols
static uint8_t symbuf[4096];
static int symindex = 0;

// Bit buffer and index used to track bits
static uint8_t bitbuf[4096];
static int bitindex = 0;

// Reads the bytes to infile
int read_bytes(int infile, uint8_t *buf, int to_read) {
  int readBytes = 0;
  int total = 0;
  do {
    readBytes = read(infile, buf + total, to_read - total);
    total += readBytes;
  } while (readBytes > 0 && total != to_read);
  uncomp += total;
  return total;
}

// Writes the bytes to outfile
int write_bytes(int outfile, uint8_t *buf, int to_write) {
  int writeBytes = 0;
  int total = 0;
  do {
    writeBytes = write(outfile, buf + total, to_write - total);
    total += writeBytes;
  } while (writeBytes > 0 && total != to_write);
  comp += total;
  return total;
}

// Reads in file header
void read_header(int infile, FileHeader *header) {
  read_bytes(infile, (uint8_t *)header, sizeof(FileHeader));
}

// Writes to file header
void write_header(int outfile, FileHeader *header) {
  write_bytes(outfile, (uint8_t *)header, sizeof(FileHeader));
}

// Reads symbol from infile
bool read_sym(int infile, uint8_t *byte) {
  static int end = 0;
  if (symindex == 0) {
    end = read_bytes(infile, symbuf, 4096);
  }
  *byte = symbuf[symindex++];
  if (symindex == 4096) {
    symindex = 0;
  }
  if (end == 4096) {
    return true;
  } else {
    if (symindex == end + 1) {
      return false;
    } else {
      return true;
    }
  }
}

// Buffers code and symbol into outfile
void buffer_pair(int outfile, uint16_t code, uint8_t sym, uint8_t bit_len) {
  for (int i = 0; i < bit_len; i++) {
    if ((code & 1) == 1) {
      //Set bit of bitbuf at bitindex
      bitbuf[bitindex / 8] |= (1 << (bitindex % 8));
    } else {
      //Clear bit of bitbuf at bitindex
      bitbuf[bitindex / 8] &= ~(1 << (bitindex % 8));
    }
    bitindex += 1;
    code >>= 1;
    if (bitindex == 4096 * 8) {
      write_bytes(outfile, bitbuf, 4096);
      bitindex = 0;
    }
  }

  for (int i = 0; i < 8; i++) {
    if ((sym & 1) == 1) {
      //Set bit of bitbuf at bitindex
      bitbuf[bitindex / 8] |= (1 << (bitindex % 8));
    } else {
      //Clear bit of bitbuf at bitindex
      bitbuf[bitindex / 8] &= ~(1 << (bitindex % 8));
    }
    bitindex += 1;
    sym >>= 1;
    if (bitindex == 4096 * 8) {
      write_bytes(outfile, bitbuf, 4096);
      bitindex = 0;
    }
  }
}

// Buffers out remaining contents to outfile
void flush_pairs(int outfile) {
  int bytes = 0;
  if (bitindex != 0) {
    if (bitindex % 8 == 0) {
      bytes = bitindex / 8;
    } else {
      bytes = (bitindex / 8) + 1;
    }
    write_bytes(outfile, bitbuf, bytes);
  }
}

// Reads infile into buffer
bool read_pair(int infile, uint16_t *code, uint8_t *sym, uint8_t bit_len) {
  *code = 0;
  for (int i = 0; i < bit_len; i++) {
    if (bitindex == 0) {
      read_bytes(infile, bitbuf, 4096);
    }
    // Get bit of bitbuf at bitindex
    if (bitbuf[bitindex / 8] & (1 << (bitindex % 8))) {
      // Set bit of code at i
      *code |= (1 << (i % 16));
    } else {
      // Clear bit of code at i
      *code &= ~(1 << (i % 16));
    }
    bitindex += 1;
    if (bitindex == 4096 * 8) {
      bitindex = 0;
    }
  }
  *sym = 0;
  for (int i = 0; i < 8; i++) {
    if (bitindex == 0) {
      read_bytes(infile, bitbuf, 4096);
    }
    // Get bit of bitbuf at bitindex
    if (bitbuf[bitindex / 8] & (1 << (bitindex % 8))) {
      // Set bit of sym at i
      *sym |= (1 << (i % 16));
    } else {
      // Clear bit of sym at i
      *sym &= ~(1 << (i % 16));
    }
    bitindex += 1;
    if (bitindex == 4096 * 8) {
      bitindex = 0;
    }
  }
  return *code != STOP_CODE;
}

// Buffers a word into outfile
void buffer_word(int outfile, Word *w) {
  for (uint32_t i = 0; i < w->len; i++) {
    symbuf[symindex++] = w->syms[i];
    if (symindex == 4096) {
      write_bytes(outfile, symbuf, 4096);
      symindex = 0;
    }
  }
}

// Buffers out remaining words into outfile
void flush_words(int outfile) {
  if (symindex != 0) {
    write_bytes(outfile, symbuf, symindex);
  }
}
