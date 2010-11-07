#!/usr/bin/env ruby -I .

require 'happening'

Happening.next do
  puts 'blah'
end

Happening.in 2 do
  puts 'later'
end

Happening.at Time.now.to_i + 5 do 
  puts 'later again'
end

Happening.run
