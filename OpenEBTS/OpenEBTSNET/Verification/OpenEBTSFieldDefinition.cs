using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OpenEBTSNet
{
	/// <summary>
	/// Detailed NIST Field Definition
	/// </summary>
	public class OpenEBTSFieldDefinition
	{
		/// <summary>
		/// Any additional comments this Field Definition contains
		/// </summary>
		public string AdditionalComments { get; internal set; }

		/// <summary>
		/// Advanced rule, e.g., 'supportedif(&quot;T10_IMT&quot;)=&quot;FACE&quot;'
		/// </summary>
		public string AdvancedRule { get; internal set; }

		/// <summary>
		/// Can data contain alphabetic characters?
		/// </summary>
		public bool DataTypeA { get; internal set; }

		/// <summary>
		/// Is data field binary?
		/// </summary>
		public bool DataTypeB { get; internal set; }

		/// <summary>
		/// Can data contain control characters?
		/// </summary>
		public bool DataTypeC { get; internal set; }

		/// <summary>
		/// Can data contain numeric characters?
		/// </summary>
		public bool DataTypeN { get; internal set; }

		/// <summary>
		/// Can data contain any pritable characters?
		/// </summary>
		public bool DataTypeP { get; internal set; }

		/// <summary>
		/// Does data consist of a set of multiple fields?
		/// </summary>
		public bool DataTypeSET { get; internal set; }

		/// <summary>
		/// Describes the format of a date field, e.g., &quot;ZCCYYMMDD&quot;
		/// </summary>
		/// <seealso cref="IsDate"/>
		public string DateFormat { get; internal set; }

		/// <summary>
		/// Short description of Field Definition
		/// </summary>
		public string Description { get; internal set; }

		/// <summary>
		/// Field index as defined by the standard, e.g., 18 for &quot;2.018&quot;
		/// </summary>
		public int FieldIndex { get; internal set; }

		/// <summary>
		/// Maximum legnth of field
		/// </summary>
		/// <remarks> A value of -1 indicates there is no specified maximum length</remarks>
		public int FieldSizeMax { get; internal set; }

		/// <summary>
		/// Minimu length of field
		/// </summary>
		public int FieldSizeMin { get; internal set; }

		/// <summary>
		/// Field Definition identifier, e.g., &quot;NAM&quot;
		/// </summary>
		public string Identifier { get; internal set; }

		/// <summary>
		/// If true, the field is automatically managed by OpenEBTS
		/// </summary>
		/// <remarks>Many of the image related fields are set based on data within the images provided.</remarks>
		public bool IsAutomaticallySet { get; internal set; }

		/// <summary>
		/// Is this a date field?
		/// </summary>
		/// <seealso cref="DateFormat"/>
		public bool IsDate { get; internal set; }

		/// <summary>
		/// If true, this field is mandatory in a NIST Transaction File for this Transaction Type. If 
		/// false the field is optional.
		/// </summary>
		public bool IsMandatory { get; internal set; }

		/// <summary>
		/// Item's index as defined by the standard, e.g., 3 for 10.042..3
		/// </summary>
		public int ItemIndex { get; internal set; }

		/// <summary>
		/// Possible list of items defined as part of this field
		/// </summary>
		/// <remarks>
		/// If this value if null, there are no fields inside this field, otherwise the returned 
		/// Field Definitions represent the fields within this field.
		/// </remarks>
		public OpenEBTSFieldDefinitions ItemList { get; internal set; }

		/// <summary>
		/// Full location index as specified by the Verification File, e.g., &quot;10.042..3&quot;
		/// </summary>
		public string LocationIndex { get; internal set; }

		/// <summary>
		/// Field's mnemonic, e.g., &quot;NAM&quot;
		/// </summary>
		public string Mnemonic { get; internal set; }

		/// <summary>
		/// Field's mnemonic, e.g., &quot;NAME&quot;
		/// </summary>
		public string Name { get; internal set; }

		/// <summary>
		/// Maximum times this field may occurr
		/// </summary>
		/// <remarks>An occurrence figure of -1 implies there is no maximum</remarks>
		public int OccurrencesMax { get; internal set; }

		/// <summary>
		/// Minimum times this field may occurr
		/// </summary>
		public int OccurrencesMin { get; internal set; }

		/// <summary>
		/// Record Type of record this field belongs to
		/// </summary>
		public int RecordType { get; internal set; }

		/// <summary>
		/// Special charachters allowed for this field's data, e.g., &quot;;-&quot;
		/// </summary>
		public string SpecialChars { get; internal set; }

		/// <summary>
		/// Tag number as defined in the Verification File, e.g., &quot;2.018&quot;
		/// </summary>
		public string TagNumber { get; internal set; }

		/// <summary>
		/// Possible optional or mandatory list of values for this field
		/// </summary>
		public OpenEBTSFieldValues Values { get; internal set; }

		/// <summary>
		/// Possible optional or mandatory filename holding the list of values for this field
		/// </summary>
		public string ValuesFile { get; internal set; }
	}
}
