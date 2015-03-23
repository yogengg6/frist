using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OpenEBTSNet
{
	/// <summary>
	/// One field value descriptor for defining an optional or mandatory field value
	/// </summary>
	public class OpenEBTSFieldValue
	{

		/// <summary>
		/// Name of value, e.g., &quot;BLK&quot;
		/// </summary>
		public string Name { get; private set; }

		/// <summary>
		/// Description of value, e.g., &quot;Black&quot;
		/// </summary>
		public string Description { get; private set; }

		internal OpenEBTSFieldValue(string name, string description)
		{
			Name = name;
			Description = description;
		}
	}
}
