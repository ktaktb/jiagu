#ifndef DEXDEC_H_
#define DEXDEC_H_

typedef struct bin_file_type
{
	int key_start;
	int key_size;
	int sosign_start;
	int sosign_size;
	int dexsign_start;
	int dexsign_size;
	int dex_start;
	int dex_size;
	int file_size;
	int unzdex_size;
}BIN_FILE_TYPE;

void analysis_bin_file(const char *binfilename, BIN_FILE_TYPE *binfiletype);

#endif
