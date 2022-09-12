deps_config := \
	src/device/Kconfig \
	src/memory/Kconfig \
	/home/jojo/Documents/Project/ICS/nemu/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
