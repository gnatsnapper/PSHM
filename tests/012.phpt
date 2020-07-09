--TEST--
Memory Test
--SKIPIF--
<?php
if (!extension_loaded('pshm')) {
	echo 'skip';
}
?>
--FILE--
<?php
$name = bin2hex(random_bytes(8));
$pshm1 = new PosixSharedMemory($name, PSHM_CREAT | PSHM_RDWR, 0600, 128);
var_dump( $pshm1->write(str_repeat("!",127) ) );
try {

    var_dump( $pshm1->write(str_repeat("*",129) ) );

    }
    catch (Exception $e)
    {
        var_dump($e->getMessage());
    }
var_dump( ( $pshm1->read() === str_repeat("!",127) ) );
var_dump($pshm1->unlink());
?>
--EXPECT--
bool(true)
string(35) "Message too large for mapped region"
bool(true)
bool(true)
