#!/usr/bin/php
<?php

/* Check Task stacks for overflows */

if($_SERVER["argc"] < 2) {
	echo "syntax: ./checkdar.php dar.txt\n";
	exit(1);
}

$dar = file_get_contents($_SERVER["argv"][1]);

$darsplit = explode("\n", $dar);

for($i=0;$i < count($darsplit);$i++) {
		if(strstr($darsplit[$i], "tc_name")) {
			$tcname = explode(":", $darsplit[$i]);
			$tcname = $tcname[1];
		}
		if(strstr($darsplit[$i], "tc_stack_start")) {
			$stack_start = explode(":", $darsplit[$i]);
			$stack_end =  explode(":", $darsplit[$i+1]);
			$sp = explode(":",$darsplit[$i+2]);
			if(hexdec($sp[1]) > hexdec($stack_end[1])) {
				$ovbytes = hexdec($sp[1]) -  hexdec($stack_end[1]);
				echo "Task name: $tcname", "\n";
            	echo "Stack Start: ", $stack_start[1],"\n";
            	echo "Stack End: ", $stack_end[1], "\n";
            	echo "Stack Pointer:", $sp[1], "\n";
				echo "Stack overflowed: $ovbytes bytes\n";
			}
		}

}

?>

