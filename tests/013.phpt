--TEST--
Fork Test
--SKIPIF--
<?php
if (!extension_loaded('pshm')) {
	echo 'skip';
}
if (!extension_loaded('pcntl')) {
	echo 'skip';
}

?>
--FILE--
<?php
$name = bin2hex(random_bytes(8));
$pshm = new PSHM($name);

    $pid = pcntl_fork();
    if ($pid == -1) {
     echo 'skip';
} else if ($pid) {

    pcntl_wait($status);

    var_dump( $pshm->read() ) ;

    var_dump( $pshm->unlink() ) ;



} else {

    var_dump( $pshm->write("Hi Mum!") );
    exit(0);

}



?>
--EXPECT--
bool(true)
string(7) "Hi Mum!"
bool(true)
