#!/usr/bin/env ruby -I .

require 'happening'

Happening.next do
  puts 'blah'
end

Happening.in 2.seconds do
  puts 'later'
end

Happening.at Time.now.to_i + 5 do 
  puts 'later again'
end

# Happening.on_files("*").are(:readable) do
#   ...
# end
# 
# Happening.on_port :http do
#   before :connect do
#   end
#   
#   on :connect do
#   end
#   
#   on :read do
#   end
#   
#   on :write do
#   end
#   
#   on :disconnect do
#   end
# end
# 
# socket = Socket.new ...
# Happening.on_socket socket do
#   on :readable do
#   end
#   
#   on :writeable do
#   end
# end

Happening.run
