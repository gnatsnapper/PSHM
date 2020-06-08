--TEST--
PSHM_EXCL Flag Test
--SKIPIF--
<?php
if (!extension_loaded('pshm')) {
	echo 'skip';
}
?>
--FILE--
<?php
$name = bin2hex(random_bytes(8));
$pshm1 = new PSHM($name, PSHM_CREAT | PSHM_RDWR);
try {

    $pshm2 = new PSHM($name,PSHM_EXCL | PSHM_CREAT | PSHM_RDWR);

    }
    catch (Exception $e)
    {
        var_dump( $e->getMessage() );
    }
var_dump( $pshm1->write("hello") );
var_dump( $pshm1->read() );
var_dump($pshm1->unlink());
?>
--EXPECT--
string(19) "Bad file descriptor"
bool(true)
string(5) "hello"
bool(true)
