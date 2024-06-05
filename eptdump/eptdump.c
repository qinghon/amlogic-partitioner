#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/* Definition */

#define EPT_PARTITION_GAP_GENERIC     0x800000U    // 8M
#define EPT_PARTITION_GAP_RESERVED    0x2000000U   // 32M

#define EPT_PARTITION_BOOTLOADER_SIZE 0x400000U    // 4M
#define EPT_PARTITION_BOOTLOADER_NAME "bootloader"
#define EPT_PARTITION_RESERVED_NAME   "reserved"
#define EPT_PARTITION_RESERVED_SIZE   0x4000000U   // 64M
#define EPT_PARTITION_CACHE_NAME      "cache"
#define EPT_PARTITION_CACHE_SIZE      0U
#define EPT_PARTITION_ENV_NAME        "env"
#define EPT_PARTITION_ENV_SIZE        0x800000U    // 8M

#define EPT_HEADER_MAGIC_UINT32       (uint32_t)0x0054504DU

/* MACRO */
#define EPT_IS_PEDANTIC(x)  !ept_is_not_pedantic(x)

/* Structure */

#define MAX_PARTITION_NAME_LENGTH 16
#define MAX_PARTITIONS_COUNT      32

struct ept_header {
	union {
		char magic_string[4];
		uint32_t magic_uint32;
	};
	union {
		char version_string[12];
		uint32_t version_uint32[3];
	};
	uint32_t partitions_count;
	uint32_t checksum;
};

struct ept_partition {
	char name[MAX_PARTITION_NAME_LENGTH];
	uint64_t size;
	uint64_t offset;
	uint32_t mask_flags;
	uint32_t padding;
};

struct ept_table {
	struct ept_header header;
	struct ept_partition partitions[MAX_PARTITIONS_COUNT];

};

void usage(void )
{
	puts("usage: eptdump <mmc dump file>|<mmc device>");
}
void dump_table(struct ept_table *table)
{
	struct ept_partition *p;
	if (table->header.partitions_count > 0 && table->header.partitions_count <= 32){
		fputs("name\tsize\toffset\tmask\tpadding\n", stderr);
	}
	for (uint8_t i = 0; i < (uint8_t)table->header.partitions_count; ++i) {
		p = &table->partitions[i];
		printf("%s\t%ld\t%ld\t%x\t%d\n", p->name, p->size, p->offset, p->mask_flags, p->padding);
	}
}

int main(int argc, char *argv[])
{
	if (argc == 1){
		usage();
		exit(-EINVAL);
	}
	const char * path = argv[1];
	if (path == NULL) {
		usage();
		exit(-EINVAL);
	}

	struct ept_table table = {0};

	int fd;
	if (*path == '-')
		fd = STDIN_FILENO;
	else
		fd = open(path, O_RDONLY);
	if (fd < 0){
		fprintf(stderr, "cannot open file %s", path);
		goto err;
	}

	if (lseek(fd, EPT_PARTITION_BOOTLOADER_SIZE + EPT_PARTITION_GAP_RESERVED, SEEK_SET) < 0){
		fprintf(stderr, "cannot seek file %s", path);
		goto err;
	}
	if (read(fd, &table, sizeof(table)) != sizeof(table)){
		fprintf(stderr, "cannot read ept table %s", path);
		goto err;
	}
	if (table.header.magic_uint32 != EPT_HEADER_MAGIC_UINT32){
		fprintf(stderr, "cannot parse ept table, magic %x != %x", table.header.magic_uint32, EPT_HEADER_MAGIC_UINT32);
		errno = EPERM;
		goto err;
	}
	dump_table(&table);
	return 0;
err:
	exit(-errno);
	return -errno;
}