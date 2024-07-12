# amlogic partitioner 

这是一个根据amlogic [EPT](https://7ji.github.io/embedded/2022/11/11/ept-with-ampart.html) 分区表配置mbr 分区表的工具

脚本读取EPT中分区`armbian_boot` 与`armbian_rootfs` 给mmc 分别设置分区boot 与rootfs

如果分区格式为ext4 或btrfs ，支持自动resize 文件系统至分区大小

