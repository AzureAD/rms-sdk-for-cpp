#ifndef TAG_H_
#define TAG_H_

#include <string>
#include <vector>
#include "extended_property.h"

namespace mip {

enum class Method : unsigned int {
  NONE,
  MANUAL,
  AUTOMATIC
};

class Tag final
{
public:
  // Returns the prefix used to encode meta-data in content.
  static const std::string& GetMetaDataPrefix();
  // TODO: Make these methods private after using friend idiom for their tests.
  static std::vector<std::pair<std::string, std::string>> ToProperties(
      const std::vector<Tag>& tags);
  static std::vector<Tag> FromProperties(
      const std::vector<std::pair<std::string, std::string>>& properties);
  static std::string GenerateLabelKey(const std::string& id, const std::string& attribute);
  static std::string GenerateExtendedKey(
      const std::string& id, const std::string& vendor, const std::string& key);

  Tag(
      const std::string& labelId,
      const std::string& labelName,
      const std::string& labelParentId,
      const std::string& owner,
      bool enabled,
      const std::string& setTime,
      const std::string& applicationName,
      const Method& method = Method::NONE,
      const std::string& siteId = "Local",
      const std::vector<ExtendedProperty>& extendedProperties = std::vector<ExtendedProperty>());

  bool operator== (const Tag& other) const;
  std::vector<std::pair<std::string, std::string>> ToProperties() const;
  const std::string& GetLabelId() const { return mLabelId; }
  const std::string& GetLabelName() const { return mLabelName; }
  const std::string& GetLabelParentId() const { return mLabelParentId; }

private:
  // TODO: Add tests for these methods.
  static std::string GetBackwardCompatibleKey(
    const std::vector<std::pair<std::string, std::string>>& properties,
    const std::string& id,
    const std::string& key,
    const std::vector<std::string>& backwardCompatibleKeys);
  static std::string GetDataForKey(
    const std::vector<std::pair<std::string, std::string>>& properties, const std::string& key);
  static std::vector<std::string> GetAllIds(
    const std::vector<std::pair<std::string, std::string>>& properties);

  std::string mLabelId;
  std::string mLabelName;
  std::string mLabelParentId;
  std::string mOwner;
  bool mEnabled;
  std::string mSiteId;
  Method mMethod;
  std::string mSetTime;
  std::string mApplicationName;
  std::vector<ExtendedProperty> mExtendedProperties;
};

} // namespace mip

#endif // API_IMPL_TAG_H_
