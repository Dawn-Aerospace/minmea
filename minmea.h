/*
 * Copyright © 2014 Kosma Moczek <kosma@cloudyourcar.com>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

#ifndef MINMEA_H
#define MINMEA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#ifdef MINMEA_INCLUDE_COMPAT
#include <minmea_compat.h>
#endif

#define MINMEA_MAX_LENGTH 80

enum minmea_sentence_id {
    MINMEA_INVALID = -1,
    MINMEA_UNKNOWN = 0,
    MINMEA_SENTENCE_RMC,
    MINMEA_SENTENCE_GGA,
    MINMEA_SENTENCE_GSA,
    MINMEA_SENTENCE_GLL,
    MINMEA_SENTENCE_GST,
    MINMEA_SENTENCE_GSV,
    MINMEA_SENTENCE_VTG,
    MINMEA_SENTENCE_ZDA,

    // FLARM sentences
    MINMEA_SENTENCE_FLARM_LAU,  // Heartbeat, status, and basic alarms
    MINMEA_SENTENCE_FLARM_LAA,  // Data on other proximate aircraft
    MINMEA_SENTENCE_FLARM_LAE,  // Self-test result and errors codes
    MINMEA_SENTENCE_GARMIN_RMZ,   // Garmin's barometric altitude
    MINMEA_SENTENCE_FLARM_LAR, // Reset
    MINMEA_SENTENCE_FLARM_LAF  // Simulated traffic and alarms
};

struct minmea_float {
    int_least32_t value;
    int_least32_t scale;
};

struct minmea_date {
    int day;
    int month;
    int year;
};

struct minmea_time {
    int hours;
    int minutes;
    int seconds;
    int microseconds;
};

struct minmea_sentence_rmc {
    struct minmea_time time;
    bool valid;
    struct minmea_float latitude;
    struct minmea_float longitude;
    struct minmea_float speed;
    struct minmea_float course;
    struct minmea_date date;
    struct minmea_float variation;
};

struct minmea_sentence_gga {
    struct minmea_time time;
    struct minmea_float latitude;
    struct minmea_float longitude;
    int fix_quality;
    int satellites_tracked;
    struct minmea_float hdop;
    struct minmea_float altitude; char altitude_units;
    struct minmea_float height; char height_units;
    struct minmea_float dgps_age;
};

enum minmea_gll_status {
    MINMEA_GLL_STATUS_DATA_VALID = 'A',
    MINMEA_GLL_STATUS_DATA_NOT_VALID = 'V',
};

// FAA mode added to some fields in NMEA 2.3.
enum minmea_faa_mode {
    MINMEA_FAA_MODE_AUTONOMOUS = 'A',
    MINMEA_FAA_MODE_DIFFERENTIAL = 'D',
    MINMEA_FAA_MODE_ESTIMATED = 'E',
    MINMEA_FAA_MODE_MANUAL = 'M',
    MINMEA_FAA_MODE_SIMULATED = 'S',
    MINMEA_FAA_MODE_NOT_VALID = 'N',
    MINMEA_FAA_MODE_PRECISE = 'P',
};

struct minmea_sentence_gll {
    struct minmea_float latitude;
    struct minmea_float longitude;
    struct minmea_time time;
    char status;
    char mode;
};

struct minmea_sentence_gst {
    struct minmea_time time;
    struct minmea_float rms_deviation;
    struct minmea_float semi_major_deviation;
    struct minmea_float semi_minor_deviation;
    struct minmea_float semi_major_orientation;
    struct minmea_float latitude_error_deviation;
    struct minmea_float longitude_error_deviation;
    struct minmea_float altitude_error_deviation;
};

enum minmea_gsa_mode {
    MINMEA_GPGSA_MODE_AUTO = 'A',
    MINMEA_GPGSA_MODE_FORCED = 'M',
};

enum minmea_gsa_fix_type {
    MINMEA_GPGSA_FIX_NONE = 1,
    MINMEA_GPGSA_FIX_2D = 2,
    MINMEA_GPGSA_FIX_3D = 3,
};

struct minmea_sentence_gsa {
    char mode;
    int fix_type;
    int sats[12];
    struct minmea_float pdop;
    struct minmea_float hdop;
    struct minmea_float vdop;
};

struct minmea_sat_info {
    int nr;
    int elevation;
    int azimuth;
    int snr;
};

struct minmea_sentence_gsv {
    int total_msgs;
    int msg_nr;
    int total_sats;
    struct minmea_sat_info sats[4];
};

struct minmea_sentence_vtg {
    struct minmea_float true_track_degrees;
    struct minmea_float magnetic_track_degrees;
    struct minmea_float speed_knots;
    struct minmea_float speed_kph;
    enum minmea_faa_mode faa_mode;
};

struct minmea_sentence_zda {
    struct minmea_time time;
    struct minmea_date date;
    int hour_offset;
    int minute_offset;
};

// Flarm sentences
struct minmea_sentence_lau {
    int rx;  // [0->99] Number of devices with unique ID's currently received.
    int tx;  // [0,1]
    int gps;  // [0,2]
    int power;  // [0,1]
    int alarm_level;  // [0,3]
    int relative_bearing;  // [-180,180]
    char alarm_type[3];  // Hex, [0, FF]
    int relative_vertical;  // [-32768,32767]
    int relative_distance;  // [0,2147483647]
    char id[7]; // 6-digit hexadecimal value, omitted for protocol version < 4
};

struct minmea_sentence_laa {
    int alarm_level;  // [0-3]
    int relative_north;  // [-20000000,20000000]
    int relative_east;  // [-20000000,20000000]
    int relative_vertical;  // [-32768,32767]
    int idtype;  // [0-2]
    char id[7];  // 6-digit hexadecimal value
    int track;  // [0-359]
    int turn_rate;  // Empty, nothing here
    int ground_speed;  // [0,32767]
    float climb_rate;  // [-32.7-32.7]
    int acft_type;  //
    int no_track;
    int source;
    int rssi;

    // Not all versions of the protocol have all fields,
    // this field indicated which fields are exluded
    // 0: Excludes, no_track, source and rssi.
    // 1: Excludes, source and rssi.
    // 2: All included.
//    int includes;
};

struct minmea_sentence_lae {
    char query_type;  // [RA]
    int severity;  // [0-3]
    char error_code[4];  // Hex, [0-FFF]
    char message[41];
};

struct minmea_sentence_rmz {
    int barometric_altitude;
    char unit;
    int position_fix_dimension;
};

//struct minmea_sentence_lar {
//
//};
//
//struct minmea_sentence_laf {
//
//};




/**
 * Calculate raw sentence checksum. Does not check sentence integrity.
 */
uint8_t minmea_checksum(const char *sentence);

/**
 * Check sentence validity and checksum. Returns true for valid sentences.
 */
bool minmea_check(const char *sentence, bool strict);

/**
 * Determine talker identifier.
 */
bool minmea_talker_id(char talker[3], const char *sentence);

/**
 * Determine sentence identifier.
 */
enum minmea_sentence_id minmea_sentence_id(const char *sentence, bool strict);

/**
 * Scanf-like processor for NMEA sentences. Supports the following formats:
 * c - single character (char *)
 * d - direction, returned as 1/-1, default 0 (int *)
 * f - fractional, returned as value + scale (int *, int *)
 * i - decimal, default zero (int *)
 * s - string (char *)
 * t - talker identifier and type (char *)
 * T - date/time stamp (int *, int *, int *)
 * Returns true on success. See library source code for details.
 */
bool minmea_scan(const char *sentence, const char *format, ...);

/*
 * Parse a specific type of sentence. Return true on success.
 */
bool minmea_parse_rmc(struct minmea_sentence_rmc *frame, const char *sentence);
bool minmea_parse_gga(struct minmea_sentence_gga *frame, const char *sentence);
bool minmea_parse_gsa(struct minmea_sentence_gsa *frame, const char *sentence);
bool minmea_parse_gll(struct minmea_sentence_gll *frame, const char *sentence);
bool minmea_parse_gst(struct minmea_sentence_gst *frame, const char *sentence);
bool minmea_parse_gsv(struct minmea_sentence_gsv *frame, const char *sentence);
bool minmea_parse_vtg(struct minmea_sentence_vtg *frame, const char *sentence);
bool minmea_parse_zda(struct minmea_sentence_zda *frame, const char *sentence);

// Flarm sentences
bool minmea_parse_lau(struct minmea_sentence_lau *frame, const char *sentence);
bool minmea_parse_laa(struct minmea_sentence_laa *frame, const char *sentence);
bool minmea_parse_lae(struct minmea_sentence_lae *frame, const char *sentence);
bool minmea_parse_rmz(struct minmea_sentence_rmz *frame, const char *sentence);
//bool minmea_parse_lar(struct minmea_sentence_lar *frame, const char *sentence);
//bool minmea_parse_laf(struct minmea_sentence_laf *frame, const char *sentence);

/**
 * Convert GPS UTC date/time representation to a UNIX timestamp.
 */
int minmea_gettime(struct timespec *ts, const struct minmea_date *date, const struct minmea_time *time_);

/**
 * Rescale a fixed-point value to a different scale. Rounds towards zero.
 */
static inline int_least32_t minmea_rescale(struct minmea_float *f, int_least32_t new_scale)
{
    if (f->scale == 0)
        return 0;
    if (f->scale == new_scale)
        return f->value;
    if (f->scale > new_scale)
        return (f->value + ((f->value > 0) - (f->value < 0)) * f->scale/new_scale/2) / (f->scale/new_scale);
    else
        return f->value * (new_scale/f->scale);
}

/**
 * Convert a fixed-point value to a floating-point value.
 * Returns NaN for "unknown" values.
 */
static inline float minmea_tofloat(struct minmea_float *f)
{
    if (f->scale == 0)
        return NAN;
    return (float) f->value / (float) f->scale;
}

/**
 * Convert a raw coordinate to a floating point DD.DDD... value.
 * Returns NaN for "unknown" values.
 */
static inline float minmea_tocoord(struct minmea_float *f)
{
    if (f->scale == 0)
        return NAN;
    int_least32_t degrees = f->value / (f->scale * 100);
    int_least32_t minutes = f->value % (f->scale * 100);
    return (float) degrees + (float) minutes / (60 * f->scale);
}

#ifdef __cplusplus
}
#endif

#endif /* MINMEA_H */

/* vim: set ts=4 sw=4 et: */
