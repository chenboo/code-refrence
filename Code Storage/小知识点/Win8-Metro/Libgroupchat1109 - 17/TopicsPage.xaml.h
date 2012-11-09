#pragma once

#include "Common\LayoutAwarePage.h" // Required by generated header
#include "TopicsPage.g.h"

namespace Libgroupchat
{
	/// <summary>
	/// A page that displays a collection of item previews.  In the Split Application this page
	/// is used to display and select one of the available groups.
	/// </summary>
	public ref class TopicsPage sealed
	{
	public:
		TopicsPage();

	protected:
		virtual void LoadState(Platform::Object^ navigationParameter,
			Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;

	private:
		virtual void Item_Click(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);

	private:
		TopicSource^ m_topics;
	};
}
