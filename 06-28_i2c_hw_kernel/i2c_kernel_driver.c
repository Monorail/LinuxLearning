// https://www.kernel.org/doc/Documentation/i2c/writing-clients
static struct i2c_device_id foo_idtable[] = {
	{ "foo", my_id_for_foo },
	{ "bar", my_id_for_bar },
	{ }
};

MODULE_DEVICE_TABLE(i2c, foo_idtable);

static struct i2c_driver foo_driver = {
	.driver = {
		.name	= "foo",
		.pm	= &foo_pm_ops,	/* optional */
	},

	.id_table	= foo_idtable,
	.probe		= foo_probe,
	.remove		= foo_remove,
	/* if device autodetection is needed: */
	.class		= I2C_CLASS_SOMETHING,
	.detect		= foo_detect,
	.address_list	= normal_i2c//,

	// .shutdown	= foo_shutdown//,	/* optional */
	.command	= foo_command//,	/* optional, deprecated */
}