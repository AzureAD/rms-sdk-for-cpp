/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#include <libsecret/secret.h>
#include "KeyStoragePosix.h"
#include "../../CryptoAPI/RMSCryptoExceptions.h"

using namespace std;
namespace rmscrypto {
namespace platform {
namespace keystorage {
const SecretSchema* str_key_schema(void) {
  static const SecretSchema the_schema = {
    "microsoft.Password", SECRET_SCHEMA_NONE,
    {
      {  "string",        SECRET_SCHEMA_ATTRIBUTE_STRING },
      {  "NULL",          static_cast<SecretSchemaAttributeType>(0) },
    },
    0,                               0,0, 0, 0, 0, 0, 0,
  };

  return &the_schema;
}

#define KEY_SCHEMA  str_key_schema()

void ProcessError(GError *error) {
  string msg(reinterpret_cast<const char *>(error->message));
  int    code = error->code;

  g_error_free(error);
  throw exceptions::RMSCryptoIOKeyException(msg.c_str(), code);
}

void KeyStoragePosix::RemoveKey(const string& csKeyWrapper) {
  GError *error = NULL;

  /*
   * The variable argument list is the attributes used to later
   * lookup the password. These attributes must conform to the schema.
   */
  gboolean removed = secret_password_clear_sync(KEY_SCHEMA, NULL, &error,
                                                "string", csKeyWrapper.c_str(),
                                                NULL);

  if (!removed && (error != NULL)) {
    ProcessError(error);
  }
}

void KeyStoragePosix::StoreKey(const string& csKeyWrapper, const string& csKey) {
  GError *error = NULL;

  /*
   * The variable argument list is the attributes used to later
   * lookup the password. These attributes must conform to the schema.
   */
  gboolean stored = secret_password_store_sync(KEY_SCHEMA,
                                               SECRET_COLLECTION_DEFAULT,
                                               "Microsoft RMSCryptoSDK",
                                               csKey.c_str(),
                                               NULL,
                                               &error,
                                               "string",
                                               csKeyWrapper.c_str(),
                                               NULL);

  if (!stored && (error != NULL)) {
    ProcessError(error);
  }
}

shared_ptr<string>KeyStoragePosix::LookupKey(const string& csKeyWrapper) {
  GError *error = NULL;

  shared_ptr<string> res = nullptr;

  /* The attributes used to lookup the password should conform to the schema. */
  gchar *password = secret_password_lookup_sync(KEY_SCHEMA, NULL, &error,
                                                "string", csKeyWrapper.c_str(),
                                                NULL);

  if (error != NULL) {
    ProcessError(error);
  } else if (password != NULL) {
    res = make_shared<string>(reinterpret_cast<const char *>(password));
    secret_password_free(password);
  }
  return res;
}

std::shared_ptr<IKeyStorage>IKeyStorage::Create() {
  return std::shared_ptr<KeyStoragePosix>(new KeyStoragePosix);
}
} // namespace keystorage
} // namespace platform
} // namespace rmscrypto
