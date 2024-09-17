CC=gcc

default: rf_canc_gen1_config

new: rf_canc_gen3_config

rf_canc_gen1_config: rf_canc_gen1_config.c
	$(CC) -o rf_canc_gen1_config rf_canc_gen1_config.c -lsub -lusb-1.0 -lm -L\usr\local\lib -I.

rf_canc_gen3_config: rf_canc_gen3_config.c
	$(CC) -o rf_canc_gen3_config rf_canc_gen3_config.c -lsub -lusb-1.0 -lm -L\usr\local\lib -I.

.PHONY: clean
clean:
	rm -f rf_canc_gen1_config rf_canc_gen3_config
