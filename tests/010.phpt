--TEST--
PSHM_TRUNC Flag Test
--SKIPIF--
<?php
if (!extension_loaded('pshm')) {
	echo 'skip';
}
?>
--FILE--
<?php
$name = bin2hex(random_bytes(8));
$pshm1 = new PosixSharedMemory($name, PSHM_CREAT | PSHM_RDWR);
var_dump( $pshm1->write("hello") );
var_dump( $pshm1->read() );
$pshm2 = new PosixSharedMemory($name, PSHM_CREAT | PSHM_RDWR | PSHM_TRUNC);
var_dump( $pshm1->read() );
var_dump( $pshm1->write("hello") );
var_dump( $pshm1->read() );
var_dump($pshm1->unlink());
?>
--EXPECT--
bool(true)
string(5) "hello"
string(0) ""
bool(true)
string(5) "hello"
bool(true)
