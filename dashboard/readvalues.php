<?php
$fp = fsockopen("localhost", 50033, $errno, $errstr, 30);
if (!$fp) {
	echo "$errstr ($errno)<br />\n";
} else {
	while (!feof($fp)) {
		echo fgets($fp, 128);
	}
	fclose($fp);
}
?>
