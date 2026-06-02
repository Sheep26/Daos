argv=("$@")

qemu-system-i386 -cdrom "${argv[0]}" -m 512 -serial stdio -vga std -drive file=disk.img,format=raw,if=ide,cache=none -boot order=dc