#include "trie.h"
#include "code.h"

// Creates a trie node
TrieNode *trie_node_create(uint16_t index) {
  TrieNode *n = (TrieNode *)calloc(1, sizeof(TrieNode));
  if (!n) {
    return 0;
  }
  n->code = index;
  return n;
}

// Deletes a trie node
void trie_node_delete(TrieNode *n) {
  free(n);
}

// Initializes a trie
TrieNode *trie_create(void) {
  return trie_node_create(EMPTY_CODE);
}

// Resets a trie back to the root node
void trie_reset(TrieNode *root) {
  for (int i = 0; i < ALPHABET; i++) {
    if (root->children[i] != NULL) {
      trie_delete(root->children[i]);
    }
  }
}

// Deletes a trie node
void trie_delete(TrieNode *n) {
  if (!n) {
    return;
  }
  for (int i = 0; i < ALPHABET; i++) {
    trie_delete(n->children[i]);
  }
  trie_node_delete(n);
}

// Steps into the next trie node
TrieNode *trie_step(TrieNode *n, uint8_t sym) {
  return n->children[sym];
}
