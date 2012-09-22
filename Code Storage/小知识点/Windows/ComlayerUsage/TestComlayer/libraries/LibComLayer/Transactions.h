#ifndef __TRANSACTIONS_H__
#define __TRANSACTIONS_H__

class CTransactionManager : public ITransactionManager
{
public:
	CTransactionManager();
protected:
	virtual ~CTransactionManager();

public:
	virtual LONG AddRef();
	virtual LONG Release();

	virtual BOOL AddTransaction(GUID *pID, CTransactionHandler *pTransactionHandler);
	virtual BOOL DeleteTransaction(GUID *pID);

	CTransactionHandler *FindTransaction(GUID *pID);
	void DoPendingDeletion();

protected:
	struct CGUIDPtrCompare {
		bool operator()(GUID *p1, GUID *p2) const {
			return memcmp(p1, p2, sizeof(GUID)) < 0;
		}
	};

	void  DeleteAllTransactions();

	typedef std::map<GUID *, CTransactionHandler *, CGUIDPtrCompare>	CTransactionMap;
	typedef std::list<CTransactionHandler *> CTransactionList;

	CTransactionMap	    m_mapTransactions;
	CTransactionList    m_delTransactions;

	CRITICAL_SECTION	m_csTransactions;

	LONG				m_nRef;
};

#endif //__TRANSACTIONS_H__