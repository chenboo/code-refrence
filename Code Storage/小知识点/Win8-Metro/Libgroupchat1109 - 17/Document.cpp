//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"

using namespace Microsoft::WRL;
using namespace concurrency;
using namespace Windows::Storage;
using namespace Windows::Storage::Search;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Data::Xml::Dom;

Document::Document(
    _In_ StorageFolder^ location,
    _In_ Platform::String^ fileName,
    _In_ Renderer^ renderer
    ) :
    m_location(location),
    m_fileName(fileName),
    m_rootXml(nullptr),
    m_rootElement(nullptr),
    m_renderer(renderer),
    m_fontCollection(),
    m_dwriteFactory()
{
    // Create DirectWrite isolated factory.
    //
    // Unlike a typical shared factory, the isolated factory does not send miss report
    // to the cross-process font cache, and therefore does not interact with DirectWrite's
    // state in other component in the same process. It's designed to be used when the
    // usage of font data within one component is meant to be isolated from the rest of
    // the application.
    //
    DX::ThrowIfFailed(
        DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_ISOLATED,
            __uuidof(IDWriteFactory),
            &m_dwriteFactory
            )
        );
}

Document::~Document()
{
}

task<void> Document::LoadAsync()
{
    // Get the document file from the storage location
    return task<StorageFile^>(m_location->GetFileAsync(m_fileName)).then([=](StorageFile^ file)
    {
        auto xmlDocument = ref new XmlDocument();
        return xmlDocument->LoadFromFileAsync(file);

    }).then([=](XmlDocument^ loadedXml)
    {
        m_rootXml = loadedXml->SelectSingleNode("Chat");
    });
}

void Document::GetDWriteFactory(_Outptr_ IDWriteFactory** dwriteFactory)
{
    *dwriteFactory = ComPtr<IDWriteFactory>(m_dwriteFactory).Detach();
}

//  Parse the XML document to an element tree, then traverse the tree
//  to bind resources to the element requiring them.
void Document::Parse()
{
	if (m_rootXml != nullptr) {
		Chat^ rootElement = Element::Create<Chat>(this, m_rootXml);
		m_rootElement = rootElement;
	}
}

Element^ Document::GetContentRoot()
{
	return m_rootElement;
}