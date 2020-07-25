#!/bin/bash

BOOST_MINOR=70

if [ $# -eq 1 ]
then
  BOOST_MINOR=$1
fi

cd examples 
make CXXFLAGS="-isystem /home/daniele/lib/boost_1_${BOOST_MINOR}_0/install/gcc-8.3.0/include" LDFLAGS="-L/home/daniele/lib/boost_1_${BOOST_MINOR}_0/install/gcc-8.3.0/lib/"
cd ..

cd test
make BOOST_INC=/home/daniele/lib/boost_1_${BOOST_MINOR}_0/install/gcc-8.3.0/include/ BOOST_LIB=/home/daniele/lib/boost_1_${BOOST_MINOR}_0/install/gcc-8.3.0/lib/ 
cd ..

test/test_suite --report_level=short


