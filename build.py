import subprocess
import sys
import os

source_dir = os.path.join("desktop")
build_dir = os.path.join("desktop", "build")

os.makedirs(build_dir, exist_ok=True)

print("-- Configuring...")
ret = subprocess.run(["cmake", "-S", source_dir, "-B", build_dir])
if ret.returncode != 0:
    sys.exit(ret.returncode)

print("-- Building...")
ret = subprocess.run(["cmake", "--build", build_dir])
sys.exit(ret.returncode)
