const char CONFIG_PAGE[] PROGMEM = R"=====(
<HTML>
 <HEAD>
   <TITLE>No WiFi POC</TITLE>
 </HEAD>
 <BODY>
  <form action="/settings" method="get">
   %s<br><br>
   %s<br><br>
   <input type="submit" value="Save" style='width: 150px;'>
   &nbsp;&nbsp;&nbsp;
   <a href="/restart">
    <button type="button" style='width: 150px;'>Restart</button>
   </a>
  </form>
 </BODY>
</HTML>
)=====";

const char GET_JSON[] PROGMEM = R"=====({
 "rssi":%d,
 "heat_power": %.0f,
 "heat_power_raw": %.0f,
 "flow": %.1f,
 "return_temp": %d,
 "supply_temp": %d,
 "outdoor_temp": %d,
 "set_temp": %d,
 "dhw_set_temp": %d,
 "in_temp": %.2f,
 "out_temp": %.2f,
 "indoor_temp": %.1f,
 "state": "%s",
 "ODU_running": %s
})=====";
