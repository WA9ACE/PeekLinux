#
# combined script: header_gen
#
# consists of:
#   header_gen_rv_swe.pl
#   header_gen_rv_test_inc.pl
#   header_gen_rv_test_exec_level.pl
#
# to be invoked with:
#   header_gen.pl { rv_swe | rv_test_inc | rv_test_exec_level } other-params
#
#   details see below
#

use strict;
#use warnings;

# Define and set global variables.
my $selector = shift;
print $selector . "   ";
my $header_file = shift;
print $header_file;
my $guard_string = "\U__${selector}_H__";


#
# Function used to open and write the header to the include file.
#
sub open_header {
    # open the file
    open (HDR,">$header_file")||die "can't open $header_file \n";

    # print guard pattern
    print HDR "#ifndef $guard_string\n";
    print HDR "#define $guard_string\n\n";
}

#
# Function used to write the footer and close the include file.
#
sub close_footer {
    # end guard pattern
    print HDR "\n#endif /* $guard_string */\n";

    # close the file
    close HDR;
}


if ($selector eq 'rv_swe')
{

  open_header();

foreach (@ARGV) {
    #look for the following pattern
    if(/^(SWE_)?(\w+)_STATE=(\d+)/)
    {
      if ($3 == 1 || $3 == 2)
      {
        #print everything
        print HDR "#define RVM_${2}_SWE\n";
      }
    }
}

  close_footer();

}
elsif ($selector eq 'rv_test_inc')
{

#
# header_gen_rv_test_inc.pl : tool to generate header files
#
# Short spec:
#
# This tool generates one header file using the information from the command
# line. The expected fommand line form is:
#
# header-file-name other-parameters
#
# where header-file-name is the filename of the include file to be generated
# (including path components).
# other-parameters parameters define an association of names to values. each
# association has the form:
#
# name=value
#
# name may have an prefix SWE_, which is then ignored. name must have the
# suffix _STATE, otherwise this particular association is ignored.
# only the component between SWE_ and _STATE gets considered. value must
# be 2, otherwise this particular association is ignored.
#
# The tool generates from the accepted (not ignored) a list of defines.
# the identifiers are the considered part of the other-parameters, but have
# the prefix TEST_ and suffix _SWE. The values they define start with 1,
# increasing.
# the order of the parameters is not important. However, it must be the same
# as of the tool header_gen_rv_test_exec_level.pl (see there).
# Conveniently both tools should be invoked with the same other-parameters.
#

  open_header();

my $counter = 1;

foreach (@ARGV) {
    #look for the following pattern
    if(/^(SWE_)?(\w+)_STATE=(\d+)/)
    {
      if ($3 == 2)
      {
        print HDR "#define TEST_${2}_SWE ($counter)\n";
        $counter++;
      }
    }
}

  close_footer();

}
elsif ($selector eq 'rv_test_exec_level')
{

#
# header_gen_rv_test_exec_level.pl : tool to generate header files
#
# Short spec:
#
# This tool generates one header file using the information from the command
# line. The expected fommand line form is:
#
# header-file-name other-parameters
#
# where header-file-name is the filename of the include file to be generated
# (including path components).
# other-parameters parameters define an association of names to values. each
# association has the form:
#
# name=value
#
# name may have an prefix SWE_, which is then ignored. name must have the
# suffix _STATE, otherwise this particular association is ignored.
# only the component between SWE_ and _STATE gets considered. value must
# be 2, otherwise this particular association is ignored.
#
# the contents of the header file will be of the ANSI C language.
# it is a the definition of an array:
# static char TEST_LEVEL_LCD [TEST_LEVEL_LCD_LEN][TEST_LEVEL_LCD_STR_LEN] =
# having 0-terminated strings as components.
# the values of TEST_LEVEL_LCD_LEN and TEST_LEVEL_LCD_STR_LEN and the elements
# of the array depend on the parameters:
# TEST_LEVEL_LCD_LEN is the number of considered (or not ignored)
# other-parameters PLUS 1.
# TEST_LEVEL_LCD_STR_LEN is the length of the strings. various rules apply,
# e.g. for 0 termination (but at last, it will get the value of 15!).
# see below and/or ask a ANSI C guru.
# the strings have the form: running index," - ", considered name component
# (max 8 chars), filling blanks (up to the final length)
# the running index use 3 characters, i.e. no more than 999 considered indexes
# are allowed. this is no problem for the time being.
# Thus we can compute the max length: 3+3+8+1=15 (running index, delimiter,
# name, string delimiter \0).
# There is one special initial string. it is always placed at index 0 and has
# the contents "  TEST LEVEL  " plus '\0'.
# the 2 spaces at the beginning are legacy, I guess they are just to have a
# left alignment with the first 9 indexes (those with 1 figure indexes)
#

if (@ARGV > 999)
{
  die "cannot stand it - too many arguments";
}

  open_header();

  my $max_name_len = 8;
my @local_args;
foreach (@ARGV)
{
    if(/^(SWE_)?(\w+)_STATE=(\d+)/ && $3 == 2)
    {
        push @local_args, substr($2, 0, $max_name_len);
    }
}

print HDR "/* RV TEST TEST_LEVEL PARAMETER */\n";
print HDR "#define TEST_LEVEL_LCD_LEN     (".(@local_args+1).")\n";
  print HDR "#define TEST_LEVEL_LCD_STR_LEN (".(3+3+$max_name_len+1).")\n\n";
print HDR "static char TEST_LEVEL_LCD [TEST_LEVEL_LCD_LEN][TEST_LEVEL_LCD_STR_LEN] = {\n";

# the literal constant 1 represents the single char at the beginning ('T')
# this is due to the fact that normally the first 3 chars are reserved for the
# running index
  my @print_lines = ("\"" . sprintf("  %-*s", 1+3+$max_name_len, "TEST LEVEL") . "\"");

  my $counter = 1;
  foreach (@local_args)
  {
    push @print_lines, "\"" . sprintf("%3d - %-*s", $counter, $max_name_len, $_) . "\"";
    $counter++;
}

print HDR "    ", join ",\n    ", @print_lines;

print HDR "};\n";

  close_footer();

}
else
{
    die "$selector not recognized";
}
