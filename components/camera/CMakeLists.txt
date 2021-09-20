set(srcs "bitmap.c"
		"camera.c"
		"ov2640.c"
		"ov7725.c"
		"sccb.c"
		"twi.c"
		"wiring.c"
		"xclk.c"
		)

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS  "./include"
                    REQUIRES console nvs_flash bt
                    REQUIRED_IDF_TARGETS esp32)
