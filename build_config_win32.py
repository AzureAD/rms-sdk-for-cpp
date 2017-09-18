#!python
import os
import os.path
import sys

include_search_path = [
    '#sdk',
    '#third_party',
    '#third_party/include',
    '#googletest',
    '#googletest/include',
    '#googlemock',
    '#googlemock/include',
]

libxml2flavor = "libxml2-2.9.3-win32-x86_64"

libxml2headerpath = "#src/external/libxml2/" + libxml2flavor + "/include/libxml2"
