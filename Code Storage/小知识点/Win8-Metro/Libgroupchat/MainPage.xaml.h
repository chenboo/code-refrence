﻿//
// MainPage.xaml.h
// Declaration of the MainPage class
//

#pragma once

#include "Common\LayoutAwarePage.h" // Required by generated header
#include "MainPage.g.h"

namespace Libgroupchat
{
	/// <summary>
	/// A basic page that provides characteristics common to most applications.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	protected:
		virtual void LoadState(Platform::Object^ navigationParameter,
			Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;
		virtual void SaveState(Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;
		void LoadPage(Platform::String^ scenarioName);

	private:
		void InvalidateSize();
		void InvalidateViewState();

	private:
		Windows::UI::Xaml::Controls::Frame^ HiddenFrame;
	};
}
