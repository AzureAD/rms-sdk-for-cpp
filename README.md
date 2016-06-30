# RMS SDK for portable C++

[![Build Status](https://travis-ci.org/AzureAD/rms-sdk-for-cpp.svg?branch=master)](https://travis-ci.org/AzureAD/rms-sdk-for-cpp)

- [How to Build](./docs/how_to_build_it.md)
- [How to Use](./docs/how_to_use_it.md)

Alternatively, run the script in `./scripts/build_and_test.sh`.

## Documentation

The complete [RMS SDK 4.1](https://msdn.microsoft.com/en-us/Library/dn758244%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396), for Linux as well as iOS/OS X, Android, Windows Phone and Windows R/T operating systems is available on MSDN.

See the [API reference](http://azuread.github.io/rms-sdk-for-cpp/index.html), created with Doxygen, here. Or, you can create your own build of the API reference by following these steps: 

1. Install [Doxygen], for example with `sudo brew install doxygen` (OSX) or `sudo apt-get install doxygen` (Ubuntu).
2. Run `doxygen` from the root of the repo.
3. Open `./docs/html/index.html` in your browser.

[Doxygen]: http://www.doxygen.org

Documentation of the source is ongoing.

## Platform support
* Ubuntu 14.04 or above
* Red Hat 7.1 or above
* CentOS 7.1 or above
* OpenSuSe 13.2 or above

## Requirements
* gcc-c++ 4.8 or above
* openssl 1.0 or above
* libsecret 1.0 or above
* Qt 5.2.1 or above 

## Troubleshooting

Information about requests is logged to `rms_log_*`, `rmsauth_log_*`, and `rmscrypto_log_*` in the folder where the executable using these libraries is called from. Request and response headers and some other info is not logged by default to avoid leaking sensitive information. To enable these hidden logs, set an environment variable `RMS_HIDDEN_LOG` to `ON`. For example:

```
> RMS_HIDDEN_LOG=ON ./rms_sample
```

## Security Reporting

If you find a security issue with our libraries or services please report it to [secure@microsoft.com](mailto:secure@microsoft.com) with as much detail as possible. Your submission may be eligible for a bounty through the [Microsoft Bounty](http://aka.ms/bugbounty) program. Please do not post security issues to GitHub Issues or any other public site. We will contact you shortly upon receiving the information. We encourage you to get notifications of when security incidents occur by visiting [this page](https://technet.microsoft.com/en-us/security/dd252948) and subscribing to Security Advisory Alerts.

## Contributing

Before we can accept your pull request, you'll need to electronically complete Microsoft Open Tech's [Contributor License Agreement](https://cla.msopentech.com/). If you've done this for other Microsoft Open Tech projects, then you're already covered.

## We Value and Adhere to the Microsoft Open Source Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

## License

Copyright (c) Microsoft Open Technologies, Inc. All rights reserved. Licensed under the MIT license.
