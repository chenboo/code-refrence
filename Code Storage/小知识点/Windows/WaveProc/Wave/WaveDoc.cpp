// WaveDoc.cpp : implementation of the CWaveDoc class
//
#include "stdafx.h"
#include "Wave.h"

#include "WaveDoc.h"

#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaveDoc

IMPLEMENT_DYNCREATE(CWaveDoc, CDocument)

BEGIN_MESSAGE_MAP(CWaveDoc, CDocument)
	//{{AFX_MSG_MAP(CWaveDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//  DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaveDoc construction/destruction

CWaveDoc::CWaveDoc()
{
	// TODO: add one-time construction code here
	fileOpened = FALSE;
}

CWaveDoc::~CWaveDoc()
{
	if (fileOpened) {
		if (channels == 1) delete[] dm;
		else {delete[] dl; delete[] dr;}
		fileOpened = FALSE;
	}
}

BOOL CWaveDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CWaveDoc serialization

#define ID_RIFF	mmioFOURCC('R', 'I', 'F', 'F')
#define ID_WAVE	mmioFOURCC('W', 'A', 'V', 'E')
#define ID_fmt	mmioFOURCC('f', 'm', 't', ' ')
#define ID_data	mmioFOURCC('d', 'a', 't', 'a')

void CWaveDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
		if (fileOpened) {
			if (channels == 1) delete[] dm;
			else {delete[] dl; delete[] dr;}
			fileOpened = FALSE;
		}
//
		//
         m_path=ar.GetFile()->GetFilePath();
		 paly();
		 
		//

		BOOL format = FALSE, data = FALSE;
		WORD bytes;
		DWORD i, pos=12, fileSize = ar.GetFile()->GetLength();
		float df; // unitary quantization step

		FOURCC id;
		DWORD fileLen, chkLen, sampleRate, avgBytesRate;
		WORD formatTag, bitsPerSample, extSize, blockAlign;

		// read RIFF header
		ar >> id;
		if (id != ID_RIFF) {
			MessageBox(NULL, "Not RIFF format!", "Error", MB_OK);
			throw(new CFileException(CFileException::generic));
		}
		ar >> fileLen;
		if (fileLen != fileSize - 8) {
			MessageBox(NULL, "Wrong file size!", "Error", MB_OK);
			throw(new CFileException(CFileException::generic));
		}

		// read wave id
		ar >> id;
		if (id != ID_WAVE) {
			MessageBox(NULL, "Not WAVE format!", "Error", MB_OK); 
			throw(new CFileException(CFileException::generic));
		}

		// read chunks
		while (!format || !data || pos < fileSize) {
			ar >> id;
			ar >> chkLen;
			if (id == ID_fmt) { // read format chunk
				ar >> formatTag;
				if (formatTag != 1) {
					MessageBox(NULL, "Not PCM format!", "Error", MB_OK); 
					throw(new CFileException(CFileException::generic));
				}
				ar >> channels;
				ar >> sampleRate;
				ar >> avgBytesRate;
				ar >> blockAlign;
				ar >> bitsPerSample;
				if (chkLen == 18) {
					ar >> extSize;
					char *buf = new char[extSize];
					if (extSize > 0) ar.Read(buf, (UINT)extSize);
					delete[] buf;
				}
				else extSize = 0;
				pos += 8 + chkLen + extSize;
				format = TRUE;
			}
			else if (id == ID_data) { // read data chunk
				if (!format) {
					MessageBox(NULL, "No format chunk before data chunk!", "Error", MB_OK);
					throw(new CFileException(CFileException::generic));
				}
				// validate and calculate parameters
				if (bitsPerSample <= 0) {
					MessageBox(NULL, "Wrong bits per sample!", "Error", MB_OK); 
					throw(new CFileException(CFileException::generic));
				}
				bytes = (WORD)((bitsPerSample + 7) / 8);
				if (bytes > 2) {
					MessageBox(NULL, "Too large bits per sample!", "Error", MB_OK);
					throw(new CFileException(CFileException::generic));
				}
				n = chkLen / (channels * bytes);
				if (n <= 0) {
					MessageBox(NULL, "Sample number <= 0!", "Error", MB_OK);
					throw(new CFileException(CFileException::generic));
				}
				//timeLen = (float)n / sampleRate;

				// calculate unitary quantization step 
				df = 1.0f / (1l << (bitsPerSample - 1));

				// read in sample data
				if (channels == 2) { // stereo
					// allocate memory for float sample data
					dl = new float[n]; dr = new float[n];

					// allocate temporary memory and read sample data
					BYTE (*b2)[2];
					short (*s2)[2];
					if (bytes == 1) {b2 = new BYTE[n][2]; ar.Read(b2, n*2);}
					else {s2 = new short[n][2]; ar.Read(s2, n*4);}

					// convert sample to unitary float data
					for (i = 0; i < n; i ++ ) {
						dl[i] = ((bytes == 1) ? b2[i][0] - 128 : s2[i][0]) * df;
						dr[i] = ((bytes == 1) ? b2[i][1] - 128 : s2[i][1]) * df;
					}
					if (bytes == 1) delete[] b2; else delete[] s2;
				}
				else { // mono
					// allocate memory for float sample data
					dm = new float[n];

					// allocate temporary memory and read sample data
					BYTE *b1;
					short *s1;
					if (bytes == 1) {b1 = new BYTE[n]; ar.Read(b1, n);}
					else {s1 = new short[n]; ar.Read(s1, n*2);}

					// convert sample to unitary float data
					for (i = 0; i < n; i ++ ) 
						dm[i] = ((bytes == 1) ? b1[i] - 128 : s1[i]) * df;
					if (bytes == 1) delete[] b1; else delete[] s1;
				}
				pos += 8 + chkLen;
				data = TRUE;
				break;
			}
			else { //read other chunk
				char* buf = new char[chkLen];
				ar.Read(buf, chkLen);
				pos += 8 + chkLen;
				delete[] buf;
			}
		}

		//ar.Close();
		if (!format || !data) {
			MessageBox(NULL, "Wrong file data!", "Error", MB_OK); 
			throw(new CFileException(CFileException::generic));
		}

		fileOpened = TRUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWaveDoc diagnostics

#ifdef _DEBUG
void CWaveDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWaveDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWaveDoc commands

void CWaveDoc::paly()
{
	PlaySound(m_path,NULL,SND_ASYNC);
}

void CWaveDoc::stop()
{
PlaySound(NULL,NULL,SND_ASYNC);
}
