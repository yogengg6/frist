using System;
using System.Runtime.Serialization;

namespace OpenEBTSNet
{
	public class OpenEBTSException :Exception
	{
		public int ErrorCode { get; private set; }

		public OpenEBTSException(SerializationInfo serializationInfo, StreamingContext  streamingContext ) : base(serializationInfo, streamingContext)
		{
			ErrorCode = -1;
		}

		public OpenEBTSException(string functionName, int errorCode, string errorMessage = null) :
			base(GetErrorMessage(functionName, (OpenEBTSErrors.ErrorCodes)errorCode, errorMessage))
		{
			ErrorCode = (int)errorCode;
		}

		static public string GetErrorMessage(string functionName, OpenEBTSErrors.ErrorCodes errorCode, string errorMessage)
		{
			string message = string.Format("OpenEBTS function {0} reported error {1}: {2}", functionName, errorCode,
										   OpenEBTSErrors.GetDescription(errorCode, ""));

			if (errorCode == OpenEBTSErrors.ErrorCodes.IW_ERR_LOADING_VERICATION && !string.IsNullOrEmpty(errorMessage))
			{
				message += string.Format("\nVerification file error:\n{0}", errorMessage);
			}

			return message;
		}
	}
}
