/*
 * Copyright (c) 2017 Nuvoton Technology Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation;version 2 of the License.
 *
 */

#include <linux/clk.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/pinctrl/machine.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/pinctrl/consumer.h>

#include <mach/map.h>
#include <mach/regs-gcr.h>

#include "../core.h"

#define MAX_NB_GPIO_PER_BANK        16

// The numbering is not related to actual layout.
const struct pinctrl_pin_desc nuc980_pins[] = {
	PINCTRL_PIN(0x00, "PA0"),
	PINCTRL_PIN(0x01, "PA1"),
	PINCTRL_PIN(0x02, "PA2"),
	PINCTRL_PIN(0x03, "PA3"),
	PINCTRL_PIN(0x04, "PA4"),
	PINCTRL_PIN(0x05, "PA5"),
	PINCTRL_PIN(0x06, "PA6"),
	PINCTRL_PIN(0x07, "PA7"),
	PINCTRL_PIN(0x08, "PA8"),
	PINCTRL_PIN(0x09, "PA9"),
	PINCTRL_PIN(0x0A, "PA10"),
	PINCTRL_PIN(0x0B, "PA11"),
	PINCTRL_PIN(0x0C, "PA12"),
	PINCTRL_PIN(0x0D, "PA13"),
	PINCTRL_PIN(0x0E, "PA14"),
	PINCTRL_PIN(0x0F, "PA15"),
	PINCTRL_PIN(0x10, "PB0"),
	PINCTRL_PIN(0x11, "PB1"),
	PINCTRL_PIN(0x12, "PB2"),
	PINCTRL_PIN(0x13, "PB3"),
	PINCTRL_PIN(0x14, "PB4"),
	PINCTRL_PIN(0x15, "PB5"),
	PINCTRL_PIN(0x16, "PB6"),
	PINCTRL_PIN(0x17, "PB7"),
	PINCTRL_PIN(0x18, "PB8"),
	PINCTRL_PIN(0x19, "PB9"),
	PINCTRL_PIN(0x1A, "PB10"),
	PINCTRL_PIN(0x1B, "PB11"),
	PINCTRL_PIN(0x1C, "PB12"),
	PINCTRL_PIN(0x1D, "PB13"),
	PINCTRL_PIN(0x1E, "PB14"),
	PINCTRL_PIN(0x1F, "PB15"),
	PINCTRL_PIN(0x20, "PC0"),
	PINCTRL_PIN(0x21, "PC1"),
	PINCTRL_PIN(0x22, "PC2"),
	PINCTRL_PIN(0x23, "PC3"),
	PINCTRL_PIN(0x24, "PC4"),
	PINCTRL_PIN(0x25, "PC5"),
	PINCTRL_PIN(0x26, "PC6"),
	PINCTRL_PIN(0x27, "PC7"),
	PINCTRL_PIN(0x28, "PC8"),
	PINCTRL_PIN(0x29, "PC9"),
	PINCTRL_PIN(0x2A, "PC10"),
	PINCTRL_PIN(0x2B, "PC11"),
	PINCTRL_PIN(0x2C, "PC12"),
	PINCTRL_PIN(0x2D, "PC13"),
	PINCTRL_PIN(0x2E, "PC14"),
	PINCTRL_PIN(0x2F, "PC15"),
	PINCTRL_PIN(0x30, "PD0"),
	PINCTRL_PIN(0x31, "PD1"),
	PINCTRL_PIN(0x32, "PD2"),
	PINCTRL_PIN(0x33, "PD3"),
	PINCTRL_PIN(0x34, "PD4"),
	PINCTRL_PIN(0x35, "PD5"),
	PINCTRL_PIN(0x36, "PD6"),
	PINCTRL_PIN(0x37, "PD7"),
	PINCTRL_PIN(0x38, "PD8"),
	PINCTRL_PIN(0x39, "PD9"),
	PINCTRL_PIN(0x3A, "PD10"),
	PINCTRL_PIN(0x3B, "PD11"),
	PINCTRL_PIN(0x3C, "PD12"),
	PINCTRL_PIN(0x3D, "PD13"),
	PINCTRL_PIN(0x3E, "PD14"),
	PINCTRL_PIN(0x3F, "PD15"),
	PINCTRL_PIN(0x40, "PE0"),
	PINCTRL_PIN(0x41, "PE1"),
	PINCTRL_PIN(0x42, "PE2"),
	PINCTRL_PIN(0x43, "PE3"),
	PINCTRL_PIN(0x44, "PE4"),
	PINCTRL_PIN(0x45, "PE5"),
	PINCTRL_PIN(0x46, "PE6"),
	PINCTRL_PIN(0x47, "PE7"),
	PINCTRL_PIN(0x48, "PE8"),
	PINCTRL_PIN(0x49, "PE9"),
	PINCTRL_PIN(0x4A, "PE10"),
	PINCTRL_PIN(0x4B, "PE11"),
	PINCTRL_PIN(0x4C, "PE12"),
	PINCTRL_PIN(0x4D, "PE13"),
	PINCTRL_PIN(0x4E, "PE14"),
	PINCTRL_PIN(0x4F, "PE15"),
	PINCTRL_PIN(0x50, "PF0"),
	PINCTRL_PIN(0x51, "PF1"),
	PINCTRL_PIN(0x52, "PF2"),
	PINCTRL_PIN(0x53, "PF3"),
	PINCTRL_PIN(0x54, "PF4"),
	PINCTRL_PIN(0x55, "PF5"),
	PINCTRL_PIN(0x56, "PF6"),
	PINCTRL_PIN(0x57, "PF7"),
	PINCTRL_PIN(0x58, "PF8"),
	PINCTRL_PIN(0x59, "PF9"),
	PINCTRL_PIN(0x5A, "PF10"),
	PINCTRL_PIN(0x5B, "PF11"),
	PINCTRL_PIN(0x5C, "PF12"),
	PINCTRL_PIN(0x5D, "PF13"),
	PINCTRL_PIN(0x5E, "PF14"),
	PINCTRL_PIN(0x5F, "PF15"),
	PINCTRL_PIN(0x60, "PG0"),
	PINCTRL_PIN(0x61, "PG1"),
	PINCTRL_PIN(0x62, "PG2"),
	PINCTRL_PIN(0x63, "PG3"),
	PINCTRL_PIN(0x64, "PG4"),
	PINCTRL_PIN(0x65, "PG5"),
	PINCTRL_PIN(0x66, "PG6"),
	PINCTRL_PIN(0x67, "PG7"),
	PINCTRL_PIN(0x68, "PG8"),
	PINCTRL_PIN(0x69, "PG9"),
	PINCTRL_PIN(0x6A, "PG10"),
	PINCTRL_PIN(0x6B, "PG11"),
	PINCTRL_PIN(0x6C, "PG12"),
	PINCTRL_PIN(0x6D, "PG13"),
	PINCTRL_PIN(0x6E, "PG14"),
	PINCTRL_PIN(0x6F, "PG15"),
};

/**
 * struct nuc980_pmx_pin - describes an NUC980 pin multi-function
 * @bank: the bank of the pin (0 for PA, 1 for PB...)
 * @pin: pin number (0 ~ 0xf)
 * @func: multi-function pin setting value
 * @conf: reserved for GPIO mode
 */
struct nuc980_pmx_pin {
	uint32_t	bank;
	uint32_t	pin;
	uint32_t	func;
	unsigned long	conf;
};

/**
 * struct nuc980_pmx_func - describes NUC980 pinmux functions
 * @name: the name of this specific function
 * @groups: corresponding pin groups
 * @ngroups: the number of groups
 */
struct nuc980_pmx_func {
	const char	*name;
	const char	**groups;
	unsigned	ngroups;
};

/**
 * struct nuc980_pin_group - describes an NUC980 pin group
 * @name: the name of this specific pin group
 * @pins_conf: the mux mode for each pin in this group. The size of this
 *  array is the same as pins.
 * @pins: an array of discrete physical pins used in this group, taken
 *  from the driver-local pin enumeration space
 * @npins: the number of pins in this group array, i.e. the number of
 *  elements in .pins so we can iterate over that array
 */
struct nuc980_pin_group {
	const char	*name;
	struct nuc980_pmx_pin	*pins_conf;
	unsigned int	*pins;
	unsigned	npins;
};

struct nuc980_pinctrl {
	struct device	*dev;
	struct pinctrl_dev *pctldev;
	int		nbanks;

	struct nuc980_pmx_func *functions;
	int		nfunctions;

	struct nuc980_pin_group *groups;
	int		ngroups;
};

static const inline struct nuc980_pin_group *nuc980_pinctrl_find_group_by_name(
			const struct nuc980_pinctrl *info,
			const char *name)
{
	const struct nuc980_pin_group *grp = NULL;
	int i;

	for (i = 0; i < info->ngroups; i++) {
		if (strcmp(info->groups[i].name, name)) {
			continue;
		}

		grp = &info->groups[i];
		dev_dbg(info->dev, "%s: %d 0:%d\n", name, grp->npins, grp->pins[0]);
		break;
	}
	return grp;
}

static int nuc980_get_groups_count(struct pinctrl_dev *pctldev)
{
	struct nuc980_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);

	return info->ngroups;
}

static const char *nuc980_get_group_name(struct pinctrl_dev *pctldev,
					 unsigned selector)
{
	struct nuc980_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);

	return info->groups[selector].name;
}

static int nuc980_get_group_pins(struct pinctrl_dev *pctldev, unsigned selector,
				 const unsigned **pins,
				 unsigned *npins)
{
	struct nuc980_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);

	if (selector >= info->ngroups) {
		return -EINVAL;
	}
	*pins = info->groups[selector].pins;
	*npins = info->groups[selector].npins;
	return 0;
}

static int nuc980_dt_node_to_map(struct pinctrl_dev *pctldev,
				 struct device_node *np,
				 struct pinctrl_map **map, unsigned *num_maps)
{
	struct nuc980_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);
	const struct nuc980_pin_group *grp;
	struct pinctrl_map *new_map;
	struct device_node *parent;
	int map_num = 1;
	int i;

	/*
	 * first find the group of this node and check if we need create
	 * config maps for pins
	 */
	grp = nuc980_pinctrl_find_group_by_name(info, np->name);
	if (!grp) {
		dev_err(info->dev, "unable to find group for node %s\n",
			np->name);
		return -EINVAL;
	}

	map_num += grp->npins;
	new_map = devm_kzalloc(info->dev, sizeof(*new_map) * map_num, GFP_KERNEL);
	if (!new_map) {
		return -ENOMEM;
	}

	*map = new_map;
	*num_maps = map_num;

	/* create mux map */
	parent = of_get_parent(np);
	if (!parent) {
		devm_kfree(info->dev, new_map);
		return -EINVAL;
	}
	new_map[0].type = PIN_MAP_TYPE_MUX_GROUP;
	new_map[0].data.mux.function = parent->name;
	new_map[0].data.mux.group = np->name;
	of_node_put(parent);

	/* create config map */
	new_map++;
	for (i = 0; i < grp->npins; i++) {
		new_map[i].type = PIN_MAP_TYPE_CONFIGS_PIN;
		new_map[i].data.configs.group_or_pin = pin_get_name(pctldev, grp->pins[i]);
		new_map[i].data.configs.configs = &grp->pins_conf[i].conf;
		new_map[i].data.configs.num_configs = 1;
	}

	dev_dbg(info->dev, "maps: function %s group %s num %d\n",
		(*map)->data.mux.function, (*map)->data.mux.group, map_num);

	return 0;
}

static void nuc980_dt_free_map(struct pinctrl_dev *pctldev,
			       struct pinctrl_map *map, unsigned num_maps)
{
}

static const struct pinctrl_ops nuc980_pctrl_ops = {
	.get_groups_count   = nuc980_get_groups_count,
	.get_group_name     = nuc980_get_group_name,
	.get_group_pins     = nuc980_get_group_pins,
	.dt_node_to_map     = nuc980_dt_node_to_map,
	.dt_free_map        = nuc980_dt_free_map,
};

static void nuc980_pin_dbg(const struct device *dev, const struct nuc980_pmx_pin *pin)
{
	dev_dbg(dev, "P%c.%d: func=%d\n", pin->bank+'A', pin->pin, pin->func);
}

static int pin_check_config(struct nuc980_pinctrl *info, const char *name,
			    int index, const struct nuc980_pmx_pin *pin)
{
	/* check if it's a valid config */
	if (pin->bank >= info->nbanks) {
		dev_err(info->dev, "%s: pin conf %d bank_id %d >= nbanks %d\n",
		        name, index, pin->bank, info->nbanks);
		return -EINVAL;
	}

	if (pin->pin >= MAX_NB_GPIO_PER_BANK) {
		dev_err(info->dev, "%s: pin conf %d pin_bank_id %d >= %d\n",
		        name, index, pin->pin, MAX_NB_GPIO_PER_BANK);
		return -EINVAL;
	}

	if (pin->func > 0xf) {
		dev_err(info->dev, "%s: invalid pin function setting %d!\n", name, pin->func);
		return -EINVAL;
	}
	return 0;
}


static int nuc980_pmx_get_funcs_count(struct pinctrl_dev *pctldev)
{
	struct nuc980_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);

	return info->nfunctions;
}

static const char *nuc980_pmx_get_func_name(struct pinctrl_dev *pctldev,
					    unsigned selector)
{
	struct nuc980_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);

	return info->functions[selector].name;
}

static int nuc980_pmx_get_groups(struct pinctrl_dev *pctldev, unsigned selector,
				 const char * const **groups,
				 unsigned * const num_groups)
{
	struct nuc980_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);

	*groups = info->functions[selector].groups;
	*num_groups = info->functions[selector].ngroups;

	return 0;
}

/*
 * selector = data.nux.func, which is entry number in nuc980_functions,
 * and group = data.mux.group, which is entry number in nuc980_pmx_func
 * group is not used since some function use different setting between
 * different ports. for example UART....
 */
int nuc980_pmx_set_mux(struct pinctrl_dev *pctldev, unsigned selector,
		       unsigned group)
{
	struct nuc980_pinctrl *info = pinctrl_dev_get_drvdata(pctldev);
	const struct nuc980_pmx_pin *pins_conf = info->groups[group].pins_conf;
	const struct nuc980_pmx_pin *pin;
	uint32_t npins = info->groups[group].npins;
	unsigned int i, ret;
	unsigned int reg, offset;

	/* first check that all the pins of the group are valid with a valid
	 * paramter */
	for (i = 0; i < npins; i++) {
		pin = &pins_conf[i];
		ret = pin_check_config(info, info->groups[group].name, i, pin);
		if (ret) {
			return ret;
		}
	}

	for (i = 0; i < npins; i++) {
		pin = &pins_conf[i];
		offset = (pin->bank * 8) + ((pin->pin > 7) ? 4 : 0);
		reg = __raw_readl(REG_MFP_GPA_L + offset);

		reg = (reg & ~(0xF << ((pin->pin & 0x7) * 4))) | (pin->func << ((pin->pin & 0x7) * 4));

		__raw_writel(reg, REG_MFP_GPA_L + offset);
	}
	return 0;
}

static const struct pinmux_ops nuc980_pmx_ops = {
	.get_functions_count  = nuc980_pmx_get_funcs_count,
	.get_function_name    = nuc980_pmx_get_func_name,
	.get_function_groups  = nuc980_pmx_get_groups,
	.set_mux = nuc980_pmx_set_mux,
};

static int nuc980_pinconf_get(struct pinctrl_dev *pctldev,
                              unsigned pin_id, unsigned long *config)
{
	return 0;
}

static int nuc980_pinconf_set(struct pinctrl_dev *pctldev,
                              unsigned pin_id, unsigned long *configs, unsigned num_configs)
{
	return 0;
}

static const struct pinconf_ops nuc980_pinconf_ops = {
	.pin_config_get         = nuc980_pinconf_get,
	.pin_config_set         = nuc980_pinconf_set,
};

static struct pinctrl_desc nuc980_pinctrl_desc = {
	.name     = "nuc980-pinctrl",
	.pins     = nuc980_pins,
	.npins    = ARRAY_SIZE(nuc980_pins),
	.pctlops  = &nuc980_pctrl_ops,
	.pmxops   = &nuc980_pmx_ops,
	.confops  = &nuc980_pinconf_ops,
	.owner    = THIS_MODULE,
};


static void nuc980_pinctrl_child_count(struct nuc980_pinctrl *info,
                                       struct device_node *np)
{
	struct device_node *child;

	for_each_child_of_node(np, child) {
		info->nfunctions++;
		info->ngroups += of_get_child_count(child);
	}
}

static int nuc980_pinctrl_parse_groups(struct device_node *np,
                                       struct nuc980_pin_group *grp,
                                       struct nuc980_pinctrl *info, u32 index)
{
	struct nuc980_pmx_pin *pin;
	int size;
	const __be32 *list;
	int i, j;

	dev_dbg(info->dev, "group(%d): %s\n", index, np->name);

	/* Initialise group */
	grp->name = np->name;

	/*
	 * the binding format is nuvoton,pins = <bank pin pin-function>,
	 * do sanity check and calculate pins number
	 */
	list = of_get_property(np, "nuvoton,pins", &size);
	/* we do not check return since it's safe node passed down */
	size /= sizeof(*list);
	if (!size || size % 4) {
		dev_err(info->dev, "wrong setting!\n");
		return -EINVAL;
	}

	grp->npins = size / 4;
	pin = grp->pins_conf = devm_kzalloc(info->dev, grp->npins * sizeof(struct nuc980_pmx_pin), GFP_KERNEL);
	grp->pins = devm_kzalloc(info->dev, grp->npins * sizeof(unsigned int), GFP_KERNEL);
	if (!grp->pins_conf || !grp->pins) {
		return -ENOMEM;
	}

	for (i = 0, j = 0; i < size; i += 4, j++) {
		pin->bank = be32_to_cpu(*list++);
		pin->pin = be32_to_cpu(*list++);
		grp->pins[j] = pin->bank * MAX_NB_GPIO_PER_BANK + pin->pin;
		pin->func = be32_to_cpu(*list++);
		pin->conf = be32_to_cpu(*list++);

		nuc980_pin_dbg(info->dev, pin);
		pin++;
	}

	return 0;
}

static int nuc980_pinctrl_parse_functions(struct device_node *np,
        struct nuc980_pinctrl *info, u32 index)
{
	struct device_node *child;
	struct nuc980_pmx_func *func;
	struct nuc980_pin_group *grp;
	int ret;
	static u32 grp_index;
	u32 i = 0;

	dev_dbg(info->dev, "parse function(%d): %s\n", index, np->name);

	func = &info->functions[index];

	/* Initialise function */
	func->name = np->name;
	func->ngroups = of_get_child_count(np);
	if (func->ngroups <= 0) {
		dev_err(info->dev, "no groups defined\n");
		return -EINVAL;
	}
	func->groups = devm_kzalloc(info->dev,
	                            func->ngroups * sizeof(char *), GFP_KERNEL);
	if (!func->groups) {
		return -ENOMEM;
	}

	for_each_child_of_node(np, child) {
		func->groups[i] = child->name;
		grp = &info->groups[grp_index++];
		ret = nuc980_pinctrl_parse_groups(child, grp, info, i++);
		if (ret) {
			return ret;
		}
	}

	return 0;
}

static int nuc980_pinctrl_probe(struct platform_device *pdev)
{
	struct nuc980_pinctrl *info;
	struct device_node *np = pdev->dev.of_node;
	struct device_node *child;
	int ret;
	int i;

	if (!np)
		return -ENODEV;

	info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
	if (!info) {
		return -ENOMEM;
	}

	info->dev = &pdev->dev;
	info->nbanks = 7;  /* PA ~ PG */
	nuc980_pinctrl_child_count(info, np);
	info->functions = devm_kzalloc(&pdev->dev, info->nfunctions * sizeof(struct nuc980_pmx_func),
	                               GFP_KERNEL);
	if (!info->functions) {
		return -ENOMEM;
	}

	info->groups = devm_kzalloc(&pdev->dev, info->ngroups * sizeof(struct nuc980_pin_group),
	                            GFP_KERNEL);
	if (!info->groups) {
		return -ENOMEM;
	}

	dev_dbg(&pdev->dev, "nfunctions = %d\n", info->nfunctions);
	dev_dbg(&pdev->dev, "ngroups = %d\n", info->ngroups);

	i = 0;
	for_each_child_of_node(np, child) {
		ret = nuc980_pinctrl_parse_functions(child, info, i++);
		if (ret) {
			dev_err(&pdev->dev, "failed to parse function\n");
			return ret;
		}
	}

	nuc980_pinctrl_desc.name = dev_name(&pdev->dev);

//  for (i = 0 , k = 0; i < info->nbanks; i++) {
//      for (j = 0; j < MAX_NB_GPIO_PER_BANK; j++, k++) {
//          pdesc->number = k;
//          pdesc->name = kasprintf(GFP_KERNEL, "pio%c%d", i + 'A', j);
//          pdesc++;
//      }
//  }

	dev_set_drvdata(&pdev->dev, info);
	info->pctldev = devm_pinctrl_register(&pdev->dev,
					       &nuc980_pinctrl_desc, info);
	if (IS_ERR(info->pctldev)) {
		dev_err(&pdev->dev, "Failed to register pinctrl device\n");
		return PTR_ERR(info->pctldev);
	}
	dev_info(&pdev->dev, "initialized NUC980 pinctrl driver\n");
	return 0;
}

static const struct of_device_id nuc980_pinctrl_match[] = {
	{ .compatible = "nuvoton,nuc980-pinctrl" },
	{ },
};
MODULE_DEVICE_TABLE(of, nuc980_pinctrl_match);

static struct platform_driver nuc980_pinctrl_driver = {
	.driver = {
		.name = "nuc980-pinctrl",
		.of_match_table = nuc980_pinctrl_match,
		.suppress_bind_attrs = true,
	},
	.probe = nuc980_pinctrl_probe,
};

static int __init nuc980_pinctrl_init(void)
{
	return platform_driver_register(&nuc980_pinctrl_driver);
}

arch_initcall(nuc980_pinctrl_init);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("ychuang3@nuvoton.com");
MODULE_DESCRIPTION("Nuvoton NUC980 series pinctrl driver");
