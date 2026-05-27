./mkdisk.sh
./build.sh

echo "Running os to format disk"
./run.sh

clear
echo "Mounting disk to upload Bad Apple. (This requires root)"
sudo ./mntdisk.sh

echo "Downloading Bad Apple"
sleep 1
yt-dlp -o badapple.mp4 --merge-output-format mp4 https://www.youtube.com/watch?v=9lNZ_Rnr7Jc

echo "Converting Bad Apple to bin"
python3 video_to_bin.py badapple.mp4 badapple.bin

echo "Moving Bad Apple bin to disk"
sudo mv badapple.bin /mnt/daos/

echo "Cleaning up Bad Apple"
rm badapple.mp4

echo "Unmounting disk"
sudo umount /mnt/daos

echo "Running OS"
./run.sh