Import("env")
import shutil
import os

print("Starting library sync...")

libs_path = "C:/ESPClientApp/libs"
project_path = env["PROJECT_DIR"]

# Копируем include - ПЕРЕЗАПИСЫВАЕМ без удаления
include_src = os.path.join(libs_path, "include")
include_dst = os.path.join(project_path, "include")

# Просто копируем всё, перезаписывая существующие файлы
if os.path.exists(include_src):
    if not os.path.exists(include_dst):
        os.makedirs(include_dst)
    
    for item in os.listdir(include_src):
        src_item = os.path.join(include_src, item)
        dst_item = os.path.join(include_dst, item)
        
        if os.path.isfile(src_item):
            shutil.copy2(src_item, dst_item)
        else:
            if os.path.exists(dst_item):
                shutil.rmtree(dst_item)
            shutil.copytree(src_item, dst_item)

print("Copied include files")

# Копируем src - ПЕРЕЗАПИСЫВАЕМ без удаления основного src
src_src = os.path.join(libs_path, "src") 
src_dst = os.path.join(project_path, "src")

for item in os.listdir(src_src):
    src_item = os.path.join(src_src, item)
    dst_item = os.path.join(src_dst, item)
    
    # Только перезаписываем файлы, не трогаем существующие
    if os.path.isfile(src_item):
        shutil.copy2(src_item, dst_item)
    else:
        if os.path.exists(dst_item):
            shutil.rmtree(dst_item)
        shutil.copytree(src_item, dst_item)

print("Libraries synced successfully!")