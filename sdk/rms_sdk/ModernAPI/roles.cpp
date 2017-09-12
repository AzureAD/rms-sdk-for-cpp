#include "roles.h"

namespace rmscore {
namespace modernapi {
const std::string Roles::Viewer()
{
  return "VIEWER";
}

/// <summary>
/// User will be able to view and edit the document. They cannot copy or print it.
/// </summary>
const std::string Roles::Reviewer()
{
  return "REVIEWER";
}

/// <summary>
/// User will be able to view, edit, copy, and print the document.
/// </summary>
const std::string Roles::Author()
{
  return "AUTHOR";
}

/// <summary>
/// User will have all permissions.
/// </summary>
const std::string Roles::CoOwner()
{
  return "COOWNER";
}
}
}
