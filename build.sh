if [ -d "build" ]; then
    rm -rf build
fi

mkdir build
mkdir build/isodir
mkdir -p build/isodir/boot/grub

shopt -s globstar

for file in src/**/*.s; do
    name=$(basename "$file" .s)

    i686-elf-as -c "$file" -o "build/$name.o"
done

for file in src/**/*.S; do
    name=$(basename "$file" .S)

    i686-elf-gcc -c "$file" -o "build/$name.o" -ffreestanding -O2 -Wall -Wextra
done

for file in libc/**/*.c; do
    name=$(basename "$file" .c)

    i686-elf-gcc -c "$file" -o "build/$name.o" -Isrc/include -Ilibc/include -std=gnu99 -ffreestanding -O2 -Wall -Wextra
done

for file in src/**/*.c; do
    name=$(basename "$file" .c)

    i686-elf-gcc -c "$file" -o "build/$name.o" -Isrc/include -Ilibc/include -std=gnu99 -ffreestanding -O2 -Wall -Wextra
done

i686-elf-gcc -T linker.ld -o build/daos -ffreestanding -O2 -nostdlib build/boot.o $(find build -name '*.o' ! -name 'boot.o') -lgcc

cp build/daos build/isodir/boot/daos
cp grub.cfg build/isodir/boot/grub/grub.cfg

grub-mkrescue -o build/daos.iso build/isodir