using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OpenEBTSNet
{
	internal struct OpenEBTSFieldRules
	{
		public int recordType;
		public int field;
		public int subfield;
		public int item;
		public string desc;
		public string longDesc;
		public string charType;
		public string specialChars;
		public string dateFormat;
		public string advancedRule;
		public int sizeMin;
		public int sizeMax;
		public int occMin;
		public int occMax;
		public int offset;
		public int automaticallySet;
		public int mandatory;
	}
}
