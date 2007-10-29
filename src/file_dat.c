/*

    File: file_dat.c

    Copyright (C) 2007 Christophe GRENIER <grenier@cgsecurity.org>
  
    This software is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
  
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
  
    You should have received a copy of the GNU General Public License along
    with this program; if not, write the Free Software Foundation, Inc., 51
    Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <stdio.h>
#include "types.h"
#include "filegen.h"

static void register_header_check_dat(file_stat_t *file_stat);
static int header_check_dat(const unsigned char *buffer, const unsigned int buffer_size, const unsigned int safe_header_only, const file_recovery_t *file_recovery, file_recovery_t *file_recovery_new);

const file_hint_t file_hint_dat= {
  .extension="dat",
  .description="Glavna Knjiga account data",
  .min_header_distance=0,
  .max_filesize=2*1024*1024,
  .recover=1,
  .header_check=&header_check_dat,
  .register_header_check=&register_header_check_dat
};

static const unsigned char dat_header[8]= {0x30, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static void register_header_check_dat(file_stat_t *file_stat)
{
  register_header_check(0, dat_header,sizeof(dat_header), &header_check_dat, file_stat);
}

static int header_check_dat(const unsigned char *buffer, const unsigned int buffer_size, const unsigned int safe_header_only, const file_recovery_t *file_recovery, file_recovery_t *file_recovery_new)
{
  if(memcmp(buffer,dat_header,sizeof(dat_header))==0)
  {
    reset_file_recovery(file_recovery_new);
    file_recovery_new->extension=file_hint_dat.extension;
    return 1;
  }
  return 0;
}
