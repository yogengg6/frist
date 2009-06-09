/*******************************************************************************

License: 
This software was developed at the National Institute of Standards and 
Technology (NIST) by employees of the Federal Government in the course 
of their official duties. Pursuant to title 17 Section 105 of the 
United States Code, this software is not subject to copyright protection 
and is in the public domain. NIST assumes no responsibility  whatsoever for 
its use by other parties, and makes no guarantees, expressed or implied, 
about its quality, reliability, or any other characteristic. 

Disclaimer: 
This software was developed to promote biometric standards and biometric
technology testing for the Federal Government in accordance with the USA
PATRIOT Act and the Enhanced Border Security and Visa Entry Reform Act.
Specific hardware and software products identified in this software were used
in order to perform the software development.  In no case does such
identification imply recommendation or endorsement by the National Institute
of Standards and Technology, nor does it imply that the products and equipment
identified are necessarily the best available for the purpose.  

*******************************************************************************/

#ifndef _IMGTYPE_H
#define _IMGTYPE_H

/* UPDATED: 03/15/2005 by MDG */

#include <wsq.h>
#include <jpegb.h>
#include <jpegl.h>
#include <ihead.h>
#include <an2k.h>
#include <dataio.h>
#include <computil.h>

#define UNKNOWN_IMG -1
#define RAW_IMG     0
#define WSQ_IMG     1
#define JPEGL_IMG   2
#define JPEGB_IMG   3
#define IHEAD_IMG   4
#define ANSI_NIST_IMG 5

/* imgtype.c */
extern int image_type(int *, unsigned char *, const int);
extern int jpeg_type(int *, unsigned char *, const int);

#endif /* !_IMGTYPE_H */
