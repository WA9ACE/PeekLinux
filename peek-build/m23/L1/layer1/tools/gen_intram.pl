#!/usr/bin/perl

use Cwd;
if ($#ARGV == -1)
  {
  die "\nSyntax is gen_intram <C file (with extension)>\n\n";
  }

$#ARGV ==2  or die "Incorrect number of parameters";

$c_input_file = @ARGV[1];


# -------------------------------------------------------------------------------
# Remove .c extension and create the new filename for the processed file

#@file_name = split /\.c/, $c_input_file;
#$c_output_file = join ('', @file_name, "_intram.c");
@file_name = reverse(split /\//, $c_input_file);
@file = split /\./, @file_name[0];
$c_output_file = @ARGV[2]."/".@file[0]."_intram.c";

print "Output file: @file[0]\n";

# -------------------------------------------------------------------------------
# Generates the C file for internal RAM

open input_stream, $c_input_file
  or die "\n\n Input C file $C_input_file cannot be opened: $!\n";

open(output_stream,'>'.$c_output_file)
  or die "\n\nCan't open output file $C_output_file: $!";		

printf output_stream "/* ----- WARNING - File automatically generated - Do not modify it ----- */\n\n\n\n\n";

$d_internal_found = 0;

# Search for data
while(<input_stream>)
  {
  chomp $_;

  # Search for start comment
  if ( / *#pragma .*DUPLICATE_FOR_INTERNAL_RAM_START/ )
    {
    # DEBUG
    #print "Found begin of string\n";
    $d_internal_found = 1;

    if (/ *#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_START/)
      {
        printf output_stream "#if (GSM_IDLE_RAM != 0)\n";
        # check if GSM IDLE FEATURE
      }

    if (/ *#pragma GSM_IDLE2_DUPLICATE_FOR_INTERNAL_RAM_START/)
      {
        printf output_stream "#if (GSM_IDLE_RAM == 2)\n";
        # check if GSM IDLE FEATURE
      }

    # Found begin internal RAM code, then output up to the end string
    while(<input_stream>)
      {
      chomp $_;
      if ( / *#pragma .*DUPLICATE_FOR_INTERNAL_RAM_END/ )
        {
        # Found end of the internal RAM code, then search for another one
        # DEBUG
        #print "Found end of string\n";

        if (/ *#pragma GSM_IDLE_DUPLICATE_FOR_INTERNAL_RAM_END/)
          {
            printf output_stream "#endif\n";
            # check if GSM IDLE FEATURE         
          }
        last;
        }
      else
        {
        printf output_stream "%s\n", $_;
        # DEBUG
        #print "Copied lines - $_\n";
        }
      }

    }
  }

close input_stream;
close output_stream;
