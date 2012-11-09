#pragma once

//  <list> arranges its child content by stacking them vertically from top to bottom.
//  If the child content can't fit within the original size of the list, the list will
//  expand to accommodate them. List is direct subclass of Rectangle. It has all the
//  characteristic of a rectangle drawing.
//
//  List accepts the following additional markup attributes:
//      margin = "<float>"
//          Amount of spacing around the four edges of the list to where the child
//          element is drawn
//
ref class MTitle : public Element
{
internal:
    MTitle();

    virtual bool AcceptChildNode(
        _In_ Document^ document,
        _In_ Windows::Data::Xml::Dom::IXmlNode^ childXmlNode
        ) override;
};
