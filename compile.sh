rm -rf build
mkdir build
mkdir build/isodir
mkdir -p build/isodir/boot/grub

i686-elf-as src/boot.s -o build/boot.o
i686-elf-gcc -c src/kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -T linker.ld -o build/daos -ffreestanding -O2 -nostdlib build/*.o -lgcc

cp build/daos build/isodir/boot/daos
cp grub.cfg build/isodir/boot/grub/grub.cfg

grub-mkrescue -o build/daos.iso build/isodir