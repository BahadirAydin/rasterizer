import os
import subprocess

# Function to find all XML files in a directory and its subdirectories
def find_xml_files(directory):
    xml_files = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".xml"):
                xml_files.append(os.path.join(root, file))
    return xml_files

# Path to the directory containing the io directory and your rasterizer executable
executable_directory = "/home/bahadir/BahadirAydin/Akademi/CENG477/hw2/rasterizer/"

# Path to the io directory
io_directory = os.path.join(executable_directory, "io")

# Path to the rasterizer executable
rasterizer_executable = os.path.join(executable_directory, "rasterizer")

# Find all XML files in the io directory and its subdirectories
xml_files = find_xml_files(io_directory)

# Run the rasterizer executable for each XML file
for xml_file in xml_files:

    # Run the rasterizer executable
    subprocess.run([rasterizer_executable, xml_file])

print("Rasterization complete.")

