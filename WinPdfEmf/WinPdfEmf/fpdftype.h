#pragma once
#ifdef _WIN32
  #define snprintf _snprintf
#endif

#define FPDF_ANNOT			0x01
#define FPDF_NO_CATCH		0x100
#define FPDF_PRINTING		0x800

enum OutputFormat {
  OUTPUT_NONE,
  OUTPUT_PPM,
#ifdef _WIN32
  OUTPUT_BMP,
  OUTPUT_EMF,
#endif
};

typedef void*	FPDF_DOCUMENT;
typedef void*	FPDF_PAGE;


#ifdef _WIN32_WCE
typedef const unsigned short* FPDF_STRING;
#else
typedef const char* FPDF_STRING;
#endif
typedef const char*				FPDF_BYTESTRING;


 