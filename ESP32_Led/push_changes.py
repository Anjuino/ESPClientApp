import shutil
import os
import sys

def push_to_repo():
    print("Добавление локальных файлов в общие файлы библиотек")
    
    project_path = "C:/ESPClientApp/ESP32ClientApp" 
    libs_path = "C:/ESPClientApp/libs"
    
    # Копируем изменения из include проекта в общий репозиторий
    include_src = os.path.join(project_path, "include")
    include_dst = os.path.join(libs_path, "include")
    
    if os.path.exists(include_src):
        for item in os.listdir(include_src):
            src_item = os.path.join(include_src, item)
            dst_item = os.path.join(include_dst, item)
            
            if os.path.isfile(src_item):
                shutil.copy2(src_item, dst_item)
                print(f"Updated: {item}")
            else:
                if os.path.exists(dst_item):
                    shutil.rmtree(dst_item)
                shutil.copytree(src_item, dst_item)
                print(f"Updated directory: {item}")
    
    # Копируем изменения из src проекта в общий репозиторий
    src_src = os.path.join(project_path, "src")
    src_dst = os.path.join(libs_path, "src")
    
    for item in os.listdir(src_src):
        src_item = os.path.join(src_src, item)
        dst_item = os.path.join(src_dst, item)
        
        # Копируем только файлы библиотек (исключаем main.cpp и т.д.)
        if item not in ["main.cpp", "main.h"] and os.path.isfile(src_item):
            shutil.copy2(src_item, dst_item)
            print(f"Updated: {item}")
        elif os.path.isdir(src_item) and item not in ["main"]:
            if os.path.exists(dst_item):
                shutil.rmtree(dst_item)
            shutil.copytree(src_item, dst_item)
            print(f"Updated directory: {item}")
    
if __name__ == "__main__":
    push_to_repo()