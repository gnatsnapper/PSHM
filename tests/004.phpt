--TEST--
Default Multi Read and Write Test
--SKIPIF--
<?php
if (!extension_loaded('pshm')) {
	echo 'skip';
}
?>
--FILE--
<?php
$pshm = new PSHM(bin2hex(random_bytes(8)));
var_dump( $pshm->write("hello") );
var_dump( $pshm->read() );
var_dump( $pshm->write("hello") );
var_dump( $pshm->write("hello") );
var_dump( $pshm->read() );
var_dump( $pshm->write("goodbye") );
var_dump( $pshm->read() );
var_dump( $pshm->read() );
var_dump($pshm->unlink());
?>
--EXPECT--
bool(true)
string(5) "hello"
bool(true)
bool(true)
string(5) "hello"
bool(true)
string(7) "goodbye"
string(7) "goodbye"
bool(true)
