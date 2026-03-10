Files overview
hd0-deb12.1.0-with-utils.zip
- hard drive image with Debian 12.1.0 amd64 minimal being installed plus some tools and compilers
qemu-x86_64.zip
- stripped qemu-w64-setup-20210825.exe installation having only qemu-system-x86_64 emulator
debian-12.1.0-amd64-netinst.iso
- minimal installer of Debian 12.1.0, requires internet connection
How to use
1. Download hard drive image you want to use.
Lets download hd0-deb12.1.0-with-utils.zip and unzip it. There is hd0.qcow2 file inside which is the disk image itself.
2. Download qemu installation you want to use. It is already exposed, so you don't need administrator priviledges to use it.
Lets download qemu-x86_64.zip which is much smaller than complete installation but has all the required contents. Unzip it.
3. Start qemu virtual machine using given hard drive image. Here is the example command line:
qemu-system-x86_64.exe -m 1024M -hda hd0.qcow2 -smp cores=4 -machine q35 -device e1000,netdev=net0 -netdev user,id=net0,net=192.168.76.0/24,dhcpstart=192.168.76.9,hostfwd=tcp::5555-:22
-m 1024M use 1GB RAM for VM, can be reduced
-hda hd0.qcow2 attach hd0.qcow2 hard drive image to the VM
-smp cores=4 emulate 1 CPU of 4 cores inside of the VM
-device e1000,netdev=net0 setup virtual network adapter based on e1000 emulation
-netdev ....,hostfwd=tcp::5555-:22 setup virtual network in user space so that 5555 tcp port of the local machine will be mapped to the 22 port of the VM
This command line gives you comfortable experience of isolated system having internet connection from your real computer. At the same time you can connect to the VM using SSH through your local 5555 port using Putty for example.
Multiple SSH connections are allowed, so you can use number of terminals at the same time.
4. Login for a system: user
Password: student
How to setup VM from scratch in a few words
1. Create new hard drive image:
qemu-img create -f qcow2 -o preallocation=off hd0.qcow2 50G
2. Use your preffered OS CD image as installation media during your first boot:
qemu-system-x86_64.exe -m 1024M -hda hd0.qcow2 -cdrom debian-12.1.0-amd64-netinst.iso -smp cores=4 -machine q35 -device e1000,netdev=net0 -netdev user,id=net0,net=192.168.76.0/24,dhcpstart=192.168.76.9,hostfwd=tcp::5555-:22
3. Install OS to the hard drive in the VM. Shut your VM down when the installation will be finished.
If the installer reboots you machine automatically, just terminate the emulator right after it goes to reboot.
4. Start your VM without installation media:
qemu-system-x86_64.exe -m 1024M -hda hd0.qcow2 -smp cores=4 -machine q35 -device e1000,netdev=net0 -netdev user,id=net0,net=192.168.76.0/24,dhcpstart=192.168.76.9,hostfwd=tcp::5555-:22
5. VM is ready to use.
If you ever want to preserve some state of the VM, you can always copy the hard drive image file. It can be reused at any time.
