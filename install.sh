argv=("$@")

if [ -f "badapple.bin" ]; then
    rm badapple.bin
fi

if [ -f "badapple.mp4" ]; then
    rm badapple.mp4
fi

./mkdisk.sh
#./build.sh

clear
echo "Running os to format disk"
sleep 1
./run.sh "${argv[0]}"

clear
echo "Mounting disk to upload Bad Apple. (This requires root)"

if command -v sudo &> /dev/null; then
    sudo ./mntdisk.sh
else
    ./mntdisk.sh
fi

echo "Downloading Bad Apple"
sleep 1
yt-dlp -o badapple.mp4 --merge-output-format mp4 https://www.youtube.com/watch?v=9lNZ_Rnr7Jc

echo "Converting Bad Apple to bin"
python3 video_to_bin.py badapple.mp4 badapple.bin

echo "Moving Bad Apple bin to disk"
if command -v sudo &> /dev/null; then
    sudo mv badapple.bin /mnt/daos/
else
    mv badapple.bin /mnt/daos/
fi

echo "Cleaning up Bad Apple"
rm badapple.mp4

echo "Unmounting disk"
if command -v sudo &> /dev/null; then
    sudo umount /mnt/daos
else
    umount /mnt/daos
fi