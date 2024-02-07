#!/usr/bin/perl
use strict;

my @g_aSeedsStart = ();
my @g_aSeedsRange = ();
my @g_aTo         = ();
my @g_aFrom       = ();
my @g_aRange      = ();


#******************************************************************************
sub getLinesFromFile($$) { my ($file, $paLines) = @_;
  my $line = "";

  return -1 unless open(IN, "<", $file);

  while ($line = <IN>) {
    chomp($line);
    next if $line eq '';
    push(@$paLines, $line);
    printf("Line %3lu >> %s\n", $. - 1, $line);
  }

  close(IN);

  return 0;
}

#******************************************************************************
sub populateArrays($) { my ($paLines) = @_;
  my $to        = 0;
  my $from      = 0;
  my $range     = 0;

  # seeds: 79 14 55 13
  my $tRxSeeds = qr/(\d+) (\d+)/;
  # seed-to-soil map:
  # 50 98 2
  my $tRxToFromRange = qr/(map:)|(?:(\d+) (\d+) (\d+))/;

  # Get all seeds.
  while ($paLines->[0] =~ /$tRxSeeds/g) {
    push(@g_aSeedsStart, $1);
    push(@g_aSeedsRange, $2);
  }

# DEBUG_PRINT
#   print "-" x 80, "\n";
# endif

  # Get all converters.
  for (my $i = 1; $i < @$paLines; ++$i) {
    $paLines->[$i] =~ $tRxToFromRange;

    # Mark next converter or fimy current converter.
    if ($1 eq 'map:') {
      $to    = -1;
      $from  = -1;
      $range = -1;
    }
    else {
      $to    = $2;
      $from  = $3;
      $range = $4;
    }

    push(@g_aTo,    $to);
    push(@g_aFrom,  $from);
    push(@g_aRange, $range);

# DEBUG_PRINT
#     printf("push(@g_aTo,    %lu\n", $to);
#     printf("push(@g_aFrom,  %lu\n", $from);
#     printf("push(@g_aRange, %lu\n", $range);
#     print "-" x 40, "\n";
# endif
  }

# DEBUG_PRINT
#   print "-" x 80, "\n";
# endif
}

#******************************************************************************
sub getLocationFromSeed($) { my ($seed) = @_;
  my $from      = 0;
  my $to        = 0;
  my $range     = 0;
  my $converted = 0;

  for (my $i = 1; $i < @g_aFrom; ++$i) {
    $from  = $g_aFrom[$i];
    $to    = $g_aTo[$i];
    $range = $g_aRange[$i];

    if ($to == -1) {
      if ($converted) {
        $converted = 0;
        next;
      }
    }

    next if $converted;

    if ($seed >= $from && $seed < $from + $range) {
      $seed = $seed - $from + $to;
      $converted = 1;
    }
  }

  return $seed;
}

#******************************************************************************
sub getLowestLocReverseSearch() {
  my $lowestLoc = ~0;
  my $ocation   = 0;
  my $start     = 0;
  my $range     = 0;

  # Now get the location from each seed.
  print "-" x 20, "\n";
  # printf("   Seed      Location\n");
  for (my $i = 0; $i < @g_aSeedsStart; ++$i) {
    $start = $g_aSeedsStart[$i];
    $range = $g_aSeedsRange[$i];

    for (my $seed = $start; $seed < $start + $range; ++$seed) {
      $ocation = getLocationFromSeed($seed);
      # printf("%7lu %7lu\n", $seed, $ocation);
      $lowestLoc = $ocation if $lowestLoc > $ocation;
    }
  }
  print "-" x 20, "\n";

  return $lowestLoc;
}


#******************************************************************************
sub main() {
  my @lines     = ();
  my $lowestLoc = 0;
  my $file      = shift(@ARGV);

  die "No file given\n"      if $file eq '';
  die "Can't open '$file'\n" if getLinesFromFile($file, \@lines) == -1;

  populateArrays(\@lines);

  $lowestLoc = getLowestLocReverseSearch();
  printf("\n\$lowestLoc = %lu\n", $lowestLoc);
}

main()
__END__
