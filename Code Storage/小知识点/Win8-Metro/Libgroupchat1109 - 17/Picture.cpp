#include "pch.h"
#include "Picture.h"

using namespace Libgroupchat::StructData;

PictureInfo::PictureInfo()
{
	nCurrentFrame = 0;
	m_Frames = ref new Platform::Collections::Vector<FrameInfo^>();
}

BOOL PictureInfo::IsDynamicPicture()
{
	return (m_Frames->Size > 1);
}

FrameInfo^ PictureInfo::GetNextFrame()
{
	if (nCurrentFrame >= m_Frames->Size) {
		nCurrentFrame = 0;
	}

	return m_Frames->GetAt(nCurrentFrame++);
}