#pragma once
#include "FrameInfo.h"

namespace Libgroupchat
{
	namespace StructData {
		public ref class PictureInfo sealed
		{
		public:
			PictureInfo();
			property Platform::String^ ImagPath;
			property int nPos;
			property int nWidth;
			property int nHeight;
			property int nCurrentFrame;

			property Windows::Foundation::Collections::IVector<FrameInfo^>^ Frames
			{
				Windows::Foundation::Collections::IVector<FrameInfo^>^ get() {return m_Frames; }
			}

			BOOL IsDynamicPicture();
			FrameInfo^ GetNextFrame();

		private:
			~PictureInfo(){};

		private:
			Windows::Foundation::Collections::IVector<FrameInfo^>^ m_Frames;
			
		};
	}
}



