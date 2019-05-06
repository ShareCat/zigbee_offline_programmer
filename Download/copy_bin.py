#!/usr/bin/python3
 
# import shutil & os
import shutil,os

print("coping bin file...");

# copy the .bin file
shutil.copy(os.path.abspath(os.path.dirname(os.path.dirname(__file__)))+"\\Project\\RVMDK\\Output\\LHD8006_application.bin",os.path.abspath(os.path.dirname(__file__)))

print("copy done!");

# add "python ./../../../copy_bin.py" to MDK_keil
