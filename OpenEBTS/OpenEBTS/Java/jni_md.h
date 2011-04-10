/*
 * %W% %E%
 *
 * Copyright (c) 2006, Oracle and/or its affiliates. All rights reserved.
 * ORACLE PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

#ifndef _JAVASOFT_JNI_MD_H_
#define _JAVASOFT_JNI_MD_H_

#ifdef WIN32

#define JNIEXPORT __declspec(dllexport)
#define JNIIMPORT __declspec(dllimport)
#define JNICALL __stdcall

typedef long jint;
typedef __int64 jlong;
typedef signed char jbyte;

#else

#define JNIEXPORT
#define JNIIMPORT
#define JNICALL

typedef int32_t jint;
typedef int64_t jlong;
typedef signed char jbyte;

#endif

#endif /* !_JAVASOFT_JNI_MD_H_ */