#ifndef DATACOLLECTION_H
#define DATACOLLECTION_H

#include "ModernAPIExport.h"
#include "IDataCollection.h"
#include <vector>
#include <memory>

namespace rmscore{
namespace modernapi{

class DataCollection;

class DataCollection: public IDataCollection
{

    friend class IDataCollection;

public:

  /**
   * @brief Checks if this data collection is verified (and thus read-only)
   * @return
   */
  virtual bool IsVerified() override;

protected:
    DataCollection();

private:

  DataCollection(std::string& json);
  bool verified;

};

}
}


#endif // DATACOLLECTION_H
