#!/usr/bin/perl
use strict;

sub cmd($) { my ($cmd) = @_;
  print "[$cmd]\n";
  system($cmd);
}

#cmd('gcc -Wall -Ofast rev_main.c -o rev_main -lpcre2-8');
cmd('gcc -Wall -g rev_main.c -o rev_main -lpcre2-8');
