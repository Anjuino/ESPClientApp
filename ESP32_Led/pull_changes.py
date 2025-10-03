import shutil
import os
import sys

def sync_libraries():
    print("Синхронизация с внешней библиотекой")
    
    # Пути (можно изменить под ваши нужды)
    libs_path = "C:/ESPClientApp/libs"
    project_path = os.getcwd()  # Текущая директория
    
    # Проверяем существование исходной директории с библиотеками
    if not os.path.exists(libs_path):
        print(f"Error: Library path not found: {libs_path}")
        return False
    
    # Копируем include - ПЕРЕЗАПИСЫВАЕМ без удаления
    include_src = os.path.join(libs_path, "include")
    include_dst = os.path.join(project_path, "include")
    
    print("Копирование include")

    if os.path.exists(include_src):
        if not os.path.exists(include_dst):
            os.makedirs(include_dst)
            print(f"Created directory: {include_dst}")
        
        for item in os.listdir(include_src):
            src_item = os.path.join(include_src, item)
            dst_item = os.path.join(include_dst, item)
            
            if os.path.isfile(src_item):
                shutil.copy2(src_item, dst_item)
                #print(f"Copied file: {item}")
            else:
                if os.path.exists(dst_item):
                    shutil.rmtree(dst_item)
                    #print(f"Removed directory: {item}")
                shutil.copytree(src_item, dst_item)
                print(f"Copied directory: {item}")
    
    print("Копирование src")
    
    # Копируем src - ПЕРЕЗАПИСЫВАЕМ без удаления основного src
    src_src = os.path.join(libs_path, "src")
    src_dst = os.path.join(project_path, "src")
    
    if os.path.exists(src_src):
        if not os.path.exists(src_dst):
            os.makedirs(src_dst)
            print(f"Created directory: {src_dst}")
        
        for item in os.listdir(src_src):
            src_item = os.path.join(src_src, item)
            dst_item = os.path.join(src_dst, item)
            
            if os.path.isfile(src_item):
                shutil.copy2(src_item, dst_item)
                #print(f"Copied file: {item}")
            else:
                if os.path.exists(dst_item):
                    shutil.rmtree(dst_item)
                    #print(f"Removed directory: {item}")
                shutil.copytree(src_item, dst_item)
                print(f"Copied directory: {item}")
    
    print("Синхронизация закончена")
    return True

if __name__ == "__main__":
    if len(sys.argv) > 1: libs_path = sys.argv[1]
    if len(sys.argv) > 2: project_path = sys.argv[2]
    
    sync_libraries()