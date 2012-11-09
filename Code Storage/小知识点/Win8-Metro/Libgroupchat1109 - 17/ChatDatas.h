#pragma once
#include <collection.h>

namespace Libgroupchat 
{
	[Windows::UI::Xaml::Data::Bindable]
	public ref class ChatMessage sealed
	{
	public:
		ChatMessage();
		property Platform::String^ Speaker;
		property Platform::String^ Time;
		property Platform::String^ Content;

	private:
		~ChatMessage(){};
	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class ChatDatas sealed
	{
	public:
		ChatDatas ();
		property Windows::Foundation::Collections::IVector<ChatMessage^>^ Items
		{
			Windows::Foundation::Collections::IVector<ChatMessage^>^ get() {return m_items; }
		}

	private:
		Platform::Collections::Vector<ChatMessage^>^ m_items;

	private:
		~ChatDatas () {};
	};
}