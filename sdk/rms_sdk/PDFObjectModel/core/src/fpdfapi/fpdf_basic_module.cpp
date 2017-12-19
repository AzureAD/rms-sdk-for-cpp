#include "../../include/fxcodec/fx_codec.h"
#include "../../include/fpdfapi/fpdf_module.h"
static CPDF_ModuleMgr*	g_FPDFAPI_pDefaultMgr = NULL;
CPDF_ModuleMgr* CPDF_ModuleMgr::Get()
{
    return g_FPDFAPI_pDefaultMgr;
}
void CPDF_ModuleMgr::Create()
{
    g_FPDFAPI_pDefaultMgr = FX_NEW CPDF_ModuleMgr;
    g_FPDFAPI_pDefaultMgr->Initialize();
}
void CPDF_ModuleMgr::Destroy()
{
    if (g_FPDFAPI_pDefaultMgr) {
        delete g_FPDFAPI_pDefaultMgr;
    }
    g_FPDFAPI_pDefaultMgr = NULL;
}
CPDF_ModuleMgr::CPDF_ModuleMgr()
{
    m_pCodecModule = NULL;
    m_pSecurityHandlerCallback = NULL;
    m_FileBufSize = 512;
}
void CPDF_ModuleMgr::Initialize()
{
    InitModules();
    m_pSecurityHandlerCallback = NULL;
    m_FileBufSize = 512;
}
void CPDF_ModuleMgr::InitModules()
{
    m_pCodecModule = NULL;
}
CPDF_ModuleMgr::~CPDF_ModuleMgr()
{
}

void CPDF_ModuleMgr::RegisterSecurityHandler(FX_LPCSTR filter, CPDF_SecurityHandler * (*CreateHandler)(void* param), void* param)
{
    if (CreateHandler == NULL) {
        m_SecurityHandlerMap.RemoveKey(filter);
    } else {
        m_SecurityHandlerMap.SetAt(filter, (void*)CreateHandler);
    }
    if (param) {
        m_SecurityHandlerMap.SetAt(FX_BSTRC("_param_") + filter, param);
    }
}
void CPDF_ModuleMgr::SetPrivateData(FX_LPVOID module_id, FX_LPVOID pData, PD_CALLBACK_FREEDATA callback)
{
    m_privateData.SetPrivateData(module_id, pData, callback);
}
FX_LPVOID CPDF_ModuleMgr::GetPrivateData(FX_LPVOID module_id)
{
    return m_privateData.GetPrivateData(module_id);
}
CPDF_SecurityHandler* CPDF_ModuleMgr::CreateSecurityHandler(FX_LPCSTR filter)
{
    CPDF_SecurityHandler* (*CreateHandler)(void*) = NULL;
    if (!m_SecurityHandlerMap.Lookup(filter, (void*&)CreateHandler)) {
        return NULL;
    }
    if (CreateHandler == NULL) {
        return NULL;
    }
    void* param = NULL;
    m_SecurityHandlerMap.Lookup(FX_BSTRC("_param_") + filter, param);
    return CreateHandler(param);
}
ICodec_FlateModule* CPDF_ModuleMgr::GetFlateModule()
{
    return m_pCodecModule ? m_pCodecModule->GetFlateModule() : NULL;
}
