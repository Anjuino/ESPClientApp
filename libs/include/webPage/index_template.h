const char Index_template[] PROGMEM = R"=(
<html>
<head>
  <meta charset="utf-8">
  <title>=====</title>
  <link rel="stylesheet" href="styles.css">  
</head>
<body onload>
  <div id="loading">Загрузка данных...</div>
    <div id="content">
    <h1>Настройка Wi-Fi</h1>
    <form id="wifiForm">
    <label for="ssid">Сеть:</label><br>
    <select id="ssid" name="ssid"></select><br><br>
    <label for="password">Пароль:</label><br>
    <input type="password" id="password" name="password"><br><br>
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
      Promise.all([GetWifiData(), ScanWifi()])
        .then(() => {
          document.getElementById('loading').style.display = 'none';
          document.getElementById('content').style.display = 'block';
        })
        .catch(error => {
          console.error('Ошибка при загрузке данных:', error);
          document.getElementById('loading').textContent = 
            'Ошибка загрузки. Пожалуйста, обновите страницу.';
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
        select.innerHTML = ""; // Очистить существующие опции

        data.forEach(network => {
            const option = document.createElement("option");
            option.value = network.SSID;
            option.text  = network.SSID;
            select.appendChild(option);
        });
      })
    }

    function GetWifiData() {
      const ssid = document.getElementById('ssid');
      const password = document.getElementById('password');
      
      return fetch(`/GetWifiData`)
      .then(response => {
        if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
        return response.json(); // Жду JSON
      })
      .then(data => {
        ssid.value = data.SSID;
        password.value = data.Password;
      })
    }
        
    function configureWifi() {
      const ssid = document.getElementById('ssid').value;
      const password = document.getElementById('password').value;
      const token = document.getElementById('Token').value;
      const users = document.getElementById('Users').value;

      var IsDataInput = true;

      if (!ssid) {
        IsDataInput = false;
        alert("Введите имя сети");
      }

      if (!password) {
        IsDataInput = false;
        alert("Введите пароль")
      }

      if (IsDataInput) fetch(`/RegDeviceData?SSID=${ssid}&Password=${password}`);
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