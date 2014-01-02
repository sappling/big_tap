#include "pebble.h"


#define MIN_ACCEL 1000
#define ACCEL_TIMER 5000

static unsigned max_x=0, max_y=0, max_z = 0;
static int16_t last_x = 0, last_y = 0, last_z = 0;
static time_t last_samples = 0;
static AccelTapHandler accel_tap_handler = NULL;

static void accel_timeout_handler(void *ptr) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Acceleration %4d / %4d / %4d - time %ld", max_x, max_y, max_z, last_samples);
	/*
	if (max_x > MIN_ACCEL) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Accel X = %d", max_x);
	}
	if (max_y > MIN_ACCEL) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Accel Y = %d", max_y);
	}
	if (max_z > MIN_ACCEL) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Accel Z = %d", max_z);
	}
	*/
	max_x = max_y = max_z = 0;
	app_timer_register(ACCEL_TIMER, accel_timeout_handler, NULL);
}


static void accelHandle(AccelData *data, uint32_t num_samples) {
	unsigned delta_x, delta_y, delta_z;
	for (unsigned i=0; i<num_samples; i++) {
		AccelData next = data[i];
		if (i == 0) {
			delta_x = abs(next.x - last_x);
			delta_y = abs(next.y - last_y);
			delta_z = abs(next.z - last_z);
	    } else {
			AccelData prev = data[i-1];
			delta_x = abs(next.x - prev.x);
			delta_y = abs(next.y - prev.y);
			delta_z = abs(next.z - prev.z);
		}
		if (delta_x > max_x) { max_x = delta_x; }
		if (delta_y > max_y) { max_y = delta_y; }
		if (delta_z > max_z) { max_z = delta_z; }
		/*
		if (abs(next.x) > max_x) { max_x = abs(next.x); }
		if (abs(next.y) > max_y) { max_y = abs(next.y); }
		if (abs(abs(next.z) - 1000) > max_z) { max_z = abs(abs(next.z) - 1000); }
		*/
	}

	if (num_samples) {
		AccelData last = data[num_samples-1];
		last_x = last.x;
		last_y = last.y;
		last_z = last.z;
	}

	if (max_x > MIN_ACCEL || max_y > MIN_ACCEL || max_z > MIN_ACCEL) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Triggering with acceleration %4d / %4d / %4d", max_x, max_y, max_z);
		max_x = max_y = max_z = 0;

		//todo - get the parameters right
		accel_tap_handler(ACCEL_AXIS_Z, 1);
	}
	last_samples = time(NULL);
}

void tap_subscribe(AccelTapHandler handler) {
	accel_tap_handler = handler;

	accel_data_service_subscribe(25, accelHandle);
	app_timer_register(ACCEL_TIMER, accel_timeout_handler, NULL);

	accel_service_set_sampling_rate(ACCEL_SAMPLING_100HZ);
}
