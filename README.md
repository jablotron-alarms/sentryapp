Official sentry example taken from https://github.com/getsentry/sentry-native

## Building

Make sure you have libcurl installed, sentry-native sdk depends on it:
`apt-get install libcurl4-openssl-dev`

In order to build this you need to download sentry-native sdk and build it:

```
# clone sentry-native sdk
git clone --recursive https://github.com/getsentry/sentry-native.git
cd sentry-native
# checkout latest released version
git checkout 0.4.8
# configure the sdk, specifying build directory
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
# build the sdk
cmake --build build --parallel
# install the sdk to ./install prefix
cmake --install build --prefix install --config RelWithDebInfo
```

Then you need to download this repository and build it:

```
# clone sentryapp example
git clone https://github.com/jablotron-alarms/sentryapp.git
cd sentryapp
# configure sentryapp, you need to specify path to install prefix specified above when installing sentry-native sdk
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_PREFIX_PATH=/path/to/sentry-native/install/
cmake --build build --parallel
```

## Uploading debug info files

If you want to upload debug info files including sources, you need to split the debug info and use sentry-cli app to upload them to sentry servers.

1. Install sentry-cli as shown here https://docs.sentry.io/product/cli/installation  
`curl -sL https://sentry.io/get-cli/ | bash`

2. Then you need to authenticate yourself as per official guide [here](https://docs.sentry.io/product/cli/configuration/).

3. Split the debug info following the official instructions [here](https://docs.sentry.io/platforms/native/data-management/debug-files/file-formats/#executable-and-linkable-format-elf).
    - Or you can use helper script from this repo:
`./debug_info.sh build/sentryapp`

4. Bundle the sources:  
`sentry-cli difutil bundle-sources build/sentryapp build/sentryapp.debug`

5. Upload all debug info files to sentry server:
`sentry-cli upload-dif --org jablotron-alarms --project testapp --wait build/sentryapp build/sentryapp.debug build/sentryapp.src.zip`

## Capturing events

Run any command that sentryapp example provides, such as:
- `build/sentryapp crash`
- `build/sentryapp capture-event`