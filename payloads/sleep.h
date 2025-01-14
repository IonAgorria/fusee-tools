#ifndef SLEEP_H
#define SLEEP_H

#define TIMERUS_CNTR_1US_0 (0x60005010)

uint32_t get_usec() {
	return reg_read(TIMERUS_CNTR_1US_0, 0);
}

void usleep(uint32_t usec) {
	uint32_t start = get_usec();

	while ((get_usec() - start) <= usec);
}

void msleep(uint32_t msec) {
	usleep(msec * 1000);
}

#endif //SLEEP_H
