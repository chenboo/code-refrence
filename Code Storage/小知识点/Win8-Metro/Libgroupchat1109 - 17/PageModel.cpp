//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"

using namespace Libgroupchat;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Interop;

PageModel::PageModel(
    _In_ Element^ content,
    _In_ Document^ document
    )
{
    if (content != nullptr)
    {
        m_content = new ContentImageSource(content, document, false);
    }
}

void PageModel::UpdateContent(_In_ Element^ content)
{
	if (nullptr == content) {
		return;
	}

	// Top-level content is always resized to fit with the current window size.
	m_content->UpdateContent(content);
	PropertyChanged(this, ref new PropertyChangedEventArgs("Content"));
	PropertyChanged(this, ref new PropertyChangedEventArgs("ContentWidth"));
	PropertyChanged(this, ref new PropertyChangedEventArgs("ContentHeight"));
}

void PageModel::ContentChanged()
{
	m_content->ContentChanged();
}

void PageModel::InvalidateRect(const RECT& rc)
{
	if (nullptr == m_content) {
		return;
	}

	m_content->InvalidateRect(rc);

	PropertyChanged(this, ref new PropertyChangedEventArgs("Content"));
	PropertyChanged(this, ref new PropertyChangedEventArgs("ContentWidth"));
	PropertyChanged(this, ref new PropertyChangedEventArgs("ContentHeight"));
}