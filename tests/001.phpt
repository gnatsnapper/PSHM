--TEST--
Check if pshm is loaded
--SKIPIF--
<?php
if (!extension_loaded('pshm')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "pshm" is available';
?>
--EXPECT--
The extension "pshm" is available
