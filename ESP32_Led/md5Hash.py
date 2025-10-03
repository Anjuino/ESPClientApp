Import("env")
import hashlib
import os

def Add_Md5_and_targetPlatform(source, target, env):
    firmware_path = target[0].path

    # Читаем собранную прошивку
    with open(firmware_path, 'rb') as f:
        firmware_data = f.read()
    
    # Вычисляем MD5 оригинальной прошивки
    md5_hash = hashlib.md5(firmware_data).hexdigest()
    
    # Определяем платформу
    platform = "ESP8266" if "esp8266" in env['PIOENV'] else "ESP32"
    
    # Фиксируем размер платформы
    platform_padded = platform.ljust(7).encode('utf-8')[:7]  # 7 байт для ESP8266
    
    # Формируем footer: MD5 + платформа
    footer = md5_hash.encode('utf-8') + platform_padded

    # Записываем: оригинальная прошивка + footer
    with open(firmware_path, 'wb') as f:
        f.write(firmware_data)    # оригинальная прошивка
        f.write(footer)           # 32 байта MD5 + 7 байт платформа

    print(f"Платформа: '{platform_padded.decode()}'")

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", Add_Md5_and_targetPlatform)