dd if=src/boot/boot.bin of=dthas.img bs=1 count=446 conv=notrunc
dd if=src/boot/boot.bin of=dthas.img seek=510 skip=510 bs=1 count=2 conv=notrunc

sudo bochs -f bochsrc_hd
