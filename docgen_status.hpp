#pragma once

#define status_check() if (status != DG_OK) { goto cleanup; }

static enum dg_status {
	DG_OK,
	DG_SYS_ERR,
	DG_FILE_ERR,
	DG_FILE_NO,
	DG_FLAGS_NO
} status = DG_OK;

static inline const char *status_msg()
{
	switch (status) {
		case DG_OK:
			return "Docgen status is okay.";
		case DG_SYS_ERR:
			return "Docgen encountered an unexpected system error.";
		case DG_FILE_ERR:
			return "Docgen encountered an unexpected file I/O error.";
		case DG_FILE_NO:
			return "Docgen encountered a problem opening a file.";
		case DG_FLAGS_NO:
			return "Docgen encountered a problem parsing flags.";
		default:
			return "Docgen encountered an unexpected status code.";
	}
}

static inline void report_status()
{
	if (status != DG_OK) {
		std::cerr << status_msg() << '\n';
	}
}
