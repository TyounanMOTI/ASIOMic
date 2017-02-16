using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Text;

public class MicLoopback : MonoBehaviour {
	ASIOMic.Library.DebugLogDelegate debugLogFunc = message => Debug.Log(message);

	// Use this for initialization
	void Start()
	{
		ASIOMic.Library.SetDebugLogFunc(debugLogFunc);
		ASIOMic.Library.Initialize("ASIO4ALL v2");
		Debug.Log("Input Latency: " + ASIOMic.Library.GetInputLatency() / ASIOMic.Library.GetSampleRate());
		Debug.Log("Output Latency: " + ASIOMic.Library.GetOutputLatency() / ASIOMic.Library.GetSampleRate());

		var num_channels = ASIOMic.Library.GetNumInputChannels();
		StringBuilder[] channelNames = new StringBuilder[num_channels];
		for (int i = 0; i < num_channels; ++i) {
			channelNames[i] = new StringBuilder(32);
		}
		ASIOMic.Library.GetInputChannelNames(channelNames);
		foreach (var name in channelNames) {
			Debug.Log("Input: " + name);
		}

		num_channels = ASIOMic.Library.GetNumOutputChannels();
		channelNames = new StringBuilder[num_channels];
		for (int i = 0; i < num_channels; ++i) {
			channelNames[i] = new StringBuilder(32);
		}
		ASIOMic.Library.GetOutputChannelNames(channelNames);
		foreach (var name in channelNames) {
			Debug.Log("Output: " + name);
		}
		
		ASIOMic.Library.SetInputSendLevel(0, 0, 1.0);
		ASIOMic.Library.SetInputSendLevel(0, 1, 1.0);
		ASIOMic.Library.StartLoopback();
	}

	// Update is called once per frame
	void Update () {
		
	}

	private void OnApplicationQuit()
	{
		ASIOMic.Library.StopLoopback();
	}
}
