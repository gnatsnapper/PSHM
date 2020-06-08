--TEST--
PSHM_RDONLY and PSHM_RDWR Flags Test
--SKIPIF--
<?php
if (!extension_loaded('pshm')) {
	echo 'skip';
}
?>
--FILE--
<?php
$name = bin2hex(random_bytes(8));
$pshm1 = new PSHM($name,PSHM_CREAT | PSHM_RDWR);
$pshm2 = new PSHM($name, PSHM_RDONLY);
var_dump( $pshm1->write("hello") );
var_dump( $pshm2->read() );
try { $pshm2->write("goodbye"); } catch (Exception $e) { var_dump($e->getMessage() );  }
var_dump( $pshm1->read() );
var_dump( $pshm2->read() );
var_dump($pshm1->unlink());
?>
--EXPECT--
bool(true)
string(5) "hello"
string(33) "Bad permissions for mapped region"
string(5) "hello"
string(5) "hello"
bool(true)
