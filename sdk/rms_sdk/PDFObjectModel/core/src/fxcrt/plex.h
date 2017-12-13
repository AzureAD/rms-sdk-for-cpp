struct CFX_Plex {
    CFX_Plex* pNext;
    void* data()
    {
        return this + 1;
    }
    static CFX_Plex* Create(CFX_Plex*& head, FX_DWORD nMax, FX_DWORD cbElement);
    void FreeDataChain();
};
