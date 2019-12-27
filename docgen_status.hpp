#pragma once

#define status_check() if (status != DG_OK) { goto cleanup; }
#define status_report() if (status != DG_OK) { std::cerr << status_str() << '\n'; }

static enum dg_status {
	DG_OK,
	DG_SYS_ERR,
	DG_FLAGS_NO
} status = DG_OK;

static inline const char *status_str()
{
	switch (status) {
		case DG_OK:
			return "Docgen status is okay.";
		case DG_SYS_ERR:
			return "Docgen encountered an unexpected system error.";
		case DG_FLAGS_NO:
			return "Docgen encountered a problem parsing flags.";
		default:
			return "Docgen encountered an unexpected status code.";
	}
}
