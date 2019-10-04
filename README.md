# About

Simplx is a C++ development framework for building reliable cache-friendly distributed and concurrent multicore software.

It enables low and predictable latency, scalability, and high throughput.

It is used at the Paris stock exchange by Euronext's market exchange platform, called "Optiq", and has been successfully deployed since November 2016.

Simplx was developped by [Tredzone SAS 2013-2019] 

[Scalewatch SAS](http://www.scalewatch.com) founded in 2019, continues to maintain, enhance and support Simplx.

Scalewatch offers support contracts and enterprise tools for monitoring, profiling, debugging, server clustering under commercial licenses.

## Requirements

This code has been built and unit-tested on Linux with:

- Linux kernel 2.6+
- g++ versions 4.9.4, 5.5, 6.4, 7.3 and 8.2
- clang++ 3.9.1 and 4.0 with the libstdc++ runtime

It requires either C++ 11, 14 or 17 and the pthreads library. Support for Windows/VisualC++ will be announced soon.


## License

Simplx is open-sourced under the Apache 2.0 license, please see the [accompanying License](./LICENSE).  


## Getting Started

About a dozen tutorials are included here, please see the [Tutorials README](./tutorial/README.md).

To build the first tutorial, open a terminal at the root of the repository and type:

```
cd tutorials/01_hello_actor
mkdir build && cd build
cmake ..
make -j8
```


## Unit Tests

To build and run the unit tests, which depend on the Google Test submodule, open a terminal at the root of the repository and type:

```
git submodule update --init thirdparty/googletest
cd test/engine
mkdir tbuild && cd tbuild
cmake ..
make -j8
```

to then run the unit tests type:

```
make test
```

## Docker

There's a Bash that'll compile the tutorials and run the unit tests under all above-mentionned versions of gcc and clang under Docker:

```
./test/docker_test.sh
```

If a compilation or unit test failure occurs, the script will stop and report the error.

The 1st run takes a while to download the Docker images, as per usual with Docker.
 

## Documentation

To generate the documentation, open a terminal at the repository root and type

```
doxygen doc/Doxyfile
```

## Docker

To run the unit tests under Docker:

```
./test/docker_test.sh -DENTERPRISE=1
```


