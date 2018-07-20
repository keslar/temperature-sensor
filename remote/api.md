# Restful API for Remote Sensors

**/sensor**

Returns a list of all sensors attached to the device, and their current reading.

*/sensor/{id}*

Returns the current sensor reading

*/sensor/{id}/{param}*

Displays the parameter for a sensor.

*/sensor/{id}/{param}/set*

Set the parameter for the sensor



**/authentication**

Returns whether supplied authentication key is valid.

*/authentication/login*

Returns and authentication key, if proper credentials are supplied



**/configuration**

Returns the list of configuration parameters and current values.  Privileged values are omitted unless authentication.

*/configuration/{param}*

Returns the current value of the configuration parameter

*/configuration/{param}/set*

Requires authentication, set the parameter value

*/configuration/{param}/default*

Require authentication, set the parameter to the default value.





# Temperature and Humidity Sensor

/sensor

​	returns json.

{ "sensors": {

​	"serial-number":"<serial number for device>",

​	"sensor-info":{

​				"sensor":{

​						"id":1,

​						"name":"<temperature|humidity>",

​						"value":<sensor reading>

​				},

​				"sensor":{

​						"id":2,

​						"name":"<temperature|humidity>",

​						"value":<sensor reading>

​				}

​	}

}





*/sensor/{1|2}*

​	returns json.

​	{	"sensor-info":{

​				"sensor":{

​						"id":<1|2>,

​						"name":"<temperature|humidity>",

​						"value":<sensor reading>

​				}

​	}



*/sensor/{id}/{param}*

​	there are currently no parameters available for the sensors in this module

