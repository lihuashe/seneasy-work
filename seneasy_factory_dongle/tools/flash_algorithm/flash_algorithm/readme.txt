Notes:

<20200624>
1. 本工程用于创建 flash 下载算法，提供给 KEIL IDE 做程序下载使用.
2. Flash memory MAP 见《Memory Map List.xlsx》文件.


备注：
1. 基于 [20190825]Apollo_Flash_Algorithm_R2_V1.1 工程编写。
2. 本工程采用 Keil 5.30.0.0 开发，工具路径在钉盘-->软件团队共享文件夹-->Tools-->IDE-->MDK5330.exe。
3. 工程编译后将生成 MXD_ARES_FLASH(FPGA)_R4_V1.0.FLM 文件，并自动拷贝到 C:\Keil_v5\ARM\Flash 路径，
   拷贝路径可在 keil-->Options for Target-->User 下修改。
4. SDK 在使用时需要事先添加 .FLM 下载算法文件到 Keil 中。

命令规则：
[release时间]Ares_Flash_Algorithm_[ROM版本]_[算法版本]
eg: [20200624]Ares_Flash_Algorithm_R3_V1.0
    发布时间 : 2020.06.24
    ROM 版本 : R4
    算法版本 : V1.0
    生成对应Keil算法名称 : MXD_ARES_FLASH_FPGA_R3_V1.0.flm or MXD_ARES_FLASH_R3_V1.0.flm