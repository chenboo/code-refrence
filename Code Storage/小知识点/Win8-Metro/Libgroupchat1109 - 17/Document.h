//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

ref class Element;
ref class Chat;
ref class Renderer;
ref class Document;

class FontLoader;

//  This method is called from Document::LoadAsync when the document is fully loaded.
delegate void DocumentLoadedCallback(_In_ Document^ document);

//  Compound document at the storage location. A document consists of a document XML file
//  describing the document structure and content as well as all its associated resource files.
//
//  Each document may contain a specific set of font files whose collective presence represents
//  a collection of fonts available to the document text elements.
//
ref class Document
{
internal:
    Document(
        _In_ Windows::Storage::StorageFolder^ location,
        _In_ Platform::String^ fileName,
        _In_ Renderer^ renderer
        );

    concurrency::task<void> LoadAsync();

    void Parse();
    void GetDWriteFactory(_Outptr_ IDWriteFactory** dwriteFactory);

    inline Windows::Storage::StorageFolder^ GetLocation()
    {
        return m_location;
    }

    inline Renderer^ GetRenderer()
    {
        return m_renderer;
    }

    Element^ GetContentRoot();

private:
    ~Document();

private:
    // Storage location of the document
    Windows::Storage::StorageFolder^ m_location;

    // Document file name
    Platform::String^ m_fileName;

    // Root XML node of the document structure
    Windows::Data::Xml::Dom::IXmlNode^ m_rootXml;

    // Root of the element tree
    Chat^	m_rootElement;

    // DirectX renderer
    Renderer^ m_renderer;

	//the page size
	D2D1_SIZE_F m_pageSize;

    // DirectWrite factory specific to this document
    Microsoft::WRL::ComPtr<IDWriteFactory> m_dwriteFactory;

    // Collection of all document fonts
    Microsoft::WRL::ComPtr<IDWriteFontCollection> m_fontCollection;
};
