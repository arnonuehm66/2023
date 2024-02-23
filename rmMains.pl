#!/usr/bin/perl
use strict;

sub cmd($) { my ($cmd) = @_;
  print "[$cmd]\n";
  system($cmd);
}

cmd("find -name 'main' -exec echo rm {} \;");
