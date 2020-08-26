# FDBus Sample

In [Quick Guide](./FDBus_quick_guide.md), we learn how to build fdbus in Ubuntu and Window. So let's write sample code to practice.

## Prepared fdbus and protobuf

copy or move build into install folder.

```shell
cd ~/workspace
mv ~/workspace/buildCentral/output/stage/host install
```

## Sample project with CMake 

Next we will create sample project with cmake in Ubuntu host.

### Job Sample

* Create project with cmake

```shell
cd ~/workspace
mkdir fdbus-sample && cd fdbus-sample
gedit CMakeLists.txt
```

```cmake
PROJECT(fdbus-sample)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
ADD_DEFINITIONS(-std=c++11)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
INCLUDE_DIRECTORIES("~/workspace/install/include" "~/workspace/install/usr/include")
LINK_DIRECTORIES("~/workspace/install/lib" "~/workspace/install/usr/lib")
ADD_EXECUTABLE(job_test job_test.cpp)
TARGET_LINK_LIBRARIES(job_test common_base)
```

* Create [job_test.cpp](../code/fdbus-sample/job_test.cpp) sample file.

* Compile and run

```shell
cd ~/workspace/fdbus-sample
mkdir build && cd build
cmake ../
make
./job_test
```

* Get run result on command line:

```
hello, world!
hello, world!
hello, world!
hello, world!
hello, world!
hello, world!
```

### Server&Client Sample

* Write cpp files:
  * message file: [CFdbIfPerson.h](../code/fdbus-sample/CFdbIfPerson.h)
  * server cpp file: [fdb_test_server.cpp](../code/fdbus-sample/fdb_test_server.cpp)
  * client cpp file: [fdb_test_client.cpp](../code/fdbus-sample/fdb_test_client.cpp)

* Update `CMakeLists.txt`

```cmake
PROJECT(fdbus-sample)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
ADD_DEFINITIONS(-std=c++11)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
INCLUDE_DIRECTORIES("~/workspace/install/include" "~/workspace/install/usr/include")
LINK_DIRECTORIES("~/workspace/install/lib" "~/workspace/install/usr/lib")
ADD_EXECUTABLE(job_test job_test.cpp)
TARGET_LINK_LIBRARIES(job_test common_base)
ADD_EXECUTABLE(fdb_test_server fdb_test_server.cpp)
TARGET_LINK_LIBRARIES(fdb_test_server common_base)
ADD_EXECUTABLE(fdb_test_client fdb_test_client.cpp)
TARGET_LINK_LIBRARIES(fdb_test_client common_base)
```

* Compile and run

```shell
cd ~/workspace/fdbus-sample/build
cmake ../
make
```

run test server in new shell terminal

```shell
cd ~/workspace/fdbus-sample/build
./fdb_test_server
```

run test client in new shell terminal

```shell
cd ~/workspace/fdbus-sample/build
./fdb_test_client
```

run name server in new shell terminal

```shell
cd ~/workspace/install/usr/bin
LD_LIBRARY_PATH=~/workspace/install/usr/lib:$LD_LIBRARY_PATH
./name_server
```