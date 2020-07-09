--TEST--
Default Write and Read Test
--SKIPIF--
<?php
if (!extension_loaded('pshm')) {
	echo 'skip';
}
?>
--FILE--
<?php
$pshm = new PosixSharedMemory(bin2hex(random_bytes(8)));
var_dump( $pshm->write("hello") );
var_dump( $pshm->read() );
var_dump($pshm->unlink());
?>
--EXPECT--
bool(true)
string(5) "hello"
bool(true)
