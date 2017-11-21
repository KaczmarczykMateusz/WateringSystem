
/*
 ============================================================================
 Name        :	fat16.h
 Author      :	Mateusz Kaczmarczyk
 Version     :
 Description :
 Notice		 : Ensure that packed attribute is working properly
			   because there are various versions of it and after
			   implementing wrong one compiler won't show warning
 ============================================================================
 */

#ifndef __FAT16_H
#define __FAT16_H

#include <util/delay.h>
#include "spi.h"
#include "uart.h"

#define FAT16_BUFFER_SIZE 32

//@brief: Bytes omitted from the start of Fat16BootSectorFragment
#define FAT16_BOOT_OFFSET 11

//@brief: Aliases for fat16_buffer in different formats
#define FAT16_part ((PartitionTable *)((void *)fat16_buffer))
#define FAT16_boot ((Fat16BootSectorFragment *)((void *)fat16_buffer))
#define FAT16_entry ((Fat16Entry *)((void *)fat16_buffer))
#define FAT16_ushort ((uint8_t *)((void *)fat16_buffer))

//@brief: Error codes for fat16_init()
#define FAT16_ERR_NO_PARTITION_FOUND -1
#define FAT16_ERR_INVALID_SECTOR_SIZE -2

//@brief: Error codes for fat16_open_file()
#define FAT16_ERR_FILE_NOT_FOUND -1
#define FAT16_ERR_FILE_READ -2

int32_t sd_sector;	//@brief: Holds address of current sector
uint8_t sd_pos;		//@brief:

//@brief: Partition information,it usually starts at offset 0x1BE
typedef struct {
    uint8_t first_byte;		//@brief: 0x80 if active (bootable), 0 otherwise
    uint8_t start_chs[3];	//@brief: Start of the partition in CHS-addressing
    uint8_t partition_type;	//@brief: Type of the partition
    uint8_t end_chs[3];		//@brief: End of the partition in CHS-addressing
    uint32_t start_sector;	//@brief: Relative offset to the partition in sectors (LBA)
    uint32_t length_sectors;//@brief: size of the partition in sectors
} __attribute((packed)) PartitionTable; // TODO: concern replacing "gcc_struct, __packed__" with only "packed"

//@brief: Partial FAT16 boot sector structure - non-essentials commented out
typedef struct {
#if 0
    uint8_t jmp[3];				//@brief: Intel 80x86 jump instruction
    int8_t oem[8];				//@brief: OEM name (not the volume name, see offset 0x02B)
#endif
    uint16_t sector_size;		//@brief: Size of single sector (usually 512 bytes)
    uint8_t sectors_per_cluster;//@brief: Number of sectors per cluster
    uint16_t reserved_sectors;	//@brief: Number of reserved sectors (including the boot sector)
    uint8_t number_of_fats;		//@brief: Number of File Allocation Tables recorded on partition
    uint16_t root_dir_entries;	//@brief: Number of directory entries in the root directory (N.A. for FAT32)
    uint16_t total_sectors_short; //@brief: If zero, later field is used, Total number of sectors on the disk/partition,
    							//or zero for disks/partitions bigger than 32 MiB (the field at offset 0x020 should then be used)
    uint8_t media_descriptor;	//@brief: Media descriptor (bit #2 holds whether the disk is removable)
    uint16_t fat_size_sectors;

#if 0
    uint16_t sectors_per_track;	//@brief: Number of sectors per track (cylinder), CHS addressing
    uint16_t number_of_heads;	//@brief: Number of heads, CHS addressing
    uint32_t hidden_sectors;	//@brief: Number of hidden sectors, which is the number of sectors before the boot sector (this field may be set to zero, though)
    uint32_t total_sectors_long;//@brief: Total number of sectors on the disk/partition, if this is above 32 MiB (only valid if the field at offset 0x013 is zero)

    uint8_t drive_number;		//@brief: Drive number (this field is at offset 0x024 in FAT12/FAT16)
    uint8_t current_head;		//@brief: Current head (internal to DOS; this field is at offset 0x025 in FAT12/FAT16)
    uint8_t boot_signature;		//@brief: Boot signature, the value 0x29 indicates the three next fields are valid (this field is at offset 0x026 in FAT12/FAT16)
    uint32_t volume_id;			//@brief: Volume ID (serial number; this field is at offset 0x027 in FAT12/FAT16)
    int8_t volume_label[11];	//@brief: Volume label (this field is at offset 0x02b in FAT12/FAT16)
    int8_t fs_type[8];			//@brief: File system type (this field is at offset 0x036 in FAT12/FAT16)
    int8_t boot_code[448];		//@brief: Boot code (starts at offset 0x03e in FAT12/FAT16, and is 448 bytes)
    uint16_t boot_sector_signature;	//@brief: Must be 0x55AA
#endif
} __attribute((packed)) Fat16BootSectorFragment;

//@brief: FAT16 file entry
typedef struct {
    uint8_t filename[8];	//@brief: Name of file for reading
    uint8_t ext[3];			//@brief: Extension of file for reading
    uint8_t attributes;		//@brief:
    uint8_t reserved[10];	//@brief:
    uint16_t modify_time;	//@brief: Time file was modified last time
    uint16_t modify_date;	//@brief: Date file was modified last time
    uint16_t starting_cluster;	//@brief: Offset of starting cluster
    uint32_t file_size;		//@brief: Total size of file as bytes
} __attribute((packed)) Fat16Entry;

//@brief: State data required by FAT16 library
typedef struct {
   uint32_t fat_start;		//@brief: FAT start position
   uint32_t data_start;		//@brief: Data start position
   uint8_t sectors_per_cluster;	//@brief: Cluster size in sectors
   uint16_t cluster; 		//@brief: Current cluster being read
   uint32_t cluster_left;	//@brief: Bytes left in current cluster
   uint32_t file_left;		//@brief:: Bytes left in the file being read
} __attribute((packed)) Fat16State;


//@brief: Global variables for read data and library state
extern uint8_t fat16_buffer[FAT16_BUFFER_SIZE];
extern Fat16State fat16_state;


/*
		**Following functions need to be provided for the library**
*/
/** @brief	: Go to specified offset. Next fat16_read should continue from here
 *  @param	: Relative linear offset to the start of the partition.. Next fat16_read should continue from here
 *  @return	: None
 */
void fat16_seek(uint32_t offset);

/** @brief	: Read <bytes> to fat16_buffer, and return the amount of bytes read
 *  @param	: Number of bytes to read
 */
void fat16_read(uint8_t bytes);


/*
		**Following functions are provided by the library**
*/
/** @brief	: Initialize the FAT16 library. Nonzero return value indicates
 * 			an error state. After init, the file stream is pointed to
 * 			the beginning of root directory entry.
 *  @param	: None
 *  @return	: 0: Success
 *  		  FAT16_ERR_NO_PARTITION_FOUND
 *  		  FAT16_ERR_INVALID_SECTOR_SIZE
 */
int8_t fat16_init(); // nonzero return values indicate error state

/** @brief	: Open a given file. Assumes that the stream is currently at
 * 			a directory entry and fat16_state.file_left contains the
 * 			amount of file entries available. Both conditions are satisfied
 * 			after fat16_init(), and also after opening a directory with this.
 *  @param	:
 *  @param	:
 *  @return	: 0: Success
 *  		  FAT16_ERR_FILE_NOT_FOUND
 *  		  FAT16_ERR_FILE_READ
 */
int8_t fat16_open_file(int8_t *filename, int8_t *ext);

/** @brief	: Read <bytes> from file. This method will automatically
 * 			  traverse all clusters of a file using the file allocation table
 *  @param	: Size of buffer as number of bytes
 *  @return	: Bytes left to read
 */
int8_t fat16_read_file(int8_t bytes);

/** @brief	: Initialise SD (not SDHC) card
 *  @param	: None
 *  @return	:  0: Success
 *  		  -1: Card did not respond to initialisation
 *  		  -2: Card did not come out of idle state
 *  @See:	 FAT16_BUFFER_SIZE
 */
int8_t SD_init();

/** @brief	: function to be used by fat16_read()
 *  @param	sector	: Current sector being read
 *  @param	offset	: Offset within current sd_sector
 *  @param	buffer	: Buffer in which we will be saving reads
 *  @param	length	: Length of data unit as bits
 *  @return	: None
 *  @see	: fat16_read()
 *	TODO: This function will not exit gracefully if SD card does not do what it should
 */
void SD_read(uint32_t sector, uint16_t offset, uint8_t *buffer, uint16_t length);


/** @brief  Sends command to SD card
 *  @param  cmd	: SD bus protocol command
 *  @param  arg	: argument dword
 *  @param  crc	: CRC byte
 *  @param  read: Number of bits for reading
 *  @return	Slave (SD card) response,
 	 	 	1: SD card busy
 */
uint8_t SD_command(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t read);

#endif
