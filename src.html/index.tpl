<!DOCTYPE html>
<html>
<head><title>IoT Light Switch</title>
<script type="text/javascript" src="index_1.js"></script>
<script type="text/javascript" src="index_2.js"></script>
<meta charset=UTF-8>
<link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
<div id="main">
<table width=auto align="center">
<tr ><td align="right" colspan="2"  ><a href ="/options/options.tpl">      <button name="options" class="options_btn">      Налаштування           </button> </a></td></tr>
<tr>
<td colspan="2" align="center">
<h1><canvas id="myCanvas" width="23" height="23"></canvas>
<script type="text/javascript" src="index_3.js"></script>
ESP_IoT_04001 Світло над вікном у вітальні
</h1>
</td>
</tr>
<p>
<ul>
<tr><td align="center" colspan="2" >Поточний час %curTime%    </td></tr>
<tr><td align="center" colspan="2" >%working_hr%    </td></tr>
<tr><td align="center" colspan="2">Температура: %Temperature%, вологість:%Humidity% </td></tr>
<tr>
<td align="right">
<div class="onoffswitch1">
<input type="checkbox" class="onoffswitch1-checkbox" id="slider1" name="slider1" onchange="slider1(this.id)" %checker_1_status% >
<label class="onoffswitch1-label" for="slider1">
<span class="onoffswitch1-inner"></span>
<span class="onoffswitch1-switch"></span>
</label>
</div>
</td>
<td align="left">
<font size="5" class="switch_text">%checker_1_text%</font>
</td>
</tr>
<tr>
<td align="right">
<div class="onoffswitch2">
<input type="checkbox" class="onoffswitch2-checkbox" id="slider2" name="slider2" onchange="slider2(this.id)" %checker_2_status% >
<label class="onoffswitch2-label" for="slider2">
<span class="onoffswitch2-inner"></span>
<span class="onoffswitch2-switch"></span>
</label>
</div>
</td>
<td align="left">
<font size="5" class="switch_text">%checker_2_text%</font>
</td>
</tr>
<table>
</ul>
</p>
<form method="POST" action="index.cgi" id="checked1">
<input type="hidden" id="slider1" name="slider1" value="1" />
</form>
<form method="POST" action="index.cgi" id="unchecked1">
<input type="hidden" id="slider1" name="slider1" value="0" />
</form>
<form method="POST" action="index.cgi" id="checked2">
<input type="hidden" id="slider2" name="slider2" value="1" />
</form>
<form method="POST" action="index.cgi" id="unchecked2">
<input type="hidden" id="slider2" name="slider2" value="0" />
</form>
</div>
</body></html>