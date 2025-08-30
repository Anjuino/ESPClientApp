Import("env")
import re
import os

def increment_version(version_str):
    parts = version_str.split('.')
    if len(parts) == 4:
        try:
            last_part = int(parts[3])
            parts[3] = str(last_part + 1)
            return '.'.join(parts)
        except ValueError:
            pass
    return version_str

def update_build_version():
    file_path = "include/Config/build.h" 
    
    # Определяем платформу из переменных окружения
    platform = "ESP8266" if "esp8266" in env['PIOENV'] else "ESP32"

    if not os.path.exists(file_path):
        print(f"Файл не найден: {file_path}")
        return
    
    # Читаем содержимое файла
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Ищем версию для текущей платформы
    pattern = r'#ifdef\s+' + platform + r'.*?inline const char build\[\]\s*=\s*"([0-9.]+)";'
    match = re.search(pattern, content, re.DOTALL)
    
    if match:
        old_version = match.group(1)
        new_version = increment_version(old_version)
        
        # Заменяем версию
        new_content = content.replace(
            f'inline const char build[] = "{old_version}";',
            f'inline const char build[] = "{new_version}";'
        )
        
        # Записываем обратно
        with open(file_path, 'w') as f:
            f.write(new_content)
        
        print(f"Сборка: {new_version}")

update_build_version()