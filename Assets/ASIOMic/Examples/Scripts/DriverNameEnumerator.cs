using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Text;

public class DriverNameEnumerator : MonoBehaviour {

	// Use this for initialization
	void Start () {
		var length = ASIOMic.Library.GetDriverNameMaxLength();
		const int maxDrivers = 8;
		StringBuilder[] driverNames = new StringBuilder[8];
		for (int i = 0; i < maxDrivers; ++i) {
			driverNames[i] = new StringBuilder(length);
		}
		ASIOMic.Library.GetDeviceList(driverNames, maxDrivers);
		foreach (var name in driverNames) {
			Debug.Log(name);
		}
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
