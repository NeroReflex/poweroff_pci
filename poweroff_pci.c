// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2021, NVIDIA CORPORATION & AFFILIATES. All rights reserved
 *
 * Copyright (C) 2012 Red Hat, Inc.  All rights reserved.
 *     Author: Alex Williamson <alex.williamson@redhat.com>
 *
 * Derived from original vfio:
 * Copyright 2010 Cisco Systems, Inc.  All rights reserved.
 * Author: Tom Lyon, pugs@cisco.com
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/device.h>
#include <linux/eventfd.h>
#include <linux/file.h>
#include <linux/interrupt.h>
#include <linux/iommu.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/notifier.h>
#include <linux/pci.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#define DRIVER_AUTHOR   "Alex Williamson <alex.williamson@redhat.com>"
#define DRIVER_DESC     "VFIO PCI - User Level meta-driver"

static char ids[1024] __initdata;
module_param_string(ids, ids, sizeof(ids), 0);
MODULE_PARM_DESC(ids, "Initial PCI IDs to add to the poweroff_pci driver, format is \"vendor:device[:subvendor[:subdevice[:class[:class_mask]]]]\" and multiple comma separated entries can be specified");

static int poweroff_pci_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	int ret;

	//dev_set_drvdata(&pdev->dev, vdev);
	ret = pci_set_power_state(pdev, PCI_D3cold);
	if (ret) {
		dev_err(&pdev->dev, "error setting pci powerstate to D3cold: %d", ret);
		goto out_put_vdev;
	} else {
		dev_info(&pdev->dev, "pci powerstate set to D3cold");
	}

	return 0;

out_put_vdev:
	return ret;
}

static void poweroff_pci_remove(struct pci_dev *pdev)
{
	//struct poweroff_pci_core_device *vdev = dev_get_drvdata(&pdev->dev);

}

static const struct pci_device_id poweroff_pci_table[] = {
	{ PCI_DRIVER_OVERRIDE_DEVICE_VFIO(PCI_ANY_ID, PCI_ANY_ID) }, /* match all by default */
	{}
};

MODULE_DEVICE_TABLE(pci, poweroff_pci_table);

static struct pci_driver poweroff_pci_driver = {
	.name			= "poweroff-pci",
	.id_table		= poweroff_pci_table,
	.probe			= poweroff_pci_probe,
	.remove			= poweroff_pci_remove,
};

static void __init poweroff_pci_fill_ids(void)
{
	char *p, *id;
	int rc;

	/* no ids passed actually */
	if (ids[0] == '\0')
		return;

	/* add ids specified in the module parameter */
	p = ids;
	while ((id = strsep(&p, ","))) {
		unsigned int vendor, device, subvendor = PCI_ANY_ID,
			subdevice = PCI_ANY_ID, class = 0, class_mask = 0;
		int fields;

		if (!strlen(id))
			continue;

		fields = sscanf(id, "%x:%x:%x:%x:%x:%x",
				&vendor, &device, &subvendor, &subdevice,
				&class, &class_mask);

		if (fields < 2) {
			pr_warn("invalid id string \"%s\"\n", id);
			continue;
		}

		rc = pci_add_dynid(&poweroff_pci_driver, vendor, device,
				   subvendor, subdevice, class, class_mask, 0);
		if (rc)
			pr_warn("failed to add dynamic id [%04x:%04x[%04x:%04x]] class %#08x/%08x (%d)\n",
				vendor, device, subvendor, subdevice,
				class, class_mask, rc);
		else
			pr_info("add [%04x:%04x[%04x:%04x]] class %#08x/%08x\n",
				vendor, device, subvendor, subdevice,
				class, class_mask);
	}
}

static int __init poweroff_pci_init(void)
{
	int ret;

	/* Register and scan for devices */
	ret = pci_register_driver(&poweroff_pci_driver);
	if (ret)
		return ret;

	poweroff_pci_fill_ids();

	return 0;
}
module_init(poweroff_pci_init);

static void __exit poweroff_pci_cleanup(void)
{
	pci_unregister_driver(&poweroff_pci_driver);
}
module_exit(poweroff_pci_cleanup);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
