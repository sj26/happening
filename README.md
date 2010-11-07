# Happening

An easy way to do event-based and scheduled programming in Ruby.

Based on [libevent][libevent].

The idea, eventually, is to have asynchronous IO using Fibers which are transparent to the programmer. Originally, this was going to be called Protein (eventful fiber) which it may well end up. This is partially a learning exercise.

## Example

  Happening.in 5.seconds do
    puts 'Later'
  end
  
  Happening.next do
    puts 'First'
  end
  
  Happening.run do
    puts 'Now'
  end
  
  # => "First"
  #    "Now"
  #    (a pause)
  #    "Later"

## TODO

 * Multiple event loops
 * IO notifications
 * Process notifications
 * Signal handling

## Caveats

 * This is the first time I've written a C extension for Ruby, plus I'm a bit rusty at C. Please, be kind and tell me what I'm doing wrong.

## Copyright

Copyright (c) 2010 Samuel Cochran. See LICENSE for details.

  [libevent]: http://monkey.org/~provos/libevent