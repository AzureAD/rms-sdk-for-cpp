/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef TYPES
#define TYPES
#include<memory>
#include<string>
#include<sstream>
#include<list>
#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<mutex>
#include<future>
#include<cstdint>

namespace rmsauth {

template<typename T>
using ptr = std::shared_ptr<T>;

template<typename T>
using ptrU = std::unique_ptr<T>;

using String = std::string;

template<typename T>
using List = std::list<T>;

template<typename T>
using ArrayList = std::vector<T>;

template<typename T>
using Iterator = typename ArrayList<T>::iterator;

template<typename K, typename V>
using HashMap = std::unordered_map<K, V>;

template<typename K>
using HashSet = std::unordered_set<K>;

template<typename K, typename V>
using KeyValuePair = std::pair<K, V>;

template<typename K, typename V>
using KeyValuePairPtr = ptr<KeyValuePair<K,V>>;

using Headers = HashMap<String, String>;
using StringMap = Headers;

using ByteArray = ArrayList<char>;
using StringArray = ArrayList<String>;
using IntArray = ArrayList<int>;

using StringStream = std::stringstream;

using Mutex = std::mutex;

using Lock = std::lock_guard<Mutex>;

template<typename T>
using Future = std::future<T>;

using DateTimeOffset = int64_t;

} // namespace rmsauth {

#endif // TYPES
