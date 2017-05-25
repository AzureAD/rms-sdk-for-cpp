#ifndef CLCCACHERESULT_H
#define CLCCACHERESULT_H

class CLCCacheResult
{
public:
    CLCCacheResult(const std::string& clc, bool cacheMissed) : CLC{clc}, CacheMissed{cacheMissed} { }
    const std::string& CLC;
    bool CacheMissed;
};

#endif // CLCCACHERESULT_H
