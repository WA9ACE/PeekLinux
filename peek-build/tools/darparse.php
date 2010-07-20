#!/usr/bin/php
<?php
/* Seriously don't laugh */
function symbol_find($addr, $symbol_data) {
	$count = 1000;
	for($i=1; $i < count($symbol_data);$i++) {

        	if($symbol_data[$i+1]['addr'] == "ffffffff")
                	    break;

		if(hexdec($symbol_data[$i]['addr']) <= hexdec($addr)) {
                	if((hexdec($addr) - hexdec($symbol_data[$i]['addr'])) < $count) {
                        	$found = $symbol_data[$i]['name'];
                        	$foundaddr = $symbol_data[$i]['addr'];
                        	$count = hexdec($addr) - hexdec($symbol_data[$i]['addr']);
                	}
        	}

	}
	if(isset($found)) {
        	echo "Symbol: $found - Address: $foundaddr";
	}
}

$fp = @fopen($_SERVER["argv"][1], "r");
if(!$fp) {
	echo "Usage: ./darparse.php rel_n5_camera_micron_vin.map dar.txt\n";
	exit();
}
$contents = fread($fp, filesize($_SERVER["argv"][1]));


$mapdata = strstr($contents, "GLOBAL SYMBOLS:");
$mapdata = strstr($mapdata, "--------   ----");
$mapdata = explode("\n", $mapdata);

$symbol_data = array();
for($i=1;$i < count($mapdata);$i++) {
	$sd = explode(" ", $mapdata[$i]);
	$symbol_data[$i]['addr'] = $sd[0];
	@$symbol_data[$i]['name'] = $sd[3];

}
$addr = $_SERVER["argv"][2];

fclose($fp);
$fp = fopen($_SERVER["argv"][2], "r");

$contents = fread($fp, filesize($_SERVER["argv"][2]));
$dardata = explode("-------------------------------", $contents);
$regs = explode("\n", $dardata[4]);
for($i=1;$i <= 15;$i++) {
	$reg = explode("=", $regs[$i]);
	echo $reg[0], $reg[1], " - ", symbol_find($reg[1], $symbol_data), "\n";
}
echo "----------------\n";
echo "Stack data\n";

$stackdata = explode("\n", $dardata[5]);
for($i=1;$i < count($stackdata);$i++) {
	if(is_numeric($stackdata[$i])) {
		echo $stackdata[$i], " - ", symbol_find($stackdata[$i], $symbol_data), "\n";	
	}
}

?>
