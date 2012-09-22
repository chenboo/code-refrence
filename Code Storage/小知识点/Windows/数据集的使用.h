//vector使用

//1>要使用vector，首先要加入以下的头文件和命名空间
#include <vector>
using std::vector;

//2>使用方法举例:
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
	
//3>添加元素
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
	
//4>删除元素
	m_arrLeftItem.erase(m_arrLeftItem.begin() + nItem);


//5>删除所有元素
	CItemArray::iterator it = m_arrLeftItem.begin();
	while (it != m_arrLeftItem.end()) {
		BAR_ITEM item = (*it);

		m_arrLeftItem.erase(it);
		it = m_arrLeftItem.begin();
	}


//下面来讲一下Map的使用

//同样，首先需要添加相应的头文件，以及命名空间
#include <map>
using namespace std;

//用法如下所示
typedef std::map<DWORD, CChannelWnd*> CChannelMap;
CChannelMap			m_MapChannel;

//添加
CRect rect(0, 0, 0, 0);
DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
CChannelWnd* pChannel = new CChannelWnd();
pChannel->SetNotifyWnd(GetSafeHwnd(), WM_CHANNEL_SELECTED);
pChannel->Create(NULL, NULL, dwStyle, rect, this, ID_CHANNEL + m_MapChannel.size());
m_MapChannel.insert(CChannelMap::value_type(pMemberInfo->dwIpAddress, pChannel));

//删除
CChannelMap::iterator it = m_MapChannel.find(pMemberInfo->dwIpAddress);

if(it != m_MapChannel.end())
{
	delete it->second;
	m_MapChannel.erase(it);
}

//全部删除
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


