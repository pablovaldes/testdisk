/*

    File: ufs.c

    Copyright (C) 2005-2007 Christophe GRENIER <grenier@cgsecurity.org>
  
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
 
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include "types.h"
#include "common.h"
#include "ufs.h"
#include "fnctdsk.h"
#include "log.h"

static int set_ufs_info(const disk_t *disk_car, const struct ufs_super_block *sb,partition_t *partition, const int verbose, const int dump_ind);
static int test_ufs(const disk_t *disk_car, const struct ufs_super_block *sb,partition_t *partition,const int verbose, const int dump_ind);

int check_ufs(disk_t *disk_car,partition_t *partition,const int verbose)
{
  const struct ufs_super_block*sb;
  unsigned char *buffer;
  buffer=(unsigned char*)MALLOC(UFS_SUPERBLOCK_SIZE);
  sb=(const struct ufs_super_block*)buffer;
  if(disk_car->read(disk_car,UFS_SUPERBLOCK_SIZE, buffer, partition->part_offset+UFS_SBLOCK)!=0)
  {
    free(buffer);
    return 1;
  }
  if(test_ufs(disk_car,sb,partition,verbose,0)!=0)
  {
    free(buffer);
    return 1;
  }
  set_ufs_info(disk_car,sb,partition,verbose,0);
  free(buffer);
  return 0;
}

static int test_ufs(const disk_t *disk_car, const struct ufs_super_block *sb,partition_t *partition,const int verbose, const int dump_ind)
{
  if(le32(sb->fs_magic)==UFS_MAGIC && le32(sb->fs_size) > 0 &&
    (le32(sb->fs_fsize)==512 || le32(sb->fs_fsize)==1024 || le32(sb->fs_fsize)==2048 || le32(sb->fs_fsize)==4096))
  {
    partition->upart_type = UP_UFS;
    if(verbose>1)
      log_info("\nUFS Marker at %u/%u/%u\n", offset2cylinder(disk_car,partition->part_offset),offset2head(disk_car,partition->part_offset),offset2sector(disk_car,partition->part_offset));
    return 0;
  }
  if(be32(sb->fs_magic)==UFS_MAGIC && be32(sb->fs_size) > 0 &&
    (be32(sb->fs_fsize)==512 || be32(sb->fs_fsize)==1024 || be32(sb->fs_fsize)==2048 || be32(sb->fs_fsize)==4096))
  {
    partition->upart_type = UP_UFS;
    if(verbose>1)
      log_info("\nUFS Marker at %u/%u/%u\n", offset2cylinder(disk_car,partition->part_offset),offset2head(disk_car,partition->part_offset),offset2sector(disk_car,partition->part_offset));
    return 0;
  }
  if(le32(sb->fs_magic)==UFS2_MAGIC && le64(sb->fs_u11.fs_u2.fs_size) > 0 &&
    (le32(sb->fs_fsize)==512 || le32(sb->fs_fsize)==1024 || le32(sb->fs_fsize)==2048 || le32(sb->fs_fsize)==4096))
  {
    partition->upart_type = UP_UFS2;
    if(verbose>1)
      log_info("\nUFS2 Marker at %u/%u/%u\n", offset2cylinder(disk_car,partition->part_offset),offset2head(disk_car,partition->part_offset),offset2sector(disk_car,partition->part_offset));
    return 0;
  }
  if(be32(sb->fs_magic)==UFS2_MAGIC && be64(sb->fs_u11.fs_u2.fs_size) > 0 &&
    (be32(sb->fs_fsize)==512 || be32(sb->fs_fsize)==1024 || be32(sb->fs_fsize)==2048 || be32(sb->fs_fsize)==4096))
  {
    partition->upart_type = UP_UFS2;
    if(verbose>1)
      log_info("\nUFS2 Marker at %u/%u/%u\n", offset2cylinder(disk_car,partition->part_offset),offset2head(disk_car,partition->part_offset),offset2sector(disk_car,partition->part_offset));
    return 0;
  }
  return 1;
}

int recover_ufs(disk_t *disk_car, const struct ufs_super_block *sb, partition_t *partition,const int verbose, const int dump_ind)
{
  if(test_ufs(disk_car,sb,partition,verbose,dump_ind)!=0)
    return 1;
  if(dump_ind!=0)
  {
    log_info("recover_ufs\n");
    dump_log(sb,sizeof(*sb));
  }
  switch(le32(sb->fs_magic))
  {
    case UFS_MAGIC:
      partition->part_size = (uint64_t)le32(sb->fs_size)*le32(sb->fs_fsize);
      if(verbose>1)
      {
	log_info("fs_size %lu, fs_fsize %lu\n", (long unsigned)le32(sb->fs_size), (long unsigned)le32(sb->fs_fsize));
	log_info("fs_sblkno %lu\n", (long unsigned)le32(sb->fs_sblkno));
      }
      break;
    case UFS2_MAGIC:
      partition->part_size = (uint64_t)le64(sb->fs_u11.fs_u2.fs_size)*le32(sb->fs_fsize);
      if(verbose>1)
      {
	log_info("fs_size %lu, fs_fsize %lu\n", (long unsigned)le64(sb->fs_u11.fs_u2.fs_size), (long unsigned)le32(sb->fs_fsize));
	log_info("fs_sblkno %lu\n", (long unsigned)le32(sb->fs_sblkno));
	log_info("fs_sblockloc %llu\n", (long long unsigned)le64(sb->fs_u11.fs_u2.fs_sblockloc));
      }
      break;
  }
  switch(be32(sb->fs_magic))
  {
    case UFS_MAGIC:
      partition->part_size = (uint64_t)be32(sb->fs_size)*be32(sb->fs_fsize);
      if(verbose>1)
      {
	log_info("fs_size %lu, fs_fsize %lu\n",(long unsigned)be32(sb->fs_size),(long unsigned)be32(sb->fs_fsize));
      }
      break;
    case UFS2_MAGIC:
      partition->part_size = (uint64_t)be64(sb->fs_u11.fs_u2.fs_size)*be32(sb->fs_fsize);
      if(verbose>1)
      {
	log_info("fs_size %lu, fs_fsize %lu\n", (long unsigned)be64(sb->fs_u11.fs_u2.fs_size), (long unsigned)be32(sb->fs_fsize));
      }
      break;
  }

  set_ufs_info(disk_car,sb,partition,verbose,dump_ind);
  if(strcmp(partition->fsname,"/")==0)
  {
    partition->part_type_sun = (unsigned char)PSUN_ROOT;
    partition->part_type_gpt=GPT_ENT_TYPE_SOLARIS_ROOT;
  }
  else if(strcmp(partition->fsname,"/var")==0)
  {
    partition->part_type_sun = (unsigned char)PSUN_VAR;
    partition->part_type_gpt=GPT_ENT_TYPE_SOLARIS_VAR;
  }
  else if(strcmp(partition->fsname,"/usr")==0)
  {
    partition->part_type_sun = (unsigned char)PSUN_USR;
    partition->part_type_gpt=GPT_ENT_TYPE_SOLARIS_USR;
  }
  else if(strcmp(partition->fsname,"/export/home")==0)
  {
    partition->part_type_sun = (unsigned char)PSUN_HOME;
    partition->part_type_gpt=GPT_ENT_TYPE_SOLARIS_HOME;
  }
  else
  {
    partition->part_type_sun = (unsigned char)PSUN_ROOT;
    partition->part_type_gpt=GPT_ENT_TYPE_SOLARIS_HOME;
  }
  return 0;
}

static int set_ufs_info(const disk_t *disk_car,const struct ufs_super_block *sb,partition_t *partition, const int verbose, const int dump_ind)
{
  partition->fsname[0]='\0';
  partition->info[0]='\0';
  switch(partition->upart_type)
  {
    case UP_UFS:
    default:
      set_part_name(partition,(const char*)sb->fs_u11.fs_u1.fs_fsmnt,sizeof(partition->fsname));
      strncpy(partition->info,"UFS1",sizeof(partition->info));
      break;
    case UP_UFS2:
      set_part_name(partition,(const char*)sb->fs_u11.fs_u2.fs_fsmnt,sizeof(partition->fsname));
      strncpy(partition->info,"UFS2",sizeof(partition->info));
      break;
  }
  return 0;
}
