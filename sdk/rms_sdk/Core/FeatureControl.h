#ifndef FEATURECONTROL_H
#define FEATURECONTROL_H
namespace rmscore {
namespace core {

// Class is stubed for server controled feature controls.
class FeatureControl
{
public:
    static bool IsEvoEnabled() {return IS_EVO_ENABLED;}

private:
    static const bool IS_EVO_ENABLED = true;
};

} //core
} //rmscore
#endif // FEATURECONTROL_H
