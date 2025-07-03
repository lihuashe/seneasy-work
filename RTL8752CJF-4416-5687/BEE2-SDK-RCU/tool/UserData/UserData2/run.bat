1>NUL del .\user_data.bin
1>NUL del .\user_data_OTA.bin

..\..\prepend_header\prepend_header.exe -t app_data1 -p "raw_user_data2.bin" -m 1 -c sha256 -u 0xFFFD
1>NUL ren raw_user_data2_MP.bin user_data2_OTA.bin
..\..\md5\md5.exe "user_data2_OTA.bin"

1>NUL ren raw_user_data2.tmp user_data2.bin





