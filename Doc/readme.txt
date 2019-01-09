-take out stm32l1xx_hal_conf.h so that the one in freertos project is used. rename original file to stm32l1xx_hal_conf_orig.h
-LEAVIT AT: In LSM6DSL_FIFOMode, analyze if stm32l1xx_it.h is needed. All other files have been analyzed and now this Inc folder can be included in the headers 
of this project. If this specific file does not need to be included , rename it with orig postscript.