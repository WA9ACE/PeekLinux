#!perl

# generate a dynamic Linker command file 
# PARAMETER ARE:

# 0         : Name of the file with the parameters
# 1         : Name of the Linker command file (output)
# 2         : value of the TOOL_CHOICE variable
# 3         : Name of the Linker command file template
# 4 - (n-1) : Name of the libraries

#$argcount = @ARGV;
my $lnk_opt_file=$ARGV[0];
my $cmd_file=$ARGV[1];
my $toolchoice_type = $ARGV[2];
my $cmd_file_temp=$ARGV[3];
my %section;


open (TMP,$lnk_opt_file)||die "can't open file\n";

# parse the parameter file and construct replacement strings
#
# the file contains entries of the following form
# (BSS_LIBS (.bss))
# or
# (BSS_LIBS (.bss) CONST_LIBS (.text, .const))
# or
# (BSS_LIBS (.bss)) (BSS_LIBS (.bss) CONST_LIBS (.text, .const)) 
#
# the syntax is as follows, white space and line breaks are not significant
# <ENTRY> := <lp> <PAIR><lp>
# <PAIR> := <name> <SECTION>
# <SECTION> := <lp><free_text><lp>
# <lp> := (
# <rp> := )
# <name> := [A-Za-z_]+
# <free_text> := [^()]+
#
# each entry will be linked with one library passed on the command line

my $line;
my $count;
while ($line=<TMP>) {
  # find <ENTRY>
  while ($line =~ /\((\s*\w+\s*\([^\(\)]+\))+\)/) {
    $line = $';
    $match = $&;
    # find <PAIR>
    while ($match =~  /\s*(\w+)\s*(\([^\(\)]+\))/  ) {
        $match = $';
        $section{"$1"} .= "\"$ARGV[$count+4]\" $2\n";
    }
    $count++;
  }
}

open (TMP1, ">$cmd_file");
open (TMP,$cmd_file_temp);
while ($line=<TMP>) {
  my $replaced = 0;
  foreach $key (keys(%section)) {
    if ($line =~ /\($key\)/g) {
			# insert the Libs
			$line = $section{$key};
      $replaced = 1;
    }
  }
  # NEW COMPILER MANAGEMENT 
  # If use of VISUAL LINKER, needs to manage trampoline download.
  # Case of:
  #      - TOOL_CHOICE == 0 => compiler v1.22e with vlinker v1.9902
  #      - TOOL_CHOICE == 3 => compiler & linker v2.54
  #      - TOOL_CHOICE == 7 => compiler & linker v2.58

  if ($toolchoice_type == 0) {
    $line =~ s(COMMENT1START)(/*);
    $line =~ s(COMMENT1END)(*/);
    $line =~ s(COMMENT2START)();
    $line =~ s(COMMENT2END)();
  } else {
    $line =~ s(COMMENT1START)();
    $line =~ s(COMMENT1END)();
    $line =~ s(COMMENT2START)(/*);
    $line =~ s(COMMENT2END)(*/);
  }

  if ($replaced || !($line =~ /^\s*\$\(\s*\w+\s*\)\s*$/ )) {
    print TMP1 $line;
  }
}
close TMP;
close TMP1;
unlink $tmp_file;
