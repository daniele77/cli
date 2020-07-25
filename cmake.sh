#!/bin/bash

BOOST_MINOR=70

if [ $# -eq 1 ]
then
  BOOST_MINOR=$1
fi

BOOST_PATH=/home/daniele/lib/boost_1_${BOOST_MINOR}_0/install/gcc-8.3.0
WARNINGS_BASICS="-Wall -Wextra -Werror"
WARNINGS_ALL="\
-pedantic \
-Wall \
-Wextra \
-Wcast-align \
-Wcast-qual \
-Wctor-dtor-privacy \
-Wdisabled-optimization \
-Wformat=2 \
-Winit-self \
-Wlogical-op \
-Wmissing-declarations \
-Wmissing-include-dirs \
-Wnoexcept \
-Wold-style-cast \
-Woverloaded-virtual \
-Wredundant-decls \
-Wshadow \
-Wsign-conversion \
-Wsign-promo \
-Wstrict-null-sentinel \
-Wstrict-overflow=5 \
-Wundef \
-Werror \
"

#WARNINGS=${WARNINGS_BASICS}
WARNINGS=${WARNINGS_ALL}

FLAGS="${WARNINGS} -O3"

# export CXX=/usr/bin/clang++

rm -rf build
mkdir build
cd build
cmake .. -DCLI_BuildTests=ON -DCLI_BuildExamples=ON -DBOOST_INCLUDEDIR=${BOOST_PATH}/include/ -DBOOST_LIBRARYDIR=${BOOST_PATH}/lib/ -DCMAKE_CXX_FLAGS="${FLAGS}"
make all
cd ..

build/test/test_suite --report_level=short


