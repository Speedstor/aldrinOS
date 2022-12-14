set OSNAME=CustomOS
set BUILDDIR=%0/../bin
set OVMFDIR=%0/../../OVMFbin

set BUILDDIR=%BUILDDIR:"=%
set OVMFDIR=%OVMFDIR:"=%
qemu-system-x86_64 -machine q35 ^
-drive file=%BUILDDIR%/%OSNAME%.img ^
-drive file=%BUILDDIR%/blank.img -m 256M ^
-cpu qemu64 ^
-drive if=pflash,format=raw,unit=0,file=%OVMFDIR%/OVMF_CODE-pure-efi.fd,readonly=on ^
-drive if=pflash,format=raw,unit=1,file=%OVMFDIR%/OVMF_VARS-pure-efi.fd -net none ^
-device usb-ehci,id=ehci                                    ^
-device usb-mouse,bus=ehci.0                            
pause
