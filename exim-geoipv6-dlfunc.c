/*
 * exim-geoipv6-dlfunc.c - MaxMind GeoIP dlfunc for Exim
 *
 * Copyright (C) 2012 Janne Snabb <snabb@epipe.com>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Additional code and updates by Andrew David Nimmo <andrew@nimmo.dev>
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* Headers for inet_pton(3): */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* MaxMind maxminddb C API header: */
#include "maxminddb.h"

/* Exim4 dlfunc API header: */
#define LOCAL_SCAN 1
#include "local_scan.h"

/*****************************************************************************
 * Configuration settings:
 *****************************************************************************/

/* GeoIP2 database */
#define MAXMINDDB_PATH US"/usr/share/GeoIP/GeoLite2-Country.mmdb"

/* default code returned when country is unknown: */
#define COUNTRY_CODE_UNKNOWN		US"--"

/* default code returned on lookup failures due to missing database: */
#define COUNTRY_CODE_LOOKUP_FAILED	US"--"

/*****************************************************************************
 * Country code lookup function:
 *****************************************************************************/

int
geoip_country_code(uschar **yield, int argc, uschar *argv[])
{
  if (argc != 1) {
    *yield = string_copy(US"Invalid number of arguments");

    return ERROR;
  }

  /* sugar */
  char *ip_address = argv[0];

  MMDB_s mmdb;
  int status =
    MMDB_open(MAXMINDDB_PATH, MMDB_MODE_MMAP, &mmdb);

  if (MMDB_SUCCESS != status) {
    log_write(0, LOG_MAIN, US"geoipv6: Failed to open GeoIP2 database");
    *yield = string_copy(COUNTRY_CODE_LOOKUP_FAILED);

    return OK;
  }

	
  int gai_error, mmdb_error;
  MMDB_lookup_result_s result =
    MMDB_lookup_string(&mmdb, ip_address , &gai_error, &mmdb_error);
	
  if (0 != gai_error) {
    log_write(0, LOG_MAIN, US"geoipv6: Error from getaddrinfo ");
    *yield = string_copy(COUNTRY_CODE_LOOKUP_FAILED);

    /* fprintf(stderr, */
    /* 	  "\n  Error from getaddrinfo for %s - %s\n\n", */
    /* 	  ip_address, gai_strerror(gai_error)); */
    /* exit(2); */

    goto end;
  }
	
  if (MMDB_SUCCESS != mmdb_error) {
    log_write(0, LOG_MAIN, US"geoipv6: Error from libmaxminddb ");
    *yield = string_copy(COUNTRY_CODE_LOOKUP_FAILED);

    /* fprintf(stderr, */
    /* 	  "\n  Got an error from libmaxminddb: %s\n\n", */
    /* 	  MMDB_strerror(mmdb_error)); */
    /* exit(3); */

    goto end;
  }

  MMDB_entry_data_s entry_data;
	
  int exit_code = 0;
  if (result.found_entry) {
    int status =
      MMDB_get_value(&result.entry, &entry_data, "country", "iso_code", NULL);
	  
    if (MMDB_SUCCESS != status) {
      log_write(0, LOG_MAIN, US"geoipv6: Error looking up the entry data ");
      *yield = string_copy(COUNTRY_CODE_LOOKUP_FAILED);

      /* fprintf( */
      /* 	    stderr, */
      /* 	    "Got an error looking up the entry data - %s\n", */
      /* 	    MMDB_strerror(status)); */
      /* exit_code = 4; */

      goto end;
    }
	  
    if (NULL != &entry_data) {
      if (MMDB_DATA_TYPE_UTF8_STRING == entry_data.type) {
	*yield = string_copyn((uschar *) entry_data.utf8_string, entry_data.data_size);
      } else {
	log_write(0, LOG_MAIN, US"geoipv6: Error unexpected data type ");
	*yield = string_copy(COUNTRY_CODE_LOOKUP_FAILED);
      }

      goto end;
    }
	  
  } else {
    log_write(0, LOG_MAIN, US"geoipv6: No data for the IP address ");
    *yield = string_copy(COUNTRY_CODE_LOOKUP_FAILED);

    /* fprintf( */
    /* 	  stderr, */
    /* 	  "\n  No entry for this IP address (%s) was found\n\n", */
    /* 	  ip_address); */
    /* exit_code = 5; */
	  
    goto end;
  }
    
 end:
  MMDB_close(&mmdb);
  return OK;
	
}

/*****************************************************************************
 * eof
 *****************************************************************************/
