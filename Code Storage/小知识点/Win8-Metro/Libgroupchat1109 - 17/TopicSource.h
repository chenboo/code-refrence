#pragma once
#include <collection.h>

namespace Libgroupchat
{
	[Windows::UI::Xaml::Data::Bindable]
	public ref class Topic sealed
	{
	public:
		Topic();
		property Platform::String^ Name;
		property int OnLineCount;
	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class TopicSource sealed
	{
	public:
		TopicSource ();

		property Windows::Foundation::Collections::IVector<Topic^>^ Items
		{
			Windows::Foundation::Collections::IVector<Topic^>^ get() {return m_items; }
		}

	private:
		Platform::Collections::Vector<Topic^>^ m_items;
	};
}