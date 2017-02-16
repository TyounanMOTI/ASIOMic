using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Text;

namespace ASIOMic
{
	public class Library
	{
		public delegate void DebugLogDelegate([In, MarshalAs(UnmanagedType.LPWStr)]string message);

		[DllImport("ASIOMic", CharSet = CharSet.Unicode)]
		public static extern void SetDebugLogFunc(DebugLogDelegate func);

		[DllImport("ASIOMic")]
		public static extern int GetDriverNameMaxLength();

		[DllImport("ASIOMic")]
		public static extern void GetDeviceList(StringBuilder[] driverNames, long maxDrivers);

		[DllImport("ASIOMic")]
		public static extern void Initialize(string driverName);

		[DllImport("ASIOMic")]
		public static extern void StartLoopback();

		[DllImport("ASIOMic")]
		public static extern void StopLoopback();

		[DllImport("ASIOMic")]
		public static extern long GetInputLatency();

		[DllImport("ASIOMic")]
		public static extern long GetOutputLatency();

		[DllImport("ASIOMic")]
		public static extern void GetInputChannelNames(StringBuilder[] channelNames);

		[DllImport("ASIOMic")]
		public static extern void GetOutputChannelNames(StringBuilder[] channelNames);

		[DllImport("ASIOMic")]
		public static extern void SetInputSendLevel(int inputChannel, int outputChannel, double level);

		[DllImport("ASIOMic")]
		public static extern long GetNumInputChannels();

		[DllImport("ASIOMic")]
		public static extern long GetNumOutputChannels();

		[DllImport("ASIOMic")]
		public static extern double GetSampleRate();
	}
}