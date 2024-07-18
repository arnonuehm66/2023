#!/usr/bin/perl
use strict;

my $doit = shift;

sub cmd($) { my ($cmd) = @_;
  print "[$cmd]\n";
  system($cmd);
}

cmd("find -name 'main' | perl -lne 'print if not /\\.git/'");
cmd("find -name 'main' | perl -lne 'print if not /\\.git/' | xargs rm") if $doit ne '';
