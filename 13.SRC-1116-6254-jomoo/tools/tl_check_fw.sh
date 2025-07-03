#!/bin/bash 
###
 # @Author: lihuashe lihuashe@seneasy.com
 # @LastEditors: lihuashe
 # @LastEditTime: 2024-09-23 17:09:40
 # @file: .c
 # @Description: Copyright 2024 Guangdong Seneasy Intelligent Technology Co., Ltd. All rights reserved.
### 
echo "*****************************************************"
echo "this is post build!! current configure is :$1"

echo $(pwd)
echo $0
tc32-elf-objcopy -v -O binary $2.elf  $1.bin
$(dirname $0)/tl_check_fw2.exe  $1.bin
echo "**************** end of post build ******************"
