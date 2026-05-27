if [ -f "disk.img" ]; then
    rm disk.img
fi

read -p "Disk Size (MiB) (At least 300MB to store Bad Apple): " size

if [ "$size" -lt 300 ]; then
    echo "Disk size too small, clamping to 300 MiB"
    size=300
fi

dd if=/dev/zero of=disk.img bs=1M count=$size