TEMPLATE = subdirs

SUBDIRS += rmscrypto_sdk rmsauth_sdk rms_sdk \
    file_sdk

rmsauth_sdk.depends = rmscrypto_sdk
rms_sdk.depends     = rmscrypto_sdk rmsauth_sdk
file_sdk.depends    = rms_sdk
