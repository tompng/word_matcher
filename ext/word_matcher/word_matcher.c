#include "word_matcher.h"

VALUE word_matcher_class;

typedef struct {
  VALUE edge;
  void* children[256];
} tree_node;

typedef struct {
  VALUE words;
  tree_node *root;
} word_matcher_struct;

tree_node* alloc_tree_node() {
  tree_node *node = ALLOC(tree_node);
  node->edge = Qnil;
  for (int i = 0; i < 256; i++) node->children[i] = NULL;
  return node;
}

void tree_add(tree_node *node, VALUE rstr) {
  char *s = RSTRING_PTR(rstr);
  char *s_end = s + RSTRING_LEN(rstr);
  while (s != s_end) {
    unsigned char c = *s;
    if (node->children[c]) {
      node = node->children[c];
    } else {
      node = node->children[c] = alloc_tree_node();
    }
    s++;
  }
  node->edge = rstr;
}

void tree_release(tree_node *node) {
  for (int i = 0; i < 256; i++) {
    tree_node *child = node->children[i];
    if (child) tree_release(child);
  }
  ruby_xfree(node);
}

VALUE rb_word_matcher_words(VALUE self) {
  word_matcher_struct *ptr;
  Data_Get_Struct(self, word_matcher_struct, ptr);
  return ptr->words;
}

#define word_matcher_search(embed_code) word_matcher_struct *ptr;\
  Data_Get_Struct(self, word_matcher_struct, ptr);\
  char *str = RSTRING_PTR(text);\
  long size = RSTRING_LEN(text);\
  unsigned char c;\
  for (int i = 0; i < size; i++) {\
    c = str[i];\
    tree_node *node = ptr->root->children[c];\
    if (!node) continue;\
    int j = i;\
    if (node->edge != Qnil) { embed_code; break; }\
    for (j = i + 1; j < size; j++) {\
      c = str[j];\
      node = node->children[c];\
      if (!node) break;\
      if (node->edge != Qnil) { embed_code; break; }\
    }\
  }

VALUE rb_word_matcher_match(VALUE self, VALUE text) {
  word_matcher_search({
    return Qtrue;
  })
  return Qfalse;
}

VALUE rb_word_matcher_scan(VALUE self, VALUE text) {
  VALUE matched = rb_ary_new();
  word_matcher_search({
    rb_ary_push(matched, node->edge);
  });
  return matched;
}

void word_matcher_struct_mark(word_matcher_struct *ptr) {
  rb_gc_mark(ptr->words);
}

void word_matcher_struct_free(word_matcher_struct *ptr) {
  tree_release(ptr->root);
  ruby_xfree(ptr);
}

VALUE word_matcher_alloc(VALUE self, VALUE arg) {
  word_matcher_struct *ptr = ALLOC(word_matcher_struct);
  VALUE words = rb_ary_new();
  ptr->words = words;
  ptr->root = alloc_tree_node();
  RARRAY_PTR_USE(arg, argarray, {
    for (int i = 0; i < RARRAY_LEN(arg); i++) {
      VALUE str = StringValue(argarray[i]);
      str = rb_str_dup(str);
      rb_str_freeze(str);
      rb_ary_push(words, str);
      tree_add(ptr->root, str);
    }
  });
  rb_ary_freeze(words);
  return Data_Wrap_Struct(word_matcher_class, word_matcher_struct_mark, word_matcher_struct_free, ptr);
}

void Init_word_matcher(void) {
  word_matcher_class = rb_define_class("WordMatcher", rb_cObject);
  rb_define_singleton_method(word_matcher_class, "new", word_matcher_alloc, 1);
  rb_define_method(word_matcher_class, "words", rb_word_matcher_words, 0);
  rb_define_method(word_matcher_class, "match?", rb_word_matcher_match, 1);
  rb_define_method(word_matcher_class, "scan", rb_word_matcher_scan, 1);
}
