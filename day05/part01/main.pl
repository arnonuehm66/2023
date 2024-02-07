#!/usr/bin/perl
use strict;

use constant INT_MAX => 9999999999999999999;


#*******************************************************************************
sub getLeastLoc($$$$) { my ($pSeeds, $pTo, $pFrom, $pRange) = @_;
  my $leastLoc  = INT_MAX;
  my @seedsIn   = @$pSeeds;
  my @locations = ();
  my $converted = 0;
  my $count     = 0;

  my ($to, $from, $range) = (0, 0, 0);

  print "Seeds: ", join(' ', @seedsIn), "\n";

  for my $seed (@seedsIn) {

    $converted = 0;
    for (my $i = 0; $i < @$pTo; ++$i) {
      $to    = $pTo->[$i];
      $from  = $pFrom->[$i];
      $range = $pRange->[$i];

      if ($to == -1) {
        if ($converted) {
          $converted = 0;
          next;
        }
      }

      next if $converted;

      if ($seed >= $from and $seed <= $from + $range) {
        $seed = $seed - $from + $to;
        $converted = 1;
      }
    }
    $locations[$count++] = $seed;
  }

  printf "Locations: ";
  for (my $i = 0; $i < @locations; ++$i) {
    printf "$locations[$i] ($leastLoc) ";
    if ($leastLoc > $locations[$i]) {
      $leastLoc = $locations[$i];
    }
  }
  print "\n";

  return $leastLoc;
}


#*******************************************************************************
sub main() {
  my $file    = shift(@ARGV);
  my $line    = '';
  my @seeds   = ();
  my @to      = ();
  my @from    = ();
  my @range   = ();
  my $i       = 0;
  my $leatLoc = 0;

  die "No file given\n" if $file eq '';

  open(IN, '<', $file);

  $line  = <IN>;
  $line  =~ s/seeds: //;
  @seeds = split(/ /, $line);
  # Skip the first 'map:' entry.
  $line  = <IN>;
  $line  = <IN>;

  while ($line = <IN>) {
    chomp($line);
    next if $line eq '';
    print "$. >> $line\n";

    if ($line =~ /map:/) {
      ($to[$i], $from[$i], $range[$i]) = (-1, -1, -1);
    }
    else {
      ($to[$i], $from[$i], $range[$i]) = $line =~ /(\d+)\D+(\d+)\D+(\d+)/;
    }
    ++$i;
  }
  print '-' x 80, "\n";

  $leatLoc = getLeastLoc(\@seeds, \@to, \@from, \@range);
  print "$leatLoc\n";
}

main();
__END__
