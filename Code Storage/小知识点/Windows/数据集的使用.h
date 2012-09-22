//vectorʹ��

//1>Ҫʹ��vector������Ҫ�������µ�ͷ�ļ��������ռ�
#include <vector>
using std::vector;

//2>ʹ�÷�������:
	typedef struct Bar_Item
	{
		UINT		nType;
		DWORD_PTR	dwCtrl;
		CRect		rect;
		BOOL		bShow;

	public:
		Bar_Item()
		{
			nType = 0;
			dwCtrl = 0;
			bShow = TRUE;
			rect.SetRectEmpty();
		}
	}BAR_ITEM;

	typedef vector<BAR_ITEM> CItemArray;
	
	CItemArray		m_arrLeftItem;
	
//3>���Ԫ��
	BAR_ITEM item;
	item.bShow = bShow;
	item.nType = nType;
	item.dwCtrl = dwCtrl;
	item.rect = rect;

	if(nAlign == BAR_ALIGN_LEFT)
	{
		m_arrLeftItem.push_back(item);
	}
	else if(nAlign == BAR_ALIGN_RIGHT)
	{
		m_arrRightItem.push_back(item);
	}
	
//4>ɾ��Ԫ��
	m_arrLeftItem.erase(m_arrLeftItem.begin() + nItem);


//5>ɾ������Ԫ��
	CItemArray::iterator it = m_arrLeftItem.begin();
	while (it != m_arrLeftItem.end()) {
		BAR_ITEM item = (*it);

		m_arrLeftItem.erase(it);
		it = m_arrLeftItem.begin();
	}


//��������һ��Map��ʹ��

//ͬ����������Ҫ�����Ӧ��ͷ�ļ����Լ������ռ�
#include <map>
using namespace std;

//�÷�������ʾ
typedef std::map<DWORD, CChannelWnd*> CChannelMap;
CChannelMap			m_MapChannel;

//���
CRect rect(0, 0, 0, 0);
DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
CChannelWnd* pChannel = new CChannelWnd();
pChannel->SetNotifyWnd(GetSafeHwnd(), WM_CHANNEL_SELECTED);
pChannel->Create(NULL, NULL, dwStyle, rect, this, ID_CHANNEL + m_MapChannel.size());
m_MapChannel.insert(CChannelMap::value_type(pMemberInfo->dwIpAddress, pChannel));

//ɾ��
CChannelMap::iterator it = m_MapChannel.find(pMemberInfo->dwIpAddress);

if(it != m_MapChannel.end())
{
	delete it->second;
	m_MapChannel.erase(it);
}

//ȫ��ɾ��
CVoiceMonitorView::~CVoiceMonitorView()
{
	CChannelMap::iterator it = m_MapChannel.begin();
	while(it != m_MapChannel.end())
	{
		delete it->second;
		it++;
	}
	m_MapChannel.clear();
}


