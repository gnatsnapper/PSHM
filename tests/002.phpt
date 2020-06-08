--TEST--
Default Instantiation and Unlink Test
--SKIPIF--
<?php
if (!extension_loaded('pshm')) {
	echo 'skip';
}
?>
--FILE--
<?php
$pshm = new PSHM(bin2hex(random_bytes(8)));
var_dump($pshm->unlink());
?>
--EXPECT--
bool(true)
