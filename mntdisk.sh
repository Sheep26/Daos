if [ ! -d "/mnt/daos" ]; then
    mkdir /mnt/daos
fi

mount -o loop disk.img /mnt/daos