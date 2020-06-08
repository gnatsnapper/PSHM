--TEST--
Mode Test
--SKIPIF--
<?php
if (!extension_loaded('pshm')) {
	echo 'skip';
}
?>
--FILE--
<?php
$name = bin2hex(random_bytes(8));
$pshm1 = new PSHM($name, PSHM_CREAT | PSHM_RDWR, 0);
var_dump( $pshm1->write("hello") );
var_dump( $pshm1->read() );
try {

    $pshm2 = new PSHM($name, PSHM_RDONLY);

    }
    catch (Exception $e)
    {
        var_dump($e->getMessage());
    }
var_dump( $pshm1->write("goodbye") );
var_dump( $pshm1->read() );
var_dump($pshm1->unlink());
?>
--EXPECT--
bool(true)
string(5) "hello"
string(19) "Bad file descriptor"
bool(true)
string(7) "goodbye"
bool(true)
