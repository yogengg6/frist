//
// Modified from the original DOMTreeErrorReporter.cpp to include error message storage
//

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/sax/SAXParseException.hpp>
#include "DOMTreeErrorReporter.hpp"
#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif
#include <stdlib.h>
#include <string.h>

void DOMTreeErrorReporter::warning(const SAXParseException&)
{
    //
    // Ignore all warnings.
    //
}

void DOMTreeErrorReporter::error(const SAXParseException& toCatch)
{
    fSawErrors = true;

    swprintf_s(errorMessage, MAXERRORLEN, L"Error on line %d, column %d: ", toCatch.getLineNumber(), toCatch.getColumnNumber());
    wcscat_s(errorMessage, MAXERRORLEN, toCatch.getMessage());

   // XERCES_STD_QUALIFIER cerr << "Error at file \"" << StrX(toCatch.getSystemId())
		 //<< "\", line " << toCatch.getLineNumber()
		 //<< ", column " << toCatch.getColumnNumber()
   //      << "\n   Message: " << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void DOMTreeErrorReporter::fatalError(const SAXParseException& toCatch)
{
    fSawErrors = true;

    swprintf_s(errorMessage, MAXERRORLEN, L"Error on line %d, column %d: ", toCatch.getLineNumber(), toCatch.getColumnNumber());
    wcscat_s(errorMessage, MAXERRORLEN, toCatch.getMessage());

   // XERCES_STD_QUALIFIER cerr << "Fatal Error at file \"" << StrX(toCatch.getSystemId())
		 //<< "\", line " << toCatch.getLineNumber()
		 //<< ", column " << toCatch.getColumnNumber()
   //      << "\n   Message: " << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void DOMTreeErrorReporter::resetErrors()
{
    fSawErrors = false;
}


