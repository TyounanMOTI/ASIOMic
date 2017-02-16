using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DriverInitializer : MonoBehaviour {
	ASIOMic.Library.DebugLogDelegate debugLogFunc = message => Debug.Log(message);

	// Use this for initialization
	void Start () {
		ASIOMic.Library.SetDebugLogFunc(debugLogFunc);
		ASIOMic.Library.Initialize("ASIO4ALL v2");
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
