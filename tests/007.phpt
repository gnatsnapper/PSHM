--TEST--
Default Write Overflow Test
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
try { $pshm->write(str_repeat("a",1025)); } catch (Exception $e) { var_dump($e->getMessage() ); }
var_dump( $pshm->read() );
var_dump($pshm->unlink());
?>
--EXPECT--
bool(true)
string(5) "hello"
string(35) "Message too large for mapped region"
string(5) "hello"
bool(true)
