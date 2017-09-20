#!python
import os
import os.path
import sys

include_search_path = [
    '#sdk',
    '#googletest',
    '#googletest/include',
    '#googlemock',
    '#googlemock/include',
]

osx_version = '10.10'

# -Werror
osx_release_ccflags = '''
    -fmessage-length=0 -fdiagnostics-show-note-include-stack -fmacro-backtrace-limit=0 -fobjc-arc
    -Wno-trigraphs -fpascal-strings -Os -fno-common -Wno-missing-field-initializers
    -Wno-missing-prototypes -Wunreachable-code -Wno-implicit-atomic-properties
    -Wno-arc-repeated-use-of-weak -Wduplicate-method-match -Wno-missing-braces -Wparentheses
    -Wswitch -Wunused-function -Wno-unused-label -Wno-unused-parameter -Wno-unused-variable
    -Wno-unused-value -Wempty-body -Wuninitialized -Wno-unknown-pragmas -Wno-shadow
    -Wno-four-char-constants -Wno-conversion -Wno-constant-conversion -Wno-int-conversion
    -Wbool-conversion -Wno-enum-conversion -Wno-shorten-64-to-32 -Wpointer-sign -Wno-newline-eof
    -Wno-selector -Wno-strict-selector-match -Wundeclared-selector -Wno-deprecated-implementations
    -DRMS_ERROR_DLG=1 -DNS_BLOCK_ASSERTIONS=1 -DOBJC_OLD_DISPATCH_PROTOTYPES=0 -fasm-blocks
    -fstrict-aliasing -Wprotocol -Wdeprecated-declarations -Wno-sign-conversion -Winfinite-recursion
'''

libxml2flavor = ''

libxml2headerpath = "#src/external/libxml2/" + libxml2flavor + "/include/libxml2"
