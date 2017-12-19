#include "../../include/fxcrt/fx_basic.h"
#include "plex.h"
CFX_PtrList::CFX_PtrList(int nBlockSize)
    : m_pNodeHead(NULL)
    , m_pNodeTail(NULL)
    , m_nCount(0)
    , m_pNodeFree(NULL)
    , m_pBlocks(NULL)
    , m_nBlockSize(nBlockSize)
{
}
FX_POSITION CFX_PtrList::AddTail(void* newElement)
{
    CNode* pNewNode = NewNode(m_pNodeTail, NULL);
    pNewNode->data = newElement;
    if (m_pNodeTail != NULL) {
        m_pNodeTail->pNext = pNewNode;
    } else {
        m_pNodeHead = pNewNode;
    }
    m_pNodeTail = pNewNode;
    return (FX_POSITION) pNewNode;
}
FX_POSITION CFX_PtrList::AddHead(void* newElement)
{
    CNode* pNewNode = NewNode(NULL, m_pNodeHead);
    pNewNode->data = newElement;
    if (m_pNodeHead != NULL) {
        m_pNodeHead->pPrev = pNewNode;
    } else {
        m_pNodeTail = pNewNode;
    }
    m_pNodeHead = pNewNode;
    return (FX_POSITION) pNewNode;
}
FX_POSITION CFX_PtrList::InsertAfter(FX_POSITION position, void* newElement)
{
    if (position == NULL) {
        return AddTail(newElement);
    }
    CNode* pOldNode = (CNode*) position;
    CNode* pNewNode = NewNode(pOldNode, pOldNode->pNext);
    pNewNode->data = newElement;
    if (pOldNode->pNext != NULL) {
        pOldNode->pNext->pPrev = pNewNode;
    } else {
        m_pNodeTail = pNewNode;
    }
    pOldNode->pNext = pNewNode;
    return (FX_POSITION) pNewNode;
}
void CFX_PtrList::RemoveAt(FX_POSITION position)
{
    CNode* pOldNode = (CNode*) position;
    if (pOldNode == m_pNodeHead) {
        m_pNodeHead = pOldNode->pNext;
    } else {
        pOldNode->pPrev->pNext = pOldNode->pNext;
    }
    if (pOldNode == m_pNodeTail) {
        m_pNodeTail = pOldNode->pPrev;
    } else {
        pOldNode->pNext->pPrev = pOldNode->pPrev;
    }
    FreeNode(pOldNode);
}
void CFX_PtrList::FreeNode(CFX_PtrList::CNode* pNode)
{
    pNode->pNext = m_pNodeFree;
    m_pNodeFree = pNode;
    m_nCount--;
    if (m_nCount == 0) {
        RemoveAll();
    }
}
void CFX_PtrList::RemoveAll()
{
    m_nCount = 0;
    m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
    if (m_pBlocks) {
        m_pBlocks->FreeDataChain();
    }
    m_pBlocks = NULL;
}
CFX_PtrList::CNode*
CFX_PtrList::NewNode(CFX_PtrList::CNode* pPrev, CFX_PtrList::CNode* pNext)
{
    if (m_pNodeFree == NULL) {
        CFX_Plex* pNewBlock = CFX_Plex::Create(m_pBlocks, m_nBlockSize, sizeof(CNode));
        CNode* pNode = (CNode*)pNewBlock->data();
        pNode += m_nBlockSize - 1;
        for (int i = m_nBlockSize - 1; i >= 0; i--, pNode--) {
            pNode->pNext = m_pNodeFree;
            m_pNodeFree = pNode;
        }
    }
    ASSERT(m_pNodeFree != NULL);
    CFX_PtrList::CNode* pNode = m_pNodeFree;
    m_pNodeFree = m_pNodeFree->pNext;
    pNode->pPrev = pPrev;
    pNode->pNext = pNext;
    m_nCount++;
    ASSERT(m_nCount > 0);
    pNode->data = 0;
    return pNode;
}
CFX_PtrList::~CFX_PtrList()
{
    RemoveAll();
    ASSERT(m_nCount == 0);
}
FX_POSITION CFX_PtrList::FindIndex(int nIndex) const
{
    if (nIndex >= m_nCount || nIndex < 0) {
        return NULL;
    }
    CNode* pNode = m_pNodeHead;
    while (nIndex--) {
        pNode = pNode->pNext;
    }
    return (FX_POSITION) pNode;
}
FX_POSITION CFX_PtrList::Find(void* searchValue, FX_POSITION startAfter) const
{
    CNode* pNode = (CNode*) startAfter;
    if (pNode == NULL) {
        pNode = m_pNodeHead;
    } else {
        pNode = pNode->pNext;
    }
    for (; pNode != NULL; pNode = pNode->pNext)
        if (pNode->data == searchValue) {
            return (FX_POSITION) pNode;
        }
    return NULL;
}
