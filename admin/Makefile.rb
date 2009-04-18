#!/usr/bin/ruby
# tool to look through a qmake pro file and find what headers are to be 
# installed it then figures out the classes in each of those files
# and then creates the directory _include and creates files named after the 
# class names in there which #include the file that contains the class

cwd = File.dirname( __FILE__ )
require 'find'
require 'ftools'
require "#{cwd}/platform.rb"

def step3( path, classname )
puts <<-EOS
_include/lastfm/#{classname}: #{path} | _include/lastfm
	ln #{path} $@
$(DESTDIR)#{$install_prefix}/include/lastfm/#{classname}: #{path} | $(DESTDIR)#{$install_prefix}/include/lastfm
	cp #{path} $@

EOS

  $headers << "_include/lastfm/#{classname}"
  $installheaders << "$(DESTDIR)#{$install_prefix}/include/lastfm/#{classname}"
end

def step2( path )
  b = nil
  File.open( path ).each_line do |line|
    matches = /(class|struct)\s*LASTFM_[A-Z]+_DLLEXPORT\s*([a-zA-Z0-9]+)/.match( line )
    if !matches.nil?
      b = true
      step3( path, matches[2] )
    end
  end
  # if no matches, then assume one thing and just copy it, this makes it work
  # for namespaces etc.
  step3( path, File.basename( path, '.h' ) ) if b.nil?
end
################################################################################


$install_prefix = ENV['PREFIX']

exit 1 if ARGV[0].nil?
exit 1 if !File.file?( ARGV[0] )
exit 1 if $install_prefix.nil?

$installheaders = Array.new
$headers = Array.new

def mkdir( path )
  Dir.mkdir( path )
rescue SystemCallError
  # braindead for this to throw an exception on failure
  # as an additional rant, the Ruby File/Dir classes are a poo design
end

puts <<-EOS
.PHONY: all
all: headers
	cd src && $(MAKE)
	cd tests && $(MAKE)
	cd demos && $(MAKE)

EOS

while arg = ARGV.shift do
  File.open( arg ).each_line do |line|
    matches = /^\s*headers.files\s+=\s+(.*)$/.match( line )
    matches[1].split( ' ' ).each { |file| step2( File.dirname( arg ) + '/' + file ) } if !matches.nil?
  end
end

rubystring = %q[ruby -e 'Dir["_include/lastfm/*"].each { |h| puts %Q{#include "lastfm/#{File.basename h}"\n} }']

puts <<-EOS
_include/lastfm:
	mkdir -p $@
$(DESTDIR)#{$install_prefix}/include/lastfm:
	mkdir -p $@

_include/lastfm/global.h: src/global.h
	cp src/global.h $@
$(DESTDIR)#{$install_prefix}/include/lastfm/global.h: src/global.h
	cp src/global.h $@

_include/lastfm.h: | _include/lastfm
	#{rubystring} > $@
$(DESTDIR)#{$install_prefix}/include/lastfm.h:
	mkdir $(dir $@)

.PHONY: headers
headers: #{$headers.join(' ')} _include/lastfm/global.h _include/lastfm.h

.PHONY: install
install: #{$installheaders.join(' ')} $(DESTDIR)#{$install_prefix}/include/lastfm/global.h
	cd src && make install "INSTALL_ROOT=$(DESTDIR)#{$install_prefix}"

.PHONY: clean
clean:
	rm -r _include
	cd src && make clean
EOS