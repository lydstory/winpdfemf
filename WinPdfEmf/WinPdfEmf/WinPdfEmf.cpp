// WinPdfEmf.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "error.h"
#include "fpdftype.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <iostream>
using namespace std;
static int LoadPdfSDK(wstring curpath);
static int UnloadPdfSDK(void);
int InitPdfSDK();
int UninitPdfSDK();

HINSTANCE sdklib = NULL;
static int status = 0;
HANDLE hEVENT = NULL;
enum
{
	uninitialized = 0,
	initialized = 1,
	listerning = 2,
};

static int LoadUsbSDK(wstring curpath )
{
	sdklib = LoadLibrary(curpath.c_str());
	if (!sdklib) {
		return FPDFCLN_ERR_LIBNOTFIND;
	}
	return FPDFCLN_ERR_OK;
}

static int UnloadPdfSDK(void)
{
	if (!sdklib) {
		return FPDFCLN_ERR_NOTINITIALIZE;
	}
	if (FreeLibrary(sdklib))
	{
		sdklib = NULL;
		return FPDFCLN_ERR_OK;
	}
	return FPDFCLN_ERR_FAIL;
}

int InitPdfSDK()
{
 
	if (!sdklib) {
		return FPDFCLN_ERR_NOTINITIALIZE;
	}

	typedef void(__stdcall *Func_ApiInit)(void *ptr);
 	Func_ApiInit fn = (Func_ApiInit)GetProcAddress(sdklib, "FPDF_InitLibrary");
	if (!fn) {
		return FPDFCLN_ERR_NOTSUPPORT;
	}
	
	(*fn)(NULL);
	return 0;
 
}


int UninitPdfSDK()
{
 	if (status > initialized) {
		return FPDFCLN_ERR_INVALIDSEQUENCE;
	}
	if (!sdklib) {
		return FPDFCLN_ERR_NOTINITIALIZE;
	}
 
	typedef void(*Func_CleanUp)();

	Func_CleanUp fn = (Func_CleanUp)GetProcAddress(sdklib, "FPDF_DestroyLibrary");
	if (!fn) {
		return FPDFCLN_ERR_NOTSUPPORT;
	}
	(*fn)();
	return 0;
	 
}

int  FPdfcln_Initialize(wstring curpath)
{
//	getServerConnectorTable(); // ensure constructor    
	int ret;
	ret = LoadUsbSDK(curpath);
	if (FPDFCLN_ERR_OK != ret) {
		return ret;
	}
	ret = InitPdfSDK();
	if (FPDFCLN_ERR_OK != ret) {
		UnloadPdfSDK();
	}
	return ret;
}
int FPdfclnd_Shutdown(void)
{
	UninitPdfSDK();
	UnloadPdfSDK();
	return 0;
}

typedef FPDF_DOCUMENT(__stdcall *Func_LoadDocument)(FPDF_STRING file_path,FPDF_BYTESTRING password);
 

typedef int(__stdcall *Func_GetPageCount)(FPDF_DOCUMENT document);
 
 
typedef FPDF_PAGE (__stdcall *Func_LoadPage)(FPDF_DOCUMENT document, int page_index);

 
typedef double (__stdcall *Func_GetPageWidth)(FPDF_PAGE page);
typedef double (__stdcall *Func_GetPageHeight)(FPDF_PAGE page);
 
typedef void  (__stdcall *Func_RenderPage)(HDC dc, FPDF_PAGE page, int start_x, int start_y, int size_x, int size_y,int rotate, int flags);

//DLLEXPORT void STDCALL FPDF_ClosePage(FPDF_PAGE page)
typedef void (__stdcall *Func_ClosePage)(FPDF_PAGE page);
//DLLEXPORT void STDCALL FPDF_CloseDocument(FPDF_DOCUMENT document);
typedef void (__stdcall *Func_CloseDocument)(FPDF_DOCUMENT document);
void WriteEmf(FPDF_PAGE page, const char* pdf_name, int num) {
	Func_GetPageWidth fn = (Func_GetPageWidth)GetProcAddress(sdklib, "FPDF_GetPageWidth");
	int width = (int)(*fn)(page);
	Func_GetPageHeight fn2 = (Func_GetPageHeight)GetProcAddress(sdklib, "FPDF_GetPageHeight");
	int height = (int)(*fn2)(page);
	char filename[256];
	snprintf(filename, sizeof(filename), "%s.%d.emf", pdf_name, num);
    HDC dc = CreateEnhMetaFileA(NULL, filename, NULL, NULL);
	HRGN rgn = CreateRectRgn(0, 0, width, height); 
	SelectClipRgn(dc, rgn); 
	DeleteObject(rgn);
	SelectObject(dc, GetStockObject(NULL_PEN));
	SelectObject(dc, GetStockObject(WHITE_BRUSH));
	// If a PS_NULL pen is used, the dimensions of the rectangle are 1 pixel less.
	Rectangle(dc, 0, 0, width + 1, height + 1);
    Func_RenderPage fn3 = (Func_RenderPage)GetProcAddress(sdklib, "FPDF_RenderPage");
    (*fn3)(dc, page, 0, 0, width, height, 0,
	            FPDF_ANNOT | FPDF_PRINTING | FPDF_NO_CATCH);

	DeleteEnhMetaFile(CloseEnhMetaFile(dc));
}
 

void RenderPdf(const char* name, const char* pBuf, size_t len,
               OutputFormat format) {
  printf("Rendering PDF file %s.\n", name);
 
}


int _tmain(int argc, _TCHAR* argv[])
{
	
	std::cout<<"hello,world"<<std::endl;
	FPdfcln_Initialize(L"C:\\lyd\\work\\fpdfsdk\\WinPdfEmf\\dll\\fpdfsdk.dll");
 
    FPDF_DOCUMENT doc;
	Func_LoadDocument fn = (Func_LoadDocument)GetProcAddress(sdklib, "FPDF_LoadDocument");
    string inpdf ="C:\\1111.pdf";
	doc =(*fn)(inpdf.c_str(),NULL);

	Func_GetPageCount fn2 = (Func_GetPageCount)GetProcAddress(sdklib, "FPDF_GetPageCount");
	int page_count = (*fn2)(doc);
	 
    OutputFormat format = OUTPUT_EMF;
	const char* filename = "C:\\1111.pdf";
  
	Func_LoadPage fn3 = (Func_LoadPage)GetProcAddress(sdklib, "FPDF_LoadPage");
    const char* name = "1111";
  for (int i = 0; i < page_count; ++i) {
    FPDF_PAGE page = (*fn3)(doc, i);
	    WriteEmf(page, name, i);

   }
  
	/*
//	HDC m_hDC;
	HENHMETAFILE hemf = GetEnhMetaFile(L"C:\\1111.1.emf");

	ENHMETAHEADER mfh;
    GetEnhMetaFileHeader(hemf, sizeof(ENHMETAHEADER), &mfh);
    int width = mfh.rclBounds.right - mfh.rclBounds.left;
    int height = mfh.rclBounds.bottom - mfh.rclBounds.top;

	LPBYTE lpbuff;
	FPDF_PAGE page;
	GetEnhMetaFileBits(hemf,mfh.nBytes,lpbuff);
	page =lpbuff;

  

//	PlayEnhMetaFile(m_hDC, hemf, CRect(0, 0, width, height));

	DeleteEnhMetaFile(hemf); 
	  */
	FPdfclnd_Shutdown();
	system("PAUSE");
	return 0;
}

