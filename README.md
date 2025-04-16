# poweroff_pci
Power off pci devices on request.

## Usage

Load the module:
```sh
modprobe poweroff_pci
```

```sh
# bind the audio device first
echo "10de 22bc" > /sys/bus/pci/drivers/poweroff_pci/new_id
# and then the gpu
echo "10de 2788" > /sys/bus/pci/drivers/poweroff_pci/new_id
```

then check the power state of the device (change 0000:01:00.0 with the correct bus address):

```sh
cat /sys/bus/pci/devices/0000:01:00.0/power_state 
```

## Tricks

When device is powered off you can remove it from the kernel:

```sh
echo 1 > /sys/bus/pci/devices/0000:01:00.0/remove
```

a pci rescan will bring it up again later in case you need to reuse it.
