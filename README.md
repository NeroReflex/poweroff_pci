# poweroff-pci
Power off pci devices on request.

## Usage

```
echo "10de 22bc" > /sys/bus/pci/drivers/poweroff-pci/new_id
echo "10de 2788" > /sys/bus/pci/drivers/poweroff-pci/new_id
```

then check the power state of the device (change 0000:01:00.0 with the correct bus address):

```
cat /sys/bus/pci/devices/0000:01:00.0/power_state 
```