
# this script shows how to setup sentry-native sdk, example test application
# with all debug info needed for sentry.io to process it

# start docker with:
# $ docker run --rm --name sentry_container -itd debian:11
# $ docker attach sentry_container
# then run all the following commands inside the container
# P.S.: we use debian:11, because it has easily available all
# debug symbols that can be uploaded to sentry.io

# add repositories for downloading debug symbols
echo 'deb http://deb.debian.org/debian-debug/ bullseye-debug main' >> /etc/apt/sources.list
echo 'deb http://deb.debian.org/debian-debug/ bullseye-proposed-updates-debug main' >> /etc/apt/sources.list

apt update
# install build tools, gcc9 and libcurl which is a dependency of sentry-native sdk
# debian-goodies is needed for find-dbgsym-packages
apt install gcc-9 g++-9 git build-essential cmake make curl libcurl4-openssl-dev gdb debian-goodies

# install sentry-cli
curl -sL https://sentry.io/get-cli/ | bash
# login with sentry-cli, you will need to provide your auth token for sentry.io
sentry-cli login

cd /opt

# clone and build sentry-native sdk
git clone https://github.com/getsentry/sentry-native.git
cd sentry-native
git checkout 0.4.12
git submodule update --init --recursive

# build sentry-native with gcc9, as that's what is supported
CC=gcc-9 CXX=g++-9 cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build
# install libsentry locally, so we do not have to specify prefix path when building example application
cmake --install build --prefix /usr/local --config RelWithDebInfo

cd ..

# clone and build example application, also with gcc9
git clone https://github.com/jablotron-alarms/sentryapp.git
cd sentryapp
CC=gcc-9 CXX=g++-9 cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build

# upload binary executable with debug symbols, including sources,
# we do not care about spliting binary and  debug symbols for this test
sentry-cli upload-dif --org jablotron-alarms --project sentryapp --include-sources --wait build/sentryap

# upload libsentry library and debug symbols,
# we do not care about splitting library and debug symbols for this test
sentry-cli upload-dif --org jablotron-alarms --project sentryapp --wait /usr/local/lib/libsentry.so

# show all debug packages that need to be installed on system in order
# to debug test application with gdb
find-dbgsym-packages --all build/sentryapp

# you should get output similar to this:

# dpkg-query: no path found matching pattern /usr/local/lib/libsentry.so
# W: Cannot find debug package for /usr/local/lib/libsentry.so (74180cb3fe33119ab2f8ba4ff32048877a0fdcbd)
# dpkg-query: no path found matching pattern *build/sentryapp*
# W: Cannot find debug package for build/sentryapp (cff09138c6ff185809e4bc7c1e24d50b1f43a561)
# libbrotli1-dbgsym libc6-dbg libcom-err2-dbgsym libcurl4-dbgsym libffi7-dbgsym libgcc-s1-dbgsym libgcrypt20-dbgsym libgmp10-dbgsym libgnutls30-dbgsym libgpg-error0-dbgsym libhogweed6-dbgsym libidn2-0-dbgsym libkeyutils1-dbgsym libkrb5-dbg libldap-2.4-2-dbgsym libnettle8-dbgsym libnghttp2-14-dbgsym libp11-kit0-dbgsym libpsl5-dbgsym librtmp1-dbgsym libsasl2-2-dbgsym libssh2-1-dbgsym libssl1.1-dbgsym libstdc++6-dbgsym libtasn1-6-dbgsym libunistring2-dbgsym zlib1g-dbgsym

# first two warnings show that libsentry.so and sentryapp
# are not available in debian's repositories which is expected,
# we already uploaded both with sentry-cli
# the last line shows all debug packages with debug symbols
# needed to debug test application, you can install them with apt
# or with following command:

# install all dbg and dbgsym packages
find-dbgsym-packages --install build/sentryapp

# upload all debug symbols from system to sentry.io,
# providing 'symtab' (symbols tables) and 'debug' information needed for all libraries
find /usr/lib/debug/ -name "*.debug" | xargs sentry-cli upload-dif --org jablotron-alarms --project sentryapp --wait

# upload all libraries, providing 'unwind' information needed for all libraries
# ldd shows all dynamically linked libraries, grep extracts paths from this output and sentry-cli uploads these files
ldd build/sentryapp | grep -Po "=> \K.*(?= \()" | xargs sentry-cli upload-dif --org jablotron-alarms --project sentryapp --wait

# now you can run build/sentryapp, crash it, send logs
build/sentryapp --crash
build/sentryapp --log hello

# issues show up in sentry.io ui correctly
# but the crash does not display crash source location
