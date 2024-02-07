#!/usr/bin/perl
use strict;

sub cmd($) { my ($cmd) = @_;
  print "[$cmd]\n";
  system($cmd);
}

cmd('gcc main.c -o main -lpcre2-8');
