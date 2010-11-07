#!/usr/bin/env ruby

require "mkmf"

find_library("event", "event_base_new")
create_makefile("happening")
