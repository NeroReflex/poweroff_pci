# poweroff-pci
Power off pci devices on request.

## Usage

```
echo "10de 22bc" > /sys/bus/pci/drivers/poweroff-pci/new_id
echo "10de 2788" > /sys/bus/pci/drivers/poweroff-pci/new_id
```