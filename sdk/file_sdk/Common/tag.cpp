#include "tag.h"
#include <regex>
#include <string_utils.h>

using std::pair;
using std::string;
using std::vector;

namespace {
static const string kMetaDataPrefix = "MSIP_Label";
static const string kExtendedPrefix = "Extended";
}  // namespace

namespace mip {

string kMethodArray[] = {"None", "Manual", "Automatic"};

// static
const string& Tag::GetMetaDataPrefix() {
  return kMetaDataPrefix;
}

// static
string Tag::GenerateLabelKey(const string& id, const string& attribute) {
  return kMetaDataPrefix + "_" + id + "_" + attribute;
}

// static
string Tag::GenerateExtendedKey(const string& id, const string& vendor, const string& key) {
  return kMetaDataPrefix + "_" + id + "_" + kExtendedPrefix + "_" + vendor + "_" + key;
}

// static
string Tag::GetBackwardCompatibleKey(
    const vector<pair<string, string>>& properties,
    const string& id,
    const string& key,
    const vector<string>& backwardCompatibleKeys) {
  string keyLabel = GenerateLabelKey(id, key);
  string value = GetDataForKey(properties, keyLabel);

  if (!value.empty())
    return value;

  for (size_t i = 0; i < backwardCompatibleKeys.size(); i++) {
    keyLabel = GenerateLabelKey(id, backwardCompatibleKeys[i]);
    value = GetDataForKey(properties, keyLabel);
    if (!value.empty())
      return value;
  }

  return "";
}

// static
string Tag::GetDataForKey(const vector<pair<string, string>>& properties, const string& key) {
  for (size_t i = 0; i < properties.size(); i++) {
    const pair<string, string>& data = properties[i];
    if (EqualsIgnoreCase(data.first, key)) {
      return data.second;
    }
  }

  return "";
}

// static
vector<string> Tag::GetAllIds(const vector<pair<string, string>>& properties) {
  size_t length = kMetaDataPrefix.size() + 1;
  size_t guidLength = 36;

  vector<string> labelsIds;
  for (size_t i = 0; i < properties.size(); i++) {
    pair<string, string> data = properties[i];
    string id = data.first;
    string guid = id.substr(length, guidLength);
    if (std::regex_match(guid, std::regex("^" + kMetaDataPrefix + "_([a-z0-9]{8}-([a-z0-9]{4}-){3}[a-z0-9]{12})$", std::regex_constants::icase))) {
      labelsIds.push_back(guid);
    }
  }

  labelsIds.erase(unique(labelsIds.begin(), labelsIds.end()), labelsIds.end());
  return labelsIds;
}

Tag::Tag(
    const string& labelId,
    const string& labelName,
    const string& labelParentId,
    const string& owner,
    bool enabled,
    const string& setTime,
    const Method& method,
    const string& siteId,
    const vector<ExtendedProperty>& extendedProperties)
  : mLabelId(labelId),
    mLabelName(labelName),
    mLabelParentId(labelParentId),
    mOwner(owner),
    mEnabled(enabled),
    mSiteId(siteId),
    mMethod(method),
    mSetTime(setTime) {
  for (size_t i = 0; i < extendedProperties.size(); i++) {
    if (extendedProperties[i].key.length() > 255)
      throw std::length_error("Extended property key cannot be greater than 255");

    if (extendedProperties[i].value.length() > 255)
      throw std::length_error("Extended property value cannot be greater than 255");

    mExtendedProperties.push_back(extendedProperties[i]);
  }
}

Tag::Tag(const Tag& tag)
  :
    mLabelId(tag.mLabelId),
    mLabelName(tag.mLabelName),
    mLabelParentId(tag.mLabelParentId),
    mOwner(tag.mOwner),
    mEnabled(tag.mEnabled),
    mSiteId(tag.mSiteId),
    mMethod(tag.mMethod),
    mSetTime(tag.mSetTime)
{
  auto properties = tag.mExtendedProperties;
  for (size_t i = 0; i < properties.size(); i++) {
    mExtendedProperties.push_back(properties[i]);
  }
}

Tag&Tag::operator=(const Tag& tag)
{
  if (this == &tag)
    return *this;

  mLabelId = tag.mLabelId;
  mLabelName = tag.mLabelName;
  mLabelParentId = tag.mLabelParentId;
  mEnabled = tag.mEnabled;
  mMethod = tag.mMethod;
  mOwner = tag.mOwner;
  mSetTime = tag.mSetTime;
  mSiteId = tag.mSiteId;

  mExtendedProperties.clear();
  auto properties = tag.GetExtendedProperties();
  for (size_t i = 0; i < properties.size(); i++) {
    mExtendedProperties.push_back(properties[i]);
  }

  return *this;
}

bool Tag::operator== (const Tag& other) const {
  if(mExtendedProperties != other.mExtendedProperties){
    return false;
  }

  return
      mLabelId == other.mLabelId &&
      mLabelName == other.mLabelName &&
      mLabelParentId == other.mLabelParentId &&
      mEnabled == other.mEnabled &&
      mMethod == other.mMethod &&
      mOwner == other.mOwner &&
      mSetTime == other.mSetTime &&
      mSiteId == other.mSiteId;
}

vector<pair<string, string>> Tag::ToProperties() const {
  vector<pair<string, string>> result;
  string id = mLabelId;
  result.push_back(pair<string, string>(
                     GenerateLabelKey(id, "Enabled"), mEnabled ? "true" : "false"));
  result.push_back(pair<string, string>(GenerateLabelKey(id, "SiteId"), mSiteId));
  result.push_back(pair<string, string>(GenerateLabelKey(id, "Owner"), mOwner));
  result.push_back(pair<string, string>(GenerateLabelKey(id, "SetDate"), mSetTime));
  result.push_back(pair<string, string>(GenerateLabelKey(id, "Name"), mLabelName));

  Method method = mMethod;
  string methodAsString = kMethodArray[static_cast<size_t>(method)];

  result.push_back(pair<string, string>(GenerateExtendedKey(id, "MSFT", "Method"), methodAsString));
  if (!mLabelParentId.empty())
    result.push_back(pair<string, string>(GenerateLabelKey(id, "Parent"), mLabelParentId));

  for (size_t i = 0; i < mExtendedProperties.size(); i++) {
    ExtendedProperty extendedProperty = mExtendedProperties[i];
    string key = GenerateExtendedKey(id, extendedProperty.vendor, extendedProperty.key);
    result.push_back(pair<string, string>(key, extendedProperty.value));
  }

  return result;
}

vector<pair<string, string>> Tag::ToProperties(const vector<Tag>& tags) {
  vector<pair<string, string>> result;

  for (size_t i = 0; i < tags.size(); i++) {
    auto properties = tags[i].ToProperties();
    result.insert(result.end(), properties.begin(), properties.end());
  }

  return result;
}

vector<Tag> Tag::FromProperties(const vector<pair<string, string>>& properties) {
  vector<Tag> result;
  vector<string> labelsIds = GetAllIds(properties);
  for (size_t i = 0; i < labelsIds.size(); i++) {
    string labelId = labelsIds[i];
    bool isEnabled = false;
    TryParseBool(GetDataForKey(properties, GenerateLabelKey(labelId, "Enabled")), isEnabled);

    vector<string> ids = {"Ref"};

    string siteId = GetBackwardCompatibleKey(properties, labelId, "SiteId", ids);

    ids = { "AssignedBy", "SetBy" };
    string owner = GetBackwardCompatibleKey(properties, labelId, "Owner", ids);

    ids = { "DateCreated" };
    string setDate = GetBackwardCompatibleKey(properties, labelId, "SetDate", ids);

    string name = GetDataForKey(properties, GenerateLabelKey(labelId, "Name"));
    string parentId = GetDataForKey(properties, GenerateLabelKey(labelId, "Parent"));
    string methodKey = GenerateExtendedKey(labelId, "MSFT", "Method");
    string methodAsString = GetDataForKey(properties, methodKey);
    Method method = Method::NONE;
    if (methodAsString.compare("Manual") == 0)
      method = Method::MANUAL;
    else if (methodAsString.compare("Automatic") == 0)
      method = Method::AUTOMATIC;

    vector<ExtendedProperty> extendedProperties;
    string extendedPropertyPrefix = GenerateLabelKey(labelId, kExtendedPrefix);
    for (size_t j = 0; j < properties.size(); j++) {
      pair<string, string> data = properties[j];
      string id = data.first;
      string replace = "";
      if (id.compare(methodKey) != 0 && !id.compare(0, extendedPropertyPrefix.size(), extendedPropertyPrefix)) {
        string vendorAndKey = regex_replace(id, std::regex(extendedPropertyPrefix), replace);
        size_t index = vendorAndKey.find_first_of("_");
        if (index == string::npos)
          continue;

        string vendor = vendorAndKey.substr(0, index);
        string key = vendorAndKey.substr(index + 1);
        ExtendedProperty extendedProperty;
        extendedProperty.vendor = vendor;
        extendedProperty.key = key;
        extendedProperty.value = data.second;
        extendedProperties.push_back(extendedProperty);
      }
    }

    Tag tag(labelId, name, parentId, owner, isEnabled, setDate, method, siteId, extendedProperties);
    result.push_back(tag);
  }

  return result;
}

} // namespace mip
