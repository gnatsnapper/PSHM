--TEST--
Default Share Between Objects Test
--SKIPIF--
<?php
if (!extension_loaded('pshm')) {
	echo 'skip';
}
?>
--FILE--
<?php
$name = bin2hex(random_bytes(8));
$pshm1 = new PSHM($name);
$pshm2 = new PSHM($name);
var_dump( $pshm1->write("hello") );
var_dump( $pshm2->read() );
var_dump($pshm1->unlink());
?>
--EXPECT--
bool(true)
string(5) "hello"
bool(true)
