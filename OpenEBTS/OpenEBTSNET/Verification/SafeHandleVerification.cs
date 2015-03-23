using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace OpenEBTSNet
{
	internal class SafeHandleVerification : SafeHandleZeroOrMinusOneIsInvalid
	{
		internal SafeHandleVerification(IntPtr handle)
			: base(true)
		{
			SetHandle(handle);
		}

		protected override bool ReleaseHandle()
		{
			if (handle != IntPtr.Zero)
			{
				handle = OpenEBTSInteropWrap.CloseVerification(handle);
			}
			return true;
		}
	}
}
