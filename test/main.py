import os
import subprocess
import shutil


def find_xml_files(directory):
    xml_files = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".xml"):
                xml_files.append(os.path.join(root, file))
    return xml_files


executable_directory = "/home/bahadir/BahadirAydin/Akademi/CENG477/hw2/rasterizer/"

rasterizer_executable = os.path.join(executable_directory, "rasterizer")

dirs = ["culling_enabled_inputs", "culling_disabled_inputs"]

for dir in dirs:
    os.makedirs("outputs", exist_ok=True)
    io_directory = os.path.join(executable_directory, "io", dir)
    xml_files = find_xml_files(io_directory)
    for xml_file in xml_files:
        subprocess.run([rasterizer_executable, xml_file])
    shutil.copytree("outputs", dir , dirs_exist_ok=True)
    shutil.rmtree("outputs")

shutil.move("culling_enabled_inputs", "culling_enabled_outputs")
shutil.move("culling_disabled_inputs", "culling_disabled_outputs")


print("Rasterization complete.")
subprocess.run(["find", ".", "-name", "*.ppm", "-type", "f", "-delete"])
