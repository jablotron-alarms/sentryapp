Official sentry example taken from https://github.com/getsentry/sentry-native

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

You can than run any command that sentryapp example provides, such as:
- `build/sentryapp crash`
- `build/sentryapp capture-event`