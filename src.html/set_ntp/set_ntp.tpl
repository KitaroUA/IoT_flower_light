<!DOCTYPE html>
<html>
	<head>
		<meta name="generator"     content="HTML Tidy for HTML5 (experimental) for Windows https://github.com/w3c/tidy-html5/tree/c63cc39" />
		<title>Test</title>
		<link rel="stylesheet" type="text/css" href="style.css" />
		<meta charset="UTF-8" />
	</head>
	<body>
		<div id="main">
	<table width="800px">
		<tr>
			<td align="right">
			   <p>
				<a href ="/index.tpl"><button name="go_back" class="options_btn">Повернутися</button> </a>
			  </p>
			</td>
		</tr>
	</table>
			<form method="post" action="set_ntp.cgi">
				<table>
					<tr>
						<th align="left">Host name:             <input name="hn" value="%v_hostname%" size="12" />
						</th>
						<th align="right">IP NTP:             <input name="ntpip" value="%v_ntp%" size="14" />
						</th>
					</tr>
					<tr>
						<th align="left">GMT zone:             <input value="%v_timezone%" name="timezone" size="3" />
						</th>
						<th align="left">Перехід на літній час:             <input type="radio" name="DST" value="1" %DST_1%/> Так             <input type="radio" name="DST" value="0" %DST_2%/> Ні</th>
						<th align="right">Формат NTP:             <input type="radio" name="ntp_type" value="0" %ntp_type_1%/> IP             <input type="radio" name="ntp_type"  value="1" %ntp_type_2%/> DNS</th>
					</tr>
					<tr>
						<th align="left">
							<button name="btn_time" class="options_btn">Застосувати</button>
						</th>
						<th align="right">%v_ntpresult%</th>
					</tr>
				</table> <a href="/"/>
			</form>
			<form method="post" action="apply_ntp.cgi">
				<table>
					<tr>
						<th align="left">
							<button name="btn_time" class="options_btn">Оновити час</button>
						</th>
					</tr>
				</table>
			</form>
		</div>
	</body>
</html> 