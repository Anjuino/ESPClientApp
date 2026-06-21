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
    
    <hr class="ota-divider">
    <br>
    <h1>Обновление прошивки</h1>
    <div class="ota-section">
      <input type="file" id="otaFile" accept=".bin" style="font-size: 33px;">
      <div id="otaStatus"></div>
    </div>
    <button type="button" class="ota-button" onclick="updateFirmware()">Обновить</button>

    <hr class="ota-divider">
    <br>
    <button type="button" onclick="Restart()">Перезагрузка</button>
  </div>
  <script>
    document.addEventListener('DOMContentLoaded', function() {
      ScanWifi()
        .then(() => {
          return Promise.all([
            GetWifiData(),
            GetToken(),
            GetIp()
          ]);
        })
        .catch(() => {})
        .finally(() => {
          document.getElementById('loading').style.display = 'none';
          document.getElementById('content').style.display = 'block';
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
            // Добавляем уровень сигнала в скобках
            option.textContent = `${network.SSID} (${network.RSSI} dBm)`;
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
            const options = Array.from(ssid.options).map(opt => opt.value);
            const networkExists = options.includes(data.SSID);
            
            if (ssid.options.length === 0) {
              ssid.style.display = "none";
              manualSsid.style.display = "block";
              manualSsid.value = data.SSID;
            } else if (!networkExists) {
              const option = document.createElement("option");
              option.value = data.SSID;
              option.textContent = `${data.SSID} (текущая)`;
              ssid.appendChild(option);
              ssid.value = data.SSID;
              ssid.style.display = "block";
              manualSsid.style.display = "none";
            } else {
              ssid.value = data.SSID;
              ssid.style.display = "block";
              manualSsid.style.display = "none";
            }
          }
          password.value = data.Password || '';
        })
        .catch(error => {
          console.error('GetWifiData ошибка:', error);
        });
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

    function updateFirmware() {
      const fileInput = document.getElementById('otaFile');
      const statusDiv = document.getElementById('otaStatus');
      
      if (!fileInput.files || fileInput.files.length === 0) {
        statusDiv.textContent = 'Выберите файл прошивки';
        statusDiv.style.color = 'red';
        return;
      }
      
      const formData = new FormData();
      formData.append('firmware', fileInput.files[0]);
      
      statusDiv.textContent = 'Загрузка...';
      statusDiv.style.color = 'blue';
      
      fetch('/update', {
        method: 'POST',
        body: formData
      })
      .then(response => {
        if (response.ok) {
          statusDiv.textContent = 'Обновление успешно! Перезагрузка...';
          statusDiv.style.color = 'green';
          setTimeout(() => location.reload(), 5000);
        } else {
          statusDiv.textContent = 'Ошибка обновления';
          statusDiv.style.color = 'red';
        }
      })
      .catch(error => {
        statusDiv.textContent = 'Ошибка: ' + error.message;
        statusDiv.style.color = 'red';
      });
    }

    function Restart() {
      fetch('/restart')
      .then(response => {
          if (response.ok) {
              alert("Устройство будет перезагружено");
              setTimeout(() => location.reload(), 5000);
          } else {
              alert("Ошибка перезагрузки");
          }
      })
      .catch(error => {
          alert("Ошибка: " + error.message);
      });
    }
    
  </script>
</body>
</html>
)=";