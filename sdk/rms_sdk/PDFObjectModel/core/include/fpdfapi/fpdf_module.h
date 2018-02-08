#ifndef _FPDF_MODULE_
#define _FPDF_MODULE_
#ifndef _FXCRT_EXTENSION_
#include "../fxcrt/fx_ext.h"
#endif
class CPDF_ModuleMgr;
class CPDF_SecurityHandler;
class CCodec_ModuleMgr;
class ICodec_FlateModule;
typedef void (*FPDF_SecurityHandlerCallback)(FX_LPCSTR name);
class CPDF_ModuleMgr : public CFX_Object
{
public:

    static void	Create();

    static CPDF_ModuleMgr*	Get();

    static void	Destroy();



    void		SetCodecModule(CCodec_ModuleMgr* pModule)
    {
        m_pCodecModule = pModule;
    }
    CCodec_ModuleMgr*		GetCodecModule()
    {
        return m_pCodecModule;
    }
    ICodec_FlateModule*		GetFlateModule();

    void					SetSecurityHandlerCallback(FPDF_SecurityHandlerCallback callback)
    {
        m_pSecurityHandlerCallback = callback;
    }

    FPDF_SecurityHandlerCallback GetSecurityHandlerCallback()
    {
        return m_pSecurityHandlerCallback;
    }
    void					RegisterSecurityHandler(FX_LPCSTR name, CPDF_SecurityHandler * (*CreateHandler)(void* param), void* param);

    CPDF_SecurityHandler*	CreateSecurityHandler(FX_LPCSTR name);

    void					SetPrivateData(FX_LPVOID module_id, FX_LPVOID pData, PD_CALLBACK_FREEDATA callback);

    FX_LPVOID				GetPrivateData(FX_LPVOID module_id);

    int						m_FileBufSize;
protected:

    CPDF_ModuleMgr();

    ~CPDF_ModuleMgr();
    void					Initialize();

    void					InitModules();



    CCodec_ModuleMgr*		m_pCodecModule;


    void (*m_pSecurityHandlerCallback)(FX_LPCSTR name);

    CFX_MapByteStringToPtr	m_SecurityHandlerMap;

    CFX_PrivateData			m_privateData;
};
#endif
