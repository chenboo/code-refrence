#include "pch.h"
#include "TopicSource.h"
using namespace Libgroupchat;

TopicSource ::TopicSource()
{
	m_items = ref new Platform::Collections::Vector<Topic^>();
}

Topic::Topic()
{
}
