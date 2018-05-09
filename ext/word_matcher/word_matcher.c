#include "word_matcher.h"

VALUE rb_mWordMatcher;

void
Init_word_matcher(void)
{
  rb_mWordMatcher = rb_define_module("WordMatcher");
}
