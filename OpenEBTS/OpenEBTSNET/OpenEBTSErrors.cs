

using System.ComponentModel;
using System.Reflection;
namespace OpenEBTSNet
{
	public static class OpenEBTSErrors
	{
		public static string GetDescription(object enumValue, string defDesc)
		{
			FieldInfo fi = enumValue.GetType().GetField(enumValue.ToString());
		
			if (null != fi)
			{
				object[] attrs = fi.GetCustomAttributes(typeof(DescriptionAttribute), true);
				if (attrs != null && attrs.Length > 0)
				{
					return ((DescriptionAttribute)attrs[0]).Description;
				}
			}

			return defDesc;
		}
		
		public enum ErrorCodes
		{
			[Description("Success")]
			IW_SUCCESS = 0, 

			[Description("Error loading verification file")]
			IW_ERR_LOADING_VERICATION = 1,

			[Description("Error opening file for reading")]
			IW_ERR_OPENING_FILE_FOR_READING = 2,

			[Description("Error opening file for writing")]
			IW_ERR_OPENING_FILE_FOR_WRITING = 3,

			[Description("Error reading file")]
			IW_ERR_READING_FILE = 4, 
			
			[Description("Error writing file")]
			IW_ERR_WRITING_FILE = 5,

			[Description("Record not found")]
			IW_ERR_RECORD_NOT_FOUND = 6,

			[Description("Transaction not loaded")]
			IW_ERR_TRANSACTION_NOT_LOADED = 7,

			[Description("Verification file is not loaded")]
			IW_ERR_VERIFICATION_NOT_LOADED = 8,

			[Description("Null pointer")]
			IW_ERR_NULL_POINTER = 9,

			[Description("Null transaction pointer")]
			IW_ERR_NULL_TRANSACTION_POINTER = 10,

			[Description("Unsupported image format")]		
			IW_ERR_UNSUPPORTED_IMAGE_FORMAT = 11,

			[Description("Unsupported record type")]
			IW_ERR_UNSUPPORTED_RECORD_TYPE = 12,

			[Description("Index out of range")]
			IW_ERR_INDEX_OUT_OF_RANGE = 13,

			[Description("Invalid subfield number")]
			IW_ERR_INVALID_SUBFIELD_NUM = 14,

			[Description("Mnemonic not found")]
			IW_ERR_MNEMONIC_NOT_FOUND = 15,

			[Description("Out of memory")]
			IW_ERR_OUT_OF_MEMORY = 16,

			[Description("Error during a WSQ compress")]
			IW_ERR_WSQ_COMPRESS = 17,

			[Description("Error during a WSQ decompress")]
			IW_ERR_WSQ_DECOMPRESS = 18,

			[Description("Error during an image conversion")]
			IW_ERR_IMAGE_CONVERSION = 19,

			[Description("Error on an header item")]
			IW_ERR_HEADER_ITEM = 20,

			[Description("Unsupported bit depth")]
			IW_ERR_UNSUPPORTED_BIT_DEPTH = 21,

			[Description("The transaction failed verification")]
			IW_WARN_TRANSACTION_FAILED_VERIFICATION = 1000,

			[Description("Invalid field number")]
			IW_WARN_INVALID_FIELD_NUM = 1001,

			[Description("Warning - Required field missing")]
			IW_WARN_REQ_FIELD_MISSING = 1002,

			[Description("Warning - Incorrect item count")]
			IW_WARN_INCORRECT_ITEM_COUNT = 1003,

			[Description("Warning - Too few data characters")]
			IW_WARN_TOO_FEW_DATA_CHARS = 1004,

			[Description("Warning - Too many data characters")]
			IW_WARN_TOO_MANY_DATA_CHARS = 1005,

			[Description("Warning - Data not numeric")]
			IW_WARN_DATA_NOT_NUMERIC = 1006,

			[Description("Warning - Data not alpha")]
			IW_WARN_DATA_NOT_ALPHA = 1007,

			[Description("Warning - Data not alphanumeric")]
			IW_WARN_DATA_NOT_ALPHANUMERIC = 1008,

			[Description("Warning - Too few subfields")]
			IW_WARN_TOO_FEW_SUBFIELDS = 1009,

			[Description("Warning - Too many subfields")]
			IW_WARN_TOO_MANY_SUBFIELDS = 1010,

			[Description("Warning - Unsupported field present")]
			IW_WARN_UNSUPPORT_FIELD_PRESENT = 1011,

			[Description("Warning - Transaction failed XML parsing")]
			IW_WARN_TRANSACTION_FAILED_XML_PARSE = 1012,

			[Description("Warning - Data not numeric special")]
			IW_WARN_DATA_NOT_NUMERIC_SPECIAL = 1013,

			[Description("Warning - Data not alpha special")]
			IW_WARN_DATA_NOT_ALPHA_SPECIAL = 1014,

			[Description("Warning - Data not alphanumeric special")]
			IW_WARN_DATA_NOT_ALPHANUMERIC_SPECIAL = 1015,

			[Description("Warning - Invalid date")]
			IW_WARN_INVALID_DATE = 1016,

			[Description("Warning - Invalid data")]
			IW_WARN_INVALID_DATA = 1017,

			[Description("Warning - Data not printed")]
			IW_WARN_DATA_NOT_PRINT = 1018,

			[Description("Warning - Data not print control")]
			IW_WARN_DATA_NOT_PRINTCTRL = 1019,

			[Description("Warning - Request record missing")]
			IW_WARN_REQ_RECORD_MISSING = 1020,

			[Description("Warning - Unsupported record present")]
			IW_WARN_UNSUPPORT_RECORD_PRESENT = 1021,

			[Description("Warning - Unsupported TOT")]
			IW_WARN_UNKNOWN_TOT = 1022
		}	
	}
}
