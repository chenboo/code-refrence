//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"

using namespace Windows::Data::Xml::Dom;

Element::Element() :
    m_size(),
    m_name(nullptr),
    m_parent(nullptr),
    m_nextSibling(nullptr),
    m_firstChild(nullptr),
	m_rcLayout()
{
}

//  This method is called to release the reference to the parent element.
//  It is necessary to traverse the tree to release the references explicitly,
//  otherwise the destructor will never be called due to cycle dependency
//  between the child and its parent element.
//
//  This method could be removed once the reference to the parent element
//  can be expressed as a weak reference.
//
void Element::ReleaseParent()
{
    if (m_firstChild != nullptr)
    {
        m_firstChild->ReleaseParent();
    }

    if (m_nextSibling != nullptr)
    {
        m_nextSibling->ReleaseParent();
    }

    m_parent = nullptr;
}

void Element::AddChild(_In_ Element^ element)
{
    if (m_firstChild == nullptr)
    {
        element->m_parent = this;
        m_firstChild = element;
    }
    else
    {
        m_firstChild->AddSibling(element);
    }
}

void Element::AddSibling(_In_ Element^ element)
{
    if (m_nextSibling == nullptr)
    {
        element->m_parent = m_parent;
        m_nextSibling = element;
    }
    else
    {
        m_nextSibling->AddSibling(element);
    }
}

bool Element::Initialize(
    _In_ Document^ document,
    _In_ XmlElement^ xmlElement
    )
{
    Platform::String^ value;

    value = xmlElement->GetAttribute("name");

    if (value != nullptr)
    {
        m_name = ref new Platform::String(value->Data());
    }

    value = xmlElement->GetAttribute("width");

    if (value != nullptr)
    {
        m_size.width = static_cast<float>(_wtof(value->Data()));
    }

    value = xmlElement->GetAttribute("height");

    if (value != nullptr)
    {
        m_size.height = static_cast<float>(_wtof(value->Data()));
    }

    return true;
}

bool Element::AcceptChildNode(
    _In_ Document^ document,
    _In_ IXmlNode^ childXmlNode
    )
{
    // Not accepting any child element by default
    return false;
}

void Element::Measure(
	_In_ Document^ document,
	_In_ D2D1_POINT_2F pos,
	_Out_ D2D1_RECT_F* bounds
    )
{
	D2D1_POINT_2F posOffset = pos;
	m_rcLayout.left = posOffset.x;
	m_rcLayout.top = posOffset.y;

	D2D1_RECT_F maxBounds = {0};
	Element^ child = GetFirstChild();

    while (child)
    {
        D2D1_RECT_F childBounds = {0};

        child->Measure(document, posOffset, &childBounds);

        if (childBounds.left < maxBounds.left)
        {
            maxBounds.left = childBounds.left;
        }

        if (childBounds.right > maxBounds.right)
        {
            maxBounds.right = childBounds.right;
        }

        if (childBounds.top < maxBounds.top)
        {
            maxBounds.top = childBounds.top;
        }

		posOffset.y += childBounds.bottom;
		maxBounds.bottom += childBounds.bottom;
        child = child->GetNextSibling();
    }

    // Element size may be adjusted to accommodate the bounds of all its children.
    m_size = D2D1::SizeF(maxBounds.right - maxBounds.left, maxBounds.bottom - maxBounds.top);
	m_rcLayout.right = m_rcLayout.left + m_size.width;
	m_rcLayout.bottom = m_rcLayout.top + m_size.height;

    *bounds = D2D1::RectF(
        maxBounds.left,
        maxBounds.top,
        maxBounds.right,
        maxBounds.bottom
        );
}

bool Element::Draw(
    _In_ Document^ document,
    D2D1::Matrix3x2F const& transform
    )
{
    return DrawChildren(
        document,
        D2D1::Matrix3x2F::Translation(0, 0) * transform
        );
}

bool Element::DrawChildren(
    _In_ Document^ document,
    D2D1::Matrix3x2F const& transform
    )
{
    float offsetY = 0;
    Element^ child = GetFirstChild();

    while (child)
    {
        if (child->Draw(
            document,
            D2D1::Matrix3x2F::Translation(0, offsetY) * transform
            ))
        {
            return true;
        }

        offsetY += child->GetSize().height;
        child = child->GetNextSibling();
    }

    return false;
}


uint32 Element::ParseColor(_In_ Platform::String^ value)
{
    uint32 color = 0;

    wchar_t const* string = value->Data();

    if (!value->IsEmpty() && string[0] == '#')
    {
        color = wcstoul(string + 1, nullptr, 16);
    }

    return color;
}

Platform::String^ Element::ParseNameIdentifier(_In_ Platform::String^ value)
{
    wchar_t const* string = value->Data();

    if (!value->IsEmpty() && string[0] == '#')
    {
        Platform::String^ name = ref new Platform::String(const_cast<wchar_t*>(string + 1));
        return name;
    }

    return nullptr;
}
