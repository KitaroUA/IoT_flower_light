<!DOCTYPE html>
<html>
	<head>
		<title>IoT Light Switch</title>
		<meta charset=UTF-8>
			<link rel="stylesheet" type="text/css" href="style.css">
			</head>
			<body>
				<div id="main">

					<h1>
						Налаштування
					</h1>

					<table width="800px">
						<tr>
							<td align="right" colspan="2">
								<a href="/index.tpl">
									<button name="go_back" class="options_btn">Повернутися</button>
								</a>
							</td>
						</tr>

						<tr>
							<td align="right" colspan="2">
								<a href ="/wifi/wifi.tpl">
									<button name="wifi" class="options_btn"> Налаштувати WiFi  </button>
								</a>
							</td>
					</tr>
					<tr>
						<td>
							<li>Поточний час %curTime%</li>  
						</td>
						<td align="right">    
							<a href ="/set_ntp/set_ntp.tpl">
								<button name="set_ntp" class="options_btn">      Налаштувати NTP           </button>
							</a> 
						</td>
					</tr>
					<tr>
						<td>
							<li>Час увімкнення %onTime%</li>
						</td>
						<td align="right">    
							<a href ="/set_on_time/set_on_time.tpl">
								<button name="set_on_time" class="options_btn">Встановити час увімкнення</button>
							</a> 
						</td>
					</tr>
					<tr>
						<td>
							<li>Час вимкнення %offTime%</li>   
						</td>
						<td align="right">    
							<a href ="/set_off_time/set_off_time.tpl">
								<button name="set_off_time" class="options_btn">Встановити час вимкнення</button>
							</a> 
						</td>
					</tr>
					<tr>
						<td>
							<li>Встановити час тимчасового вимкнення (наразі %v_tempOff%).</li>  
						</td>
						<td align="right">    
							<a href ="/set_temp_off/set_temp_off.tpl">
								<button name="set_temp_off" class="options_btn">Встановити час вимкнення</button>
							</a> 
						</td>
					</tr>
					<tr>
						<td >
							<li>Встановити час тимчасового увімкнення (наразі %v_tempOn%).</li>  
						</td>
						<td align="right">    
							<a href ="/set_temp_on/set_temp_on.tpl">
								<button name="set_temp_on" class="options_btn">Встановити час увімкнення</button>
							</a> 
						</td>
					</tr>
					<tr>
						<td>
							<li>Поріг освітлення %minLight%</li>         
						</td>
						<td align="right">    
							<a href ="/set_min_light/set_min_light.tpl">
								<button name="set_ntp" class="options_btn">Встановити новий поріг</button>
							</a> 
						</td>
					</tr>
				</table>

			</div>
		</body>
	</html>
	