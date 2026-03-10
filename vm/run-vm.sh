qemu-system-x86_64 -m 1024M -hda hd0.qcow2 -smp cores=4 -machine q35 -device e1000,netdev=net0 -netdev user,id=net0,net=192.168.76.0/24,dhcpstart=192.168.76.9,hostfwd=tcp::5555-:22 $1
