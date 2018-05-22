require 'test_helper'
require 'benchmark'
class WordMatcherTest < Minitest::Test
  def test_that_it_has_a_version_number
    refute_nil ::WordMatcher::VERSION
  end

  def test_match
    ng = WordMatcher.new %w[apple orange sugar]
    assert_equal ng.match?('appel range suga'), false
    %w[apple orange sugar pineapple orangejuice susugarar].all? do |text|
      assert_equal ng.match?(text), true
    end
  end

  def test_scan
    ng = WordMatcher.new %w[oregon orange applepie applejuice z]
    text = 'pineapplepie oregonjuice zorangez'
    matches = %w[applepie oregon z orange z]
    assert_equal ng.scan(text), matches
  end

  def rand_text size
    Array.new(size) { [*'a'..'z'].sample }.join
  end

  def test_speed_10
    words = Array.new(10) { rand_text 4 }
    text = rand_text(1000).upcase
    ng = WordMatcher.new words
    simple_match = ->(s, ws) { ws.any? { |w| s.include? w } }
    time_match = Benchmark.measure { simple_match.call text, words }.real
    time_ng_match = Benchmark.measure { ng.match? text }.real
    assert_operator time_ng_match, :<, time_match
  end

  def test_speed_10000
    words = Array.new(10000) { rand_text 4 }
    simple_match = ->(s, ws) { ws.any? { |w| s.include? w } }
    simple_scan = ->(s, ws) { ws.select { |w| s.include? w } }
    ng = WordMatcher.new words
    text = rand_text 10_000
    no_match_text = text.upcase
    time_match = Benchmark.measure { simple_match.call no_match_text, words }.real
    time_ng_match = Benchmark.measure { ng.match? no_match_text }.real
    assert_operator time_ng_match * 64, :<, time_match
    time_scan = Benchmark.measure { simple_scan.call text, words }.real
    time_ng_scan = Benchmark.measure { ng.scan text }.real
    assert_operator time_ng_scan * 64, :<, time_scan
    assert_equal simple_scan.call(text, words).uniq.sort, ng.scan(text).uniq.sort
  end
end
