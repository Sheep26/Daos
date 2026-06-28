# DaOS

![Hello World!!!](https://github.com/Sheep26/Daos/blob/main/assets/Screenshot_20260603_095311.png)
![Hello World!!!](https://github.com/Sheep26/Daos/blob/main/assets/Screenshot_20260603_094827.png)

## Features
Bad Apple  
Fat32 Filesystem  
Virtual Filesystem  
Terminal  
Help Page  

## How to run in QEMU.
Please have QEMU installed.
Please download both daos.iso and disk.img from releases  
Then run: ```qemu-system-i386 -cdrom daos.iso -m 512 -serial stdio -vga std -drive file=disk.img,format=raw,if=ide,cache=none -boot order=dc```  
Replace daos.iso and disk.img with the paths to the files you downloaded from github releases.  

## Prerequisites to build
Python  
&emsp;&emsp;- opencv (needed to make bad apple binary)  
&emsp;&emsp;- numpy (needed to make bad apple binary)  
&emsp;&emsp;- yt-dlp (needed to download bad apple)  
i686-elf-gcc  
i686-elf-ld  
i686-elf-as  

## Recomended Specs.
Ram: 512MiB (Could work with less but this was what I picked)  
Storage: 512MiB (could work with less but this was what I picked)  
