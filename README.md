# RMS SDK for portable C++

[![Build Status](https://travis-ci.org/AzureAD/rms-sdk-for-cpp.svg?branch=master)](https://travis-ci.org/AzureAD/rms-sdk-for-cpp)

- [How to Build](./docs/how_to_build_it.md)
- [How to Use](./docs/how_to_use_it.md)

Alternatively, run the script in `./scripts/build_and_test.sh`.

## Documentation

API reference docs can be created with Doxygen. Documentation of the source is ongoing.
To generate docs:

1. Install [Doxygen], for example with `sudo brew install doxygen` (OSX) or `sudo apt-get install doxygen` (Ubuntu).
2. Run `doxygen` from the root of the repo.
3. Open `./docs/html/index.html` in your browser.

[Doxygen]: http://www.doxygen.org

## Troubleshooting

Information about requests is logged to `rms_log_*`, `rmsauth_log_*`, and `rmscrypto_log_*` in the folder where the executable using these libraries is called from. Request and response headers and some other info is not logged by default to avoid leaking sensitive information. To enable these hidden logs, set an environment variable `RMS_HIDDEN_LOG` to `ON`. For example:

```
> RMS_HIDDEN_LOG=ON ./rms_sample
```

## Contributing

Before we can accept your pull request, you'll need to electronically complete Microsoft Open Tech's [Contributor License Agreement](https://cla.msopentech.com/). If you've done this for other Microsoft Open Tech projects, then you're already covered.

## License

Copyright (c) Microsoft Open Technologies, Inc. All rights reserved. Licensed under the MIT license.
