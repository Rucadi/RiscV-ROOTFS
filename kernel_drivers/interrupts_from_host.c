#include <linux/init.h>
#include <linux/module.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/irqdomain.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>




struct s_host_irq_data
{
    u64  irq;
    u64  io_size;
    u64 io_register;
    u64 io_remapped_register;
    spinlock_t lock;

};


static struct s_host_irq_data *host_irq_data;


static irqreturn_t host_irq_isr(int irq, void *o)
{
    unsigned long flags;
    spin_lock_irqsave(&host_irq_data->lock, flags);

    //if(first_time)
    //{
        printk("Interrupt should be handled here\n");
        printk("Interrupt IO_REMAPPED_REGISTER is : %llx\n", host_irq_data->io_remapped_register );

  //  }
    *((int*) host_irq_data->io_remapped_register) = 0;

    spin_unlock_irqrestore(&host_irq_data->lock, flags);

    return IRQ_HANDLED;
}



static int hostirq_request_port(struct s_host_irq_data * device)
{
    if (!request_mem_region(device->io_register, device->io_size, "meep_host_irq")) 
    {
		printk("memory region busy %llx\n", device->io_register);
		return -EBUSY;
	}
    
    device->io_remapped_register  = (u64) ioremap( device->io_register, device->io_size);
    
    if(!device->io_remapped_register)
    {
		release_mem_region(device->io_register, device->io_size);
		return -EBUSY;
    }

    return 0;
}

static int hostirq_probe(struct platform_device *pdev)
{
	struct resource *res;
	int irq, suc, ret;

    host_irq_data = devm_kzalloc(&pdev->dev, sizeof(struct s_host_irq_data),  GFP_KERNEL);

    spin_lock_init(&host_irq_data->lock);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENODEV;
    
    host_irq_data->io_size = 64;
    host_irq_data->io_register = res->start;

    printk("Registered Base Address IO REG %llx", host_irq_data->io_register);


    suc = hostirq_request_port(host_irq_data);
    
    if(suc)
    {
        printk("ERROR: Cannot request memory\n");
        return -ENODEV;
    }

	irq = platform_get_irq(pdev, 0);
	if (irq <= 0)
		return -ENXIO;

    host_irq_data->irq = irq;
    

	ret = request_irq(irq, host_irq_isr,  IRQF_TRIGGER_HIGH, "meep_host_irq", host_irq_data);

    printk("Registered IRQ %d", irq);

    if(ret)
    {
        printk("ERROR: Cannot request IRQ %d", irq);
        printk(" - code %d , EIO %d , EINVAL %d\n", ret, EIO, EINVAL);
    }


	return ret;
}

static int hostirq_remove(struct platform_device *pdev)
{
    int irq;
	struct resource *res;

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENODEV;

    irq = platform_get_irq(pdev, 0);
    if (irq <= 0)
		return -ENXIO;

	free_irq(irq,  host_irq_data);

    return 0;
}


/* work with hotplug and coldplug */
MODULE_ALIAS("platform:hostirq");



#if defined(CONFIG_OF)
/* Match table for of_platform binding */
static const struct of_device_id hostirq_of_match[] = {
	{ .compatible = "meep,meep-host-irq", },
	{}
};
MODULE_DEVICE_TABLE(of, hostirq_of_match);
#endif /* CONFIG_OF */



static struct platform_driver hostirq_platform_driver = {
	.probe = hostirq_probe,
	.remove = hostirq_remove,
	.driver = {
		.name  = "meep_hostirq",
		.of_match_table = of_match_ptr(hostirq_of_match),
	},
};



static int __init hostirq_init(void)
{

	pr_debug("hostirq: calling driver register()\n");
	return platform_driver_register(&hostirq_platform_driver);
}

static void __exit hostirq_exit(void)
{
	platform_driver_unregister(&hostirq_platform_driver);
}

module_init(hostirq_init);
module_exit(hostirq_exit);



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ruben Cano");
MODULE_DESCRIPTION("TEST IRQ ROCKET");
