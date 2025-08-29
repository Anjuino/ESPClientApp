Import("env")
import hashlib
import os

def add_md5_to_firmware_footer(source, target, env):

    firmware_path = target[0].path
    print(f"Добавляем MD5: {firmware_path}")
    
    # Читаем собранную прошивку
    with open(firmware_path, 'rb') as f:
        firmware_data = f.read()
    
    # Вычисляем MD5 оригинальной прошивки
    md5_hash = hashlib.md5(firmware_data).hexdigest()

    # Записываем: оригинальная прошивка + MD5
    with open(firmware_path, 'wb') as f:
        f.write(firmware_data)             # оригинальная прошивка
        f.write(md5_hash.encode('utf-8'))  # 32 байта MD5 в конце

    print(f"Файл обновлен: {firmware_path}")

# Добавляем хук после сборки
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", add_md5_to_firmware_footer)