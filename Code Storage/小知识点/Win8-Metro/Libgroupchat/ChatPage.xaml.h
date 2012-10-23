//
// ChatPage.xaml.h
// Declaration of the ChatPage class
//

#pragma once

#include "Common\LayoutAwarePage.h" // Required by generated header
#include "ChatPage.g.h"
#include "ItemSource.h"

namespace Libgroupchat
{
	/// <summary>
	/// A basic page that provides characteristics common to most applications.
	/// </summary>
	public ref class ChatPage sealed
	{
	public:
		ChatPage();

	protected:
		virtual void LoadState(Platform::Object^ navigationParameter,
			Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;
		virtual void SaveState(Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;

	private:
		void GetItemData();

	private:
		ItemSource^ m_item;
	};
}
