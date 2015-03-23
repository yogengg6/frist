using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OpenEBTSNet
{
	/// <summary>
	/// Image formats supported by NIST files
	/// </summary>
	public enum OpenEBTSImageFormat
	{
		/// <summary>
		/// Uncompressed, headerless image data
		/// </summary>
		imageFormatRAW = 0,
		/// <summary>
		/// Windows Bitmap
		/// </summary>
		imageFormatBMP,
		/// <summary>
		/// Standard JPEG image
		/// </summary>
		imageFormatJPG,
		/// <summary>
		/// Wavelet Scalar Quantization, for fingerprints
		/// </summary>
		imageFormatWSQ,
		/// <summary>
		/// JPEG 2000 image
		/// </summary>
		imageFormatJP2,
		/// <summary>
		/// CCITT Group 4 Fax image
		/// </summary>
		imageFormatFX4,
		/// <summary>
		/// CBEFF header (not an image)
		/// </summary>
		imageFormatCBEFF,
		/// <summary>
		/// Portable Network Graphics image
		/// </summary>
		imageFormatPNG
	}
}
