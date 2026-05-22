if [ -f "disk.img" ]; then
    rm disk.img
fi

read -p "Disk Size (MiB): " size

dd if=/dev/zero of=disk.img bs=1M count=$size