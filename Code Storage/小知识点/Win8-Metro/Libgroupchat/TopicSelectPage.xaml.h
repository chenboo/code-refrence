//
// TopicSelectPage.xaml.h
// Declaration of the TopicSelectPage class
//

#pragma once

#include "Common\LayoutAwarePage.h" // Required by generated header
#include "TopicSelectPage.g.h"

namespace Libgroupchat
{
	/// <summary>
	/// A page that displays a collection of item previews.  In the Split Application this page
	/// is used to display and select one of the available groups.
	/// </summary>
	public ref class TopicSelectPage sealed
	{
	public:
		TopicSelectPage();

	protected:
		virtual void LoadState(Platform::Object^ navigationParameter,
			Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;
	};
}
