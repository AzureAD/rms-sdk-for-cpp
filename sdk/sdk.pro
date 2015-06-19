TEMPLATE = subdirs

SUBDIRS += rmscrypto_sdk rmsauth_sdk rms_sdk

rmsauth_sdk.depends = rmscrypto_sdk
rms_sdk.depends     = rmscrypto_sdk rmsauth_sdk
