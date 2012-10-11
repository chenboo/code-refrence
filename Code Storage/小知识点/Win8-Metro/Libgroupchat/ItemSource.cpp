#include "pch.h"
#include "ItemSource.h"
using namespace Libgroupchat;

ItemSource ::ItemSource()
{
	m_items = ref new Platform::Collections::Vector<Item^>();
}

Item::Item()
{
}
