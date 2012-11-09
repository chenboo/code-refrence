#include "pch.h"
#include "ChatDatas.h"
using namespace Libgroupchat;

ChatDatas ::ChatDatas()
{
	m_items = ref new Platform::Collections::Vector<ChatMessage^>();
}

ChatMessage::ChatMessage()
{
}
