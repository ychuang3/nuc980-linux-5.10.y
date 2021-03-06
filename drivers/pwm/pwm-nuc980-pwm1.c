/*
 * Copyright (c) 2018 Nuvoton Technology Corp.
 *
 * NUC980 Series PWM driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
*/


#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/pwm.h>
#include <linux/pinctrl/consumer.h>
#include <mach/map.h>
#include <mach/regs-pwm1.h>
#include <mach/regs-clock.h>

//#define DEBUG_PWM

struct nuc980_chip {
	struct platform_device	*pdev;
	struct clk		*clk;
	struct pwm_chip		 chip;
};

#define to_nuc980_chip(chip)	container_of(chip, struct nuc980_chip, chip)

#ifdef DEBUG_PWM
static void pwm_dbg(void)
{

	printk("%08x\n", __raw_readl(REG_PWM_PPR));
	printk("%08x\n", __raw_readl(REG_PWM_CSR));
	printk("%08x\n", __raw_readl(REG_PWM_PCR));
	printk("%08x\n", __raw_readl(REG_PWM_CNR0));
	printk("%08x\n", __raw_readl(REG_PWM_CMR0));
	printk("%08x\n", __raw_readl(REG_PWM_CNR1));
	printk("%08x\n", __raw_readl(REG_PWM_CMR1));
	printk("%08x\n", __raw_readl(REG_PWM_CNR2));
	printk("%08x\n", __raw_readl(REG_PWM_CMR2));
	printk("%08x\n", __raw_readl(REG_PWM_CNR3));
	printk("%08x\n", __raw_readl(REG_PWM_CMR3));
	printk("%08x\n", __raw_readl(REG_PWM_PIER));
	printk("%08x\n", __raw_readl(REG_PWM_PIIR));

}
#endif

static int nuc980_pwm_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	//struct nuc980_chip *nuc980 = to_nuc980_chip(chip);
	int ch = pwm->hwpwm;
	unsigned long flags, cnr, cmr;

#ifdef DEBUG_PWM
	printk("Enter %s ....ch[%d]\n",__FUNCTION__,pwm->hwpwm);
#endif

	local_irq_save(flags);

	if(ch == 0) {
		cnr = __raw_readl(REG_PWM_CNR0);
		cmr = __raw_readl(REG_PWM_CMR0);
		__raw_writel(__raw_readl(REG_PWM_PCR) | (9), REG_PWM_PCR);
		__raw_writel(cnr, REG_PWM_CNR0);
		__raw_writel(cmr, REG_PWM_CMR0);
	} else if(ch == 1) {
		cnr = __raw_readl(REG_PWM_CNR1);
		cmr = __raw_readl(REG_PWM_CMR1);
		__raw_writel(__raw_readl(REG_PWM_PCR) | (9 << 8), REG_PWM_PCR);
		__raw_writel(cnr, REG_PWM_CNR1);
		__raw_writel(cmr, REG_PWM_CMR1);
	} else if (ch == 2) {
		cnr = __raw_readl(REG_PWM_CNR2);
		cmr = __raw_readl(REG_PWM_CMR2);
		__raw_writel(__raw_readl(REG_PWM_PCR) | (9 << 12), REG_PWM_PCR);
		__raw_writel(cnr, REG_PWM_CNR2);
		__raw_writel(cmr, REG_PWM_CMR2);
	} else {	/* ch 3 */
		cnr = __raw_readl(REG_PWM_CNR3);
		cmr = __raw_readl(REG_PWM_CMR3);
		__raw_writel(__raw_readl(REG_PWM_PCR) | (9 << 16), REG_PWM_PCR);
		__raw_writel(cnr, REG_PWM_CNR3);
		__raw_writel(cmr, REG_PWM_CMR3);
	}

	local_irq_restore(flags);
#ifdef DEBUG_PWM
	pwm_dbg();
#endif
	return 0;
}

static void nuc980_pwm_disable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	//struct nuc980_chip *nuc980 = to_nuc980_chip(chip);
	int ch = pwm->hwpwm;
	unsigned long flags;

#ifdef DEBUG_PWM
	printk("Enter %s ....ch[%d]\n",__FUNCTION__,pwm->hwpwm);
#endif

	local_irq_save(flags);
	if(ch == 0)
		__raw_writel(__raw_readl(REG_PWM_PCR) & ~(1), REG_PWM_PCR);
	else if(ch == 1)
		__raw_writel(__raw_readl(REG_PWM_PCR) & ~(1 << 8), REG_PWM_PCR);
	else if (ch == 2)
		__raw_writel(__raw_readl(REG_PWM_PCR) & ~(1 << 12), REG_PWM_PCR);
	else	/* ch 3 */
		__raw_writel(__raw_readl(REG_PWM_PCR) & ~(1 << 16), REG_PWM_PCR);

	local_irq_restore(flags);
#ifdef DEBUG_PWM
	pwm_dbg();
#endif
}

static int nuc980_pwm_set_polarity(struct pwm_chip *chip, struct pwm_device *pwm, enum pwm_polarity polarity)
{
	//struct nuc980_chip *nuc980 = to_nuc980_chip(chip);
	int ch = pwm->hwpwm;
	unsigned long flags;

#ifdef DEBUG_PWM
	printk("Enter %s ....ch[%d]\n",__FUNCTION__,pwm->hwpwm);
#endif

	local_irq_save(flags);

	if(ch == 0) {
		if (polarity == PWM_POLARITY_NORMAL)
			__raw_writel(__raw_readl(REG_PWM_PCR) & ~(4), REG_PWM_PCR);
		else
			__raw_writel(__raw_readl(REG_PWM_PCR) | (4), REG_PWM_PCR);
	} else if(ch == 1) {
		if (polarity == PWM_POLARITY_NORMAL)
			__raw_writel(__raw_readl(REG_PWM_PCR) & ~(4 << 8), REG_PWM_PCR);
		else
			__raw_writel(__raw_readl(REG_PWM_PCR) | (4 << 8), REG_PWM_PCR);
	} else if (ch == 2) {
		if (polarity == PWM_POLARITY_NORMAL)
			__raw_writel(__raw_readl(REG_PWM_PCR) & ~(4 << 12), REG_PWM_PCR);
		else
			__raw_writel(__raw_readl(REG_PWM_PCR) | (4 << 12), REG_PWM_PCR);
	} else {	/* ch 3 */
		if (polarity == PWM_POLARITY_NORMAL)
			__raw_writel(__raw_readl(REG_PWM_PCR) & ~(4 << 16), REG_PWM_PCR);
		else
			__raw_writel(__raw_readl(REG_PWM_PCR) | (4 << 16), REG_PWM_PCR);
	}

	local_irq_restore(flags);
#ifdef DEBUG_PWM
	pwm_dbg();
#endif
	return 0;
}


static int nuc980_pwm_config(struct pwm_chip *chip, struct pwm_device *pwm,
                             int duty_ns, int period_ns)
{
	struct nuc980_chip *nuc980 = to_nuc980_chip(chip);
	unsigned long period, duty, prescale;
	unsigned long flags;
	int ch = pwm->hwpwm;

#ifdef DEBUG_PWM
	printk("Enter %s ....ch[%d]\n",__FUNCTION__,pwm->hwpwm);
#endif

	// Get PCLK, calculate valid parameter range.
	prescale = clk_get_rate(nuc980->clk) / 1000000 - 1;

	// now pwm time unit is 1000ns.
	period = (period_ns + 500) / 1000;
	duty = (duty_ns + 500) / 1000;

	// don't want the minus 1 below change the value to -1 (0xFFFF)
	if(period == 0)
		period = 1;
	if(duty == 0)
		duty = 1;

	local_irq_save(flags);
	// Set prescale for all pwm channels
	__raw_writel(prescale | (prescale << 8), REG_PWM_PPR);

	if(ch == 0) {
		__raw_writel(period - 1, REG_PWM_CNR0);
		__raw_writel(duty - 1, REG_PWM_CMR0);
	} else if(ch == 1) {
		__raw_writel(period - 1, REG_PWM_CNR1);
		__raw_writel(duty - 1, REG_PWM_CMR1);
	} else if (ch == 2) {
		__raw_writel(period - 1, REG_PWM_CNR2);
		__raw_writel(duty - 1, REG_PWM_CMR2);
	} else {/* ch 3 */
		__raw_writel(period - 1, REG_PWM_CNR3);
		__raw_writel(duty - 1, REG_PWM_CMR3);
	}

	local_irq_restore(flags);

#ifdef DEBUG_PWM
	pwm_dbg();
#endif

	return 0;
}

static struct pwm_ops nuc980_pwm_ops = {
	.enable = nuc980_pwm_enable,
	.disable = nuc980_pwm_disable,
	.config = nuc980_pwm_config,
	.set_polarity = nuc980_pwm_set_polarity,
	.owner = THIS_MODULE,
};

static int nuc980_pwm_probe(struct platform_device *pdev)
{

	struct nuc980_chip *nuc980;
	struct pinctrl *p;
	int ret;
#if defined(CONFIG_USE_OF)
	u32 id;
#endif

	nuc980 = devm_kzalloc(&pdev->dev, sizeof(*nuc980), GFP_KERNEL);
	if (nuc980 == NULL) {
		dev_err(&pdev->dev, "failed to allocate memory for pwm_device\n");
		return -ENOMEM;
	}
	/* calculate base of control bits in TCON */

	nuc980->chip.dev = &pdev->dev;
	nuc980->chip.ops = &nuc980_pwm_ops;
	nuc980->chip.base = pdev->id;
	nuc980->chip.npwm = 4;

	nuc980->clk = clk_get(NULL, "pwm1");
	if (IS_ERR(nuc980->clk)) {
		dev_err(&pdev->dev, "failed to get pwm clock\n");
		ret = PTR_ERR(nuc980->clk);
		return ret;
	}

	clk_prepare(nuc980->clk);
	clk_enable(nuc980->clk);
	// all channel prescale output div by 1
	__raw_writel(0x4444, REG_PWM_CSR);

#if defined(CONFIG_USE_OF)
	if (of_property_read_u32(pdev->dev.of_node, "id", &id)) {
		printk("can't get pwm id from dt\n");
	} else {
		pdev->id = id;
	}
#endif

	if(pdev->id == 4) {
#if defined(CONFIG_USE_OF)
		p = devm_pinctrl_get_select_default(&pdev->dev);
#else
#if defined (CONFIG_NUC980_PWM1_CH0_PB12)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm10-PB12");
#elif defined (CONFIG_NUC980_PWM1_CH0_PG6)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm10-PG6");
#elif defined (CONFIG_NUC980_PWM1_CH0_PG11)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm10-PG11");
#elif defined (CONFIG_NUC980_PWM1_CH0_PF9)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm10-PF9");
#endif

#ifndef CONFIG_NUC980_PWM1_CH0_NONE
		if(IS_ERR(p)) {
			dev_err(&pdev->dev, "unable to reserve output pin\n");

		}
#endif
#endif
	}
	if(pdev->id == 5) {
#if defined(CONFIG_USE_OF)
		p = devm_pinctrl_get_select_default(&pdev->dev);
#else
#if defined (CONFIG_NUC980_PWM1_CH1_PB11)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm11-PB11");
#elif defined (CONFIG_NUC980_PWM1_CH1_PG7)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm11-PG7");
#elif defined (CONFIG_NUC980_PWM1_CH1_PG12)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm11-PG12");
#elif defined (CONFIG_NUC980_PWM1_CH1_PF10)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm11-PF10");
#endif

#ifndef CONFIG_NUC980_PWM1_CH1_NONE
		if(IS_ERR(p)) {
			dev_err(&pdev->dev, "unable to reserve output pin\n");
		}
#endif
#endif
	}
	if(pdev->id == 6) {
#if defined(CONFIG_USE_OF)
		p = devm_pinctrl_get_select_default(&pdev->dev);
#else
#if defined (CONFIG_NUC980_PWM1_CH2_PB10)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm12-PB10");
#elif defined (CONFIG_NUC980_PWM1_CH2_PG8)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm12-PG8");
#elif defined (CONFIG_NUC980_PWM1_CH2_PG13)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm12-PG13");
#elif defined (CONFIG_NUC980_PWM1_CH2_PE10)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm12-PE10");
#endif

#ifndef CONFIG_NUC980_PWM1_CH2_NONE
		if(IS_ERR(p)) {
			dev_err(&pdev->dev, "unable to reserve output pin\n");
		}
#endif
#endif
	}
	if(pdev->id == 7) {
#if defined(CONFIG_USE_OF)
		p = devm_pinctrl_get_select_default(&pdev->dev);
#else
#if defined (CONFIG_NUC980_PWM1_CH3_PB9)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm13-PB9");
#elif defined (CONFIG_NUC980_PWM1_CH3_PG9)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm13-PG9");
#elif defined (CONFIG_NUC980_PWM1_CH3_PG14)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm13-PG14");
#elif defined (CONFIG_NUC980_PWM1_CH3_PE12)
		p = devm_pinctrl_get_select(&pdev->dev, "pwm13-PE12");
#endif

#ifndef CONFIG_NUC980_PWM1_CH3_NONE
		if(IS_ERR(p)) {
			dev_err(&pdev->dev, "unable to reserve output pin\n");
		}
#endif
#endif
	}

	ret = pwmchip_add(&nuc980->chip);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to register pwm\n");
		goto err;
	}

	platform_set_drvdata(pdev, nuc980);

	return 0;

err:
	//clk_disable(nuc980->clk);
	return ret;
}

static int nuc980_pwm_remove(struct platform_device *pdev)
{
	struct nuc980_chip *nuc980 = platform_get_drvdata(pdev);

	clk_disable(nuc980->clk);
	return pwmchip_remove(&nuc980->chip);
}

#ifdef CONFIG_PM
static u32 pcr_save, cnr0_save, cnr1_save, cnr2_save, cnr3_save;
static int nuc980_pwm_suspend(struct platform_device *pdev, pm_message_t state)
{
	//struct nuc980_chip *chip = dev_get_drvdata(dev);

	if (pdev->id == 7) {
		pcr_save = __raw_readl(REG_PWM_PCR);

		cnr3_save = __raw_readl(REG_PWM_CNR3);
		__raw_writel(0, REG_PWM_CNR3);
		while(__raw_readl(REG_PWM_PDR3));
	} else if (pdev->id == 6) {
		cnr2_save = __raw_readl(REG_PWM_CNR2);
		__raw_writel(0, REG_PWM_CNR2);
		while(__raw_readl(REG_PWM_PDR2));
	} else if (pdev->id == 5) {
		cnr1_save = __raw_readl(REG_PWM_CNR1);
		__raw_writel(0, REG_PWM_CNR1);
		while(__raw_readl(REG_PWM_PDR1));
	}
	if (pdev->id == 4) {
		cnr0_save = __raw_readl(REG_PWM_CNR0);
		__raw_writel(0, REG_PWM_CNR0);
		while(__raw_readl(REG_PWM_PDR0));

		__raw_writel( __raw_readl(REG_PWM_PCR) & ~0x11101, REG_PWM_PCR);
	}


	return 0;
}

static int nuc980_pwm_resume(struct platform_device *pdev)
{
	//struct nuc980_chip *chip = dev_get_drvdata(dev);

	if (pdev->id == 4) {
		__raw_writel(cnr0_save, REG_PWM_CNR0);
	} else if (pdev->id == 5) {
		__raw_writel(cnr1_save, REG_PWM_CNR1);
	} else if (pdev->id == 6) {
		__raw_writel(cnr2_save, REG_PWM_CNR2);
	} else if (pdev->id == 7) {
		__raw_writel(cnr3_save, REG_PWM_CNR3);

		__raw_writel(pcr_save, REG_PWM_PCR);
	}

	return 0;
}

//static SIMPLE_DEV_PM_OPS(nuc980_pwm_pm_ops, nuc980_pwm_suspend, nuc980_pwm_resume);
#else
#define nuc980_pwm_suspend NULL
#define nuc980_pwm_resume  NULL
#endif


#if defined(CONFIG_USE_OF)
static const struct of_device_id nuc980_pwm1_of_match[] = {
	{   .compatible = "nuvoton,nuc980-pwm1" } ,
	{	},
};
MODULE_DEVICE_TABLE(of, nuc980_pwm1_of_match);
#endif

static struct platform_driver nuc980_pwm1_driver = {
	.driver		= {
		.name	= "nuc980-pwm1",
		.owner	= THIS_MODULE,
#if defined(CONFIG_USE_OF)
		.of_match_table = of_match_ptr(nuc980_pwm1_of_match),
#endif
//#ifdef CONFIG_PM
//		.pm	= &nuc980_pwm_pm_ops,
//#endif
	},
	.probe		= nuc980_pwm_probe,
	.remove		= nuc980_pwm_remove,
	.suspend        = nuc980_pwm_suspend,
	.resume         = nuc980_pwm_resume,
};



module_platform_driver(nuc980_pwm1_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nuvoton Technology Corp.");
MODULE_ALIAS("platform:nuc980-pwm1");
