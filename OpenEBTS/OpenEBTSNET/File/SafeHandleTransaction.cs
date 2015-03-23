using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace OpenEBTSNet
{
	internal class SafeHandleTransaction : SafeHandleZeroOrMinusOneIsInvalid
	{
		internal SafeHandleTransaction(IntPtr handle)
			: base(true)
		{
			SetHandle(handle);
		}

		protected override bool ReleaseHandle()
		{
			if (handle != IntPtr.Zero)
			{
				handle = OpenEBTSInteropWrap.Close(handle);
			}
			return true;
		}
	}
}
