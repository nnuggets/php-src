--TEST--
Check for sqreen presence
--INI--
;disable warning output(fopens)
display_errors=Off
;log error on output
error_log=
--SKIPIF--
<?php if (!extension_loaded("sqreen")) print "skip"; ?>
--FILE--
<?php 
echo "sqreen extension is available\n";
// No log
$infile = fopen("test.text", "r"); //or die("Unable to open file!");
echo fread($infile,filesize("test.text"));
fclose($infile);

// Log
sqreenon();
$infile = fopen("test.text", "r"); // or die("Unable to open file!");
echo fread($infile,filesize("test.text"));
fclose($infile);

$infile = fopen("test.text", "z"); // or die("Unable to open file!");
echo fread($infile,filesize("test.text"));
fclose($infile);

// No log
sqreenoff();
$infile = fopen("test.text", "r"); // or die("Unable to open file!");
?>
--EXPECT--
sqreen extension is available
fopen('test.text', 'r'[, 0, 0])
fopen('test.text', 'z'[, 0, 0])
