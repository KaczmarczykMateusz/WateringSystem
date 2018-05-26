#include "fat16.h"

uint8_t fat16_buffer[FAT16_BUFFER_SIZE];
Fat16State fat16_state;

/*************************************************************************
 Function: fat16_init()
 Purpose:  Initialize the library - locates the first FAT16 partition,
 loads the relevant part of its boot sector to calculate
 values needed for operation, and finally positions the
 file reading routines to the start of root directory entries
Return: 0 if success, negative value if failure
**************************************************************************/
int8_t fat16_init() {
    int8_t i;
    uint32_t root_start;

    fat16_seek(0x1BE);

    for(i=0; i<4; i++) {
        fat16_read(sizeof(PartitionTable));

        if(FAT16_part->partition_type == 4 ||
           FAT16_part->partition_type == 6 ||
           FAT16_part->partition_type == 14)
            break;
    }

    if(i == 4) {// none of the partitions were FAT16
        return FAT16_ERR_NO_PARTITION_FOUND;
    }

    fat16_state.fat_start = 512 * FAT16_part->start_sector; // temporary

    fat16_seek(fat16_state.fat_start + FAT16_BOOT_OFFSET);
    fat16_read(sizeof(Fat16BootSectorFragment));

    if(FAT16_boot->sector_size != 512)
        return FAT16_ERR_INVALID_SECTOR_SIZE;

    fat16_state.fat_start += FAT16_boot->reserved_sectors * 512;

    root_start = fat16_state.fat_start + (uint32_t)FAT16_boot->fat_size_sectors *
        (uint32_t)FAT16_boot->number_of_fats * 512;

    fat16_state.data_start = root_start + sizeof(Fat16Entry) *
        (uint32_t)FAT16_boot->root_dir_entries;

    fat16_state.sectors_per_cluster = FAT16_boot->sectors_per_cluster;

    // Prepare for fat16_open_file(), cluster is not needed
    fat16_state.file_left = FAT16_boot->root_dir_entries * sizeof(Fat16Entry);
    fat16_state.cluster_left = 0xFFFFFFFF; // avoid FAT lookup with root dir

#ifdef DEBUG
    printf("FAT start at %08X, root dir at %08X, data at %08X\n",
           fat16_state.fat_start, root_start, fat16_state.data_start);
#endif

    fat16_seek(root_start);

    return 0;
}

/*************************************************************************
 Function: fat16_open_file()
 Purpose:  Assumes we are at the beginning of a directory and fat16_state.file_left
 is set to amount of file entries. Reads on until a given file is found,
 or no more file entries are available.
 Input
 Return:
 **************************************************************************/
int8_t fat16_open_file(int8_t *filename, int8_t *ext) {
    int8_t i, bytes;

#ifdef DEBUG
    printf("Trying to open file [%s.%s]\n", filename, ext);
#endif

    do {
        bytes = fat16_read_file(sizeof(Fat16Entry));

        if(bytes < sizeof(Fat16Entry)) {
            return FAT16_ERR_FILE_READ;
        }

#ifdef DEBUG
        if(FAT16_entry->filename[0]) {
            printf("Found file [%.8s.%.3s]\n", FAT16_entry->filename, FAT16_entry->ext);
        }
#endif

        for(i=0; i<8; i++) {// we don't have memcmp on a MCU...
            if(FAT16_entry->filename[i] != filename[i]) {
                break;
            }
        }
        if(i < 8) {// not the filename we are looking for
            continue;
        }

        for(i=0; i<3; i++) {// we don't have memcmp on a MCU...
            if(FAT16_entry->ext[i] != ext[i]) {
                break;
            }
        }
        if(i < 3) {// not the extension we are looking for
            continue;
        }

#ifdef DEBUG
        printf("File found at cluster %d!\n", FAT16_entry->starting_cluster);
#endif

        // Initialize reading variables
        fat16_state.cluster = FAT16_entry->starting_cluster;
        fat16_state.cluster_left = (uint32_t)fat16_state.sectors_per_cluster * 512;

        if(FAT16_entry->filename[0] == 0x2E ||
           (FAT16_entry->attributes & 0x10)) { // directory
            // set file size so large that the file entries
            // are not limited by it, but by the sectors used
            fat16_state.file_left = 0xFFFFFFFF;
        } else {
            fat16_state.file_left = FAT16_entry->file_size;
        }

        // Go to first cluster
        fat16_seek(fat16_state.data_start + (uint32_t)(fat16_state.cluster-2) *
            (uint32_t)fat16_state.sectors_per_cluster * 512);
        return 0;
    } while(fat16_state.file_left > 0);

#ifdef DEBUG
    printf("File not found: [%s.%s]!\n", filename, ext);
#endif

    return FAT16_ERR_FILE_NOT_FOUND;
}

/*************************************************************************
 Function: fat16_read_file()
 Purpose: Read content of SD card as text byte by byte
 Input:	 Size of buffer for reading as number of bytes
 Return: Bytes left to read
 See:	 FAT16_BUFFER_SIZE
 **************************************************************************/
int8_t fat16_read_file(int8_t bytes) {
#ifdef DEBUG
    //printf("fat16_read_file: Cluster %d, bytes left %d/%d\n", fat16_state.cluster, fat16_state.file_left, fat16_state.cluster_left);
#endif

    if(fat16_state.file_left == 0) {
        return 0;
    }

    if(fat16_state.cluster_left == 0) {
        fat16_seek(fat16_state.fat_start + (uint32_t)fat16_state.cluster*2);
        fat16_read(2);

        fat16_state.cluster = FAT16_ushort[0];
        fat16_state.cluster_left = (uint32_t)fat16_state.sectors_per_cluster * 512;

        if(fat16_state.cluster == 0xFFFF) { // end of cluster chain
            fat16_state.file_left = 0;
            return 0;
        }

        // Go to first cluster
        fat16_seek(fat16_state.data_start + (uint32_t)(fat16_state.cluster-2) *
            (uint32_t)fat16_state.sectors_per_cluster * 512);

#ifdef DEBUG
        printf("Next cluster %d\n", fat16_state.cluster);
#endif
    }

    if(bytes > fat16_state.file_left) {
        bytes = fat16_state.file_left;
    }
    if(bytes > fat16_state.cluster_left) {
        bytes = fat16_state.cluster_left;
    }

    fat16_read(bytes);

    fat16_state.file_left -= bytes;
    fat16_state.cluster_left -= bytes;

#ifdef DEBUG
    //printf("%d bytes read: Cluster %d, bytes left %d/%d\n", bytes, fat16_state.cluster, fat16_state.file_left, fat16_state.cluster_left);
#endif

    return bytes;
}

/*************************************************************************
 Function: fat16_seek()
 Purpose:  Go to specified offset. Next fat16_read should continue from here
 Input:	   Relative linear offset to the start of the partition.
 	 	   Next fat16_read should continue from here
 **************************************************************************/
void fat16_seek(uint32_t offset) {
	sd_sector = offset >> 9;
	sd_pos = offset & 0x1FF;
}

/*************************************************************************
 Function: fat16_read()
 Purpose:
 Input:	Amount of bytes to read
 **************************************************************************/
void fat16_read(uint8_t bytes) {
	SD_read(sd_sector, sd_pos, fat16_buffer, bytes);
	sd_pos += (uint16_t)bytes;

	if(sd_pos == 512) {
		sd_pos = 0;
		sd_sector++;
	}
}

/*************************************************************************
 Function: SD_init()
 Purpose: Initialise SD  card
 Return:  0 if success, negative value if failure
 **************************************************************************/
int8_t SD_init() {
	int8_t i;

	// ]r:10
	CS_DISABLE();
	for(i=0; i<10; i++) {// idle for 1 bytes / 80 clocks
		SPI_write(0xFF);
	}

	//Send clock pulses for maximum 80 cycles (“read” 10 bytes) by writing zero
	//SD card should respond within this time with "1" indicating successful initialisation
	for(i=0; i<10 && SD_command(0x40, 0x00000000, 0x95, 8) != 1; i++) {
		_delay_ms(100);
	}

	if(i == 10) {// card did not respond to initialisation
		return -1;
	}

	//Send CMD1 until it returns 0 (it returns 1 while the card is busy)
	for(i=0; i<10 && SD_command(0x41, 0x00000000, 0xFF, 8) != 0; i++)
		_delay_ms(100);

	if(i == 10) {// card did not come out of idle
		return -2;
	}

	// Set block length to 512
	SD_command(0x50, 0x00000200, 0xFF, 8);

	sd_sector = sd_pos = 0;

	return 0;
}

/*************************************************************************
 Function: SD_read()
 Purpose: Reading data from SD card into "*buffer"
 Input:	Current sector being read
		Offset within current sd_sector
		Buffer in which we will be saving reads
		Length of data unit as bits
 // TODO: This function will not exit gracefully if SD card does not do what it should
 **************************************************************************/
void SD_read(uint32_t sector, uint16_t offset, uint8_t *buffer, uint16_t length) {
	uint16_t i;

	CS_ENABLE();
	SPI_write(0x51);
	SPI_write(sector >> 15); // sector*512 >> 24
	SPI_write(sector >> 7);  // sector*512 >> 16
	SPI_write(sector << 1);  // sector*512 >> 8
	SPI_write(0);          // sector*512
	SPI_write(0xFF);

	for(i=0; i<100 && SPI_write(0xFF) != 0x00; i++) {
	} // wait for 0

	for(i=0; i<100 && SPI_write(0xFF) != 0xFE; i++) {
	} // wait for data start

	for(i=0; i<offset; i++)  {// "skip" bytes
		SPI_write(0xFF);
	}

	for(i=0; i<length; i++) { // read length bytes
		buffer[i] = SPI_write(0xFF);
	}

	for(i+=offset; i<512; i++) {// "skip" again
		SPI_write(0xFF);
	}

	// skip checksum
	SPI_write(0xFF);
	SPI_write(0xFF);

	CS_DISABLE();
}

/*************************************************************************
 Function: SD_command()
 Purpose: Send command to SD card
 Input: SD bus protocol command, dword argument, CRC byte, Number of bits for reading
 Return: Slave (SD card) response, "1" while SDsuccessfull Card busy
 **************************************************************************/
uint8_t SD_command(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t read) {
	uint8_t i, buffer[32], ret = 0xFF;

	uartTxStr("CMD ");
	uartTxHex(cmd);

	CS_ENABLE();
	SPI_write(cmd);
	SPI_write(arg>>24);
	SPI_write(arg>>16);
	SPI_write(arg>>8);
	SPI_write(arg);
	SPI_write(crc);

	for(i=0; i<read; i++) {
		buffer[i] = SPI_write(0xFF);
	}

	CS_DISABLE();

	for(i=0; i<read; i++) {
		uartTxChar(' ');
		uartTxHex(buffer[i]);
		if(buffer[i] != 0xFF) {
			ret = buffer[i];
		}
	}

	uartTxStr("\r\n");

	return ret;
}

