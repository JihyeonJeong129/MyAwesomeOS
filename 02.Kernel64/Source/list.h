#ifndef __LIST_H__
#define __LIST_H__

#include "Types.h"

typedef struct kListHeadStruct {
    struct kListHeadStruct* pstNext;
    struct kListHeadStruct* pstPrev;
} LISTHEAD;

#define kOffsetOf(TYPE, MEMBER) ((QWORD)&(((TYPE*)0)->MEMBER))

#define kContainerOf(PTR, TYPE, MEMBER) \
    ((TYPE*)((char*)(PTR) - kOffsetOf(TYPE, MEMBER)))

#define list_entry(PTR, TYPE, MEMBER) \
    kContainerOf(PTR, TYPE, MEMBER)

#define LIST_HEAD_INIT(NAME) { &(NAME), &(NAME) }

#define LIST_HEAD(NAME) \
    LISTHEAD NAME = LIST_HEAD_INIT(NAME)

#define list_for_each_safe(POS, NEXT, HEAD) \
    for ((POS) = (HEAD)->pstNext, (NEXT) = (POS)->pstNext; \
         (POS) != (HEAD); \
         (POS) = (NEXT), (NEXT) = (POS)->pstNext)

static inline void INIT_LIST_HEAD(LISTHEAD* pstList)
{
    pstList->pstNext = pstList;
    pstList->pstPrev = pstList;
}

static inline BOOL list_empty(const LISTHEAD* pstHead)
{
    return (pstHead->pstNext == pstHead);
}

static inline void __list_add(LISTHEAD* pstNew, LISTHEAD* pstPrev,
        LISTHEAD* pstNext)
{
    pstNext->pstPrev = pstNew;
    pstNew->pstNext = pstNext;
    pstNew->pstPrev = pstPrev;
    pstPrev->pstNext = pstNew;
}

static inline void list_add_tail(LISTHEAD* pstNew, LISTHEAD* pstHead)
{
    __list_add(pstNew, pstHead->pstPrev, pstHead);
}

static inline void __list_del(LISTHEAD* pstPrev, LISTHEAD* pstNext)
{
    pstNext->pstPrev = pstPrev;
    pstPrev->pstNext = pstNext;
}

static inline void list_del(LISTHEAD* pstEntry)
{
    __list_del(pstEntry->pstPrev, pstEntry->pstNext);
    INIT_LIST_HEAD(pstEntry);
}

static inline LISTHEAD* list_del_header(LISTHEAD* pstHead)
{
    LISTHEAD* pstEntry;

    if (list_empty(pstHead) == TRUE) {
        return NULL;
    }

    pstEntry = pstHead->pstNext;
    list_del(pstEntry);

    return pstEntry;
}

static inline LISTHEAD* list_del_tail(LISTHEAD* pstHead)
{
    LISTHEAD* pstEntry;

    if (list_empty(pstHead) == TRUE) {
        return NULL;
    }

    pstEntry = pstHead->pstPrev;
    list_del(pstEntry);

    return pstEntry;
}

#endif /* __LIST_H__ */
