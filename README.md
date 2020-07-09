[![Build Status](https://travis-ci.org/gnatsnapper/PSHM.svg?branch=master)](https://travis-ci.org/gnatsnapper/PSHM)

PSHM
======

PSHM is a php extension for interacting with posix shared memory.  Posix shared memory is an updated alternative for sysv shared memory, i.e. low-latency kernel-based interprocess communication. Tested on PHP7.4 on linux and bsd.

## Install

```
$ phpize
$ ./configure
$ make && make install
```

## Usage

### Create Shared Memory

```php
$pshm = new PSHM("/myshm");
```
PSHM with only a queue name will attempt to create a new or join an existing shared memory with read or read and write permissions.  As per the standard, queue names must begin with a single forward slash (/) and be less than 255 characters.

PSHM's uses O flags formatting:
* ```PSHM_RDONLY``` - (```O_RDONLY```) read only
* ```PSHM_RDWR``` - (```O_RDWR```) read and write
* ```PSHM_CREAT``` - (```O_CREAT```) create
* ```PSHM_EXCL``` - (```O_EXCL```) exclusive (do not create if file exists)
* ```PSHM_TRUNC``` - (```O_TRUNC```) truncate memory on creation

Shared memory permissions can be set in umask format and shared memory size (bytes) can also be set, if the users permissions allow:

```php
$pshm = new PSHM("/mysharedmemory", PSHM_CREAT | PSHM_RDWR, 0600, 8192);
```

### Write

```php
$pshm->write("hello");
```

### Read

```php
$pmq->read();
```

### Delete Shared Memory

```php
$pshm->unlink();
```
Shared memory not deleted will persist until computer is restarted, so always delete if not needed by another process.

## Troubleshooting

### Bad file descriptor
This occurs where attempting to send to a queue with only read access, or vice versa.
