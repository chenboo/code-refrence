#pragma once
#include "pch.h"
#include <collection.h>

namespace Libgroupchat 
{
	[Windows::UI::Xaml::Data::Bindable]
	public ref class Item sealed
	{
	public:
		Item();
		property Platform::String^ Imagpath;

	private:
		~Item(){};

	};

	[Windows::UI::Xaml::Data::Bindable]
	public ref class ItemSource sealed
	{
	public:
		ItemSource ();
		

		property Windows::Foundation::Collections::IVector<Item^>^ Items
		{
			Windows::Foundation::Collections::IVector<Item^>^ get() {return m_items; }
		}

	private:

		Platform::Collections::Vector<Item^>^ m_items;

	private:
		~ItemSource () {};

	};
}