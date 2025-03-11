C based demo code for a decent ADC/DAC. 

Device note: The NXT IC is solid!

General code note: Be warned about data type bytes, (mis)using basic addition can cause "rollover" e.g. 255+3 -> 2 !
(It would be wise to add a function that prevents accidental rollover)
