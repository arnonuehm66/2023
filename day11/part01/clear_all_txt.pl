#!/usr/bin/perl
use strict;

sub cmd($) { my ($cmd) = @_;
  print "[$cmd]\n";
  system($cmd);
}

cmd('echo > testdata.txt');
cmd('echo > data.txt');
cmd('echo > info.txt');
