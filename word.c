#include "word.h"
#include "code.h"

// Creates a word
Word *word_create(uint8_t *syms, uint64_t len) {
  Word *w = (Word *)malloc(sizeof(Word));
  if (!w) {
    return 0;
  }
  w->len = len;
  w->syms = (uint8_t *)calloc(1, sizeof(uint8_t) + w->len);
  if (!w->syms) {
    free(w);
    return 0;
  }
  for (uint32_t i = 0; i < len; i++) { // Copy symbols to word struct
    w->syms[i] = syms[i];
  }
  return w;
}

// Appends a symbol to the end of a word and creates a new word
Word *word_append_sym(Word *w, uint8_t sym) {
  Word *x = (Word *)malloc(sizeof(Word));
  if (!x) {
    return 0;
  }
  x->len = w->len + 1; // Makes 1 larger than w
  x->syms = (uint8_t *)calloc(1, sizeof(uint8_t) + x->len);
  if (!x->syms) {
    free(x);
    return 0;
  }
  uint32_t i = 0;
  while (i < w->len) { // Copies w to x
    x->syms[i] = w->syms[i];
    i++;
  }
  x->syms[i] = sym; // Appends sym to end of x
  return x;
}

// Deletes a word
void word_delete(Word *w) {
  free(w->syms);
  free(w);
}

// Creates a word table
WordTable *wt_create(void) {
  WordTable *wt = (WordTable *)calloc(MAX_CODE, sizeof(Word));
  if (!wt) {
    return 0;
  }
  wt[EMPTY_CODE] = word_create(NULL, 0);
  return wt;
}

// Resets a word table
void wt_reset(WordTable *wt) {
  for (uint32_t i = START_CODE; i < MAX_CODE; i++) {
    if (wt[i] != NULL) {
      word_delete(wt[i]);
    }
  }
}

// Deletes a word table
void wt_delete(WordTable *wt) {
  for (uint32_t i = 0; i < MAX_CODE; i++) {
    if (wt[i] != NULL) {
      word_delete(wt[i]);
    }
    free(wt);
  }
}
