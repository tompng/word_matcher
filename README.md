# WordMatcher

```ruby
# Gemfile
gem 'word_matcher', github: 'tompng/word_matcher'
```

```ruby
banned_list = WordMatcher.new %w[banned_word_00001 banned_word_00002 ... banned_word_99999]
banned_list.match? 'hello world'
#=> false
banned_list.match? 'hello banned_word_99999'
#=> true
banned_list.scan 'Lorem ipsumbanned_word_99999 dobanned_word_00002lor sit amet'
#=> ["banned_word_99999", "banned_word_00002"]
```
