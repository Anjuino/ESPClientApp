const char Index[] PROGMEM = R"=(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Настройки</title>
  <link rel="stylesheet" href="styles.css">  
</head>
<body onload>
	<div id="loading">Загрузка данных...</div>
    <div id="content">
		<h1>Настройка Wi-Fi</h1>
		<form id="wifiForm">
      <label for="ssid">Сеть:</label><br>
      <select id="ssid" name="ssid" style="display: none;"></select>
      <input type="text" id="manualSsid" name="manualSsid" style="display: none;" placeholder="Введите SSID вручную"><br><br>
      <label for="password">Пароль:</label><br>
      <input type="password" id="password" name="password"><br><br>
		</form>
    <h1>Настройка подключения к серверу</h1>
    <form id="TgBotForm">
      <label for="Token">Токен:</label><br>
      <input type="text" id="Token" name="Token"><br><br>
      <label for="Ip">Ip:</label><br>
      <input type="text" id="Ip" name="Ip"><br><br>
    </form>
		<button type="button" onclick="configureWifi()">Сохранить</button>
	</div>		
  <script>
    document.addEventListener('DOMContentLoaded', function() {
      Promise.all([GetWifiData(), ScanWifi(), GetToken(), GetIp()])
        .then(() => {
          document.getElementById('loading').style.display = 'none';
          document.getElementById('content').style.display = 'block';
        })
        .catch(error => {
          console.error('Ошибка при загрузке данных:', error);
          document.getElementById('loading').textContent = 
            'Ошибка загрузки';
        });
    });
    
    function ScanWifi() {
      return fetch(`/ScanWifi`)
      .then(response => {
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.json();
      })
      .then(data => {
        const select = document.getElementById("ssid");
        const manualInput = document.getElementById("manualSsid");
        select.innerHTML = "";

        if (data && data.length > 0) {
          data.forEach(network => {
            const option = document.createElement("option");
            option.value = network.SSID;
            option.textContent = network.SSID;
            select.appendChild(option);
          });
          select.style.display = "block";
          manualInput.style.display = "none";
        } else {
          select.style.display = "none";
          manualInput.style.display = "block";
          manualInput.focus();
        }
      })
    }

    function GetWifiData() {
      const ssid = document.getElementById('ssid');
      const password = document.getElementById('password');
      const manualSsid = document.getElementById('manualSsid');
      
      return fetch(`/GetWifiData`)
      .then(response => {
        if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
        return response.json();
      })
      .then(data => {
        if (data.SSID) {
          if (ssid.style.display !== "none") {
            ssid.value = data.SSID;
          } else {
            manualSsid.value = data.SSID;
          }
        }
        password.value = data.Password;
      })
    }
	
    function GetToken() {
      const token = document.getElementById('Token');
      
      return fetch(`/GetToken`)
      .then(response => {
        if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
        return response.json();
      })
      .then(data => {
        token.value = data.Token;
      })
    }

    function GetIp() {
      const Ip = document.getElementById('Ip');
      
      return fetch(`/GetIpServer`)
      .then(response => {
        if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
        return response.json();
      })
      .then(data => {
        Ip.value = data.Ip;
      })
    }

    function configureWifi() {
      let ssid;
      const manualSsid = document.getElementById('manualSsid');
      
      if (manualSsid.style.display !== "none") {
        ssid = manualSsid.value;
      } else {
        ssid = document.getElementById('ssid').value;
      }

      const password = document.getElementById('password').value;
      const token = document.getElementById('Token').value;
      const ip = document.getElementById('Ip').value;

      var IsDataInput = true;

      if (!ssid) {
        IsDataInput = false;
        alert("Введите имя сети");
      }

      if (!password) {
        IsDataInput = false;
        alert("Введите пароль")
      }

      if (!token) {
        IsDataInput = false;
        alert("Введите токен")
      }

      if (!ip) {
        IsDataInput = false;
        alert("Введите ip")
      }

      if (IsDataInput) fetch(`/RegDeviceData?SSID=${ssid}&Password=${password}&Token=${token}&Ip=${ip}`);
    }
  </script>
</body>
</html>
)=";