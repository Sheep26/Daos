if [ -f "disk.img" ]; then
    rm disk.img
fi

dd if=/dev/zero of=disk.img bs=1M count=128