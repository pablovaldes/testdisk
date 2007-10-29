/*

    File: fnctdsk.h

    Copyright (C) 1998-2007 Christophe GRENIER <grenier@cgsecurity.org>
  
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
unsigned long int C_H_S2LBA(const disk_t *disk_car,const unsigned int C, const unsigned int H, const unsigned int S);
uint64_t CHS2offset(const disk_t *disk_car,const CHS_t*CHS);
uint64_t C_H_S2offset(const disk_t *disk_car,const unsigned int C, const unsigned int H, const unsigned int S);
unsigned int offset2sector(const disk_t *disk_car, const uint64_t offset);
unsigned int offset2head(const disk_t *disk_car, const uint64_t offset);
unsigned int offset2cylinder(const disk_t *disk_car, const uint64_t offset);
void offset2CHS(const disk_t *disk_car,const uint64_t offset, CHS_t*CHS);

void dup_CHS(CHS_t * CHS_dest, const CHS_t * CHS_source);
list_part_t *element_new(partition_t *part);
list_disk_t *insert_new_disk(list_disk_t *list_disk, disk_t *disk_car);
list_part_t *insert_new_partition(list_part_t *list_part, partition_t *part, const int force_insert, int *insert_error);
list_part_t *remove_partition(list_part_t *list_part, list_part_t *element2removed);
list_part_t *sort_partition_list(list_part_t *list_part);
list_part_t *gen_sorted_partition_list(const list_part_t *list_part);
void part_free_list(list_part_t *list_part);
void part_free_list_only(list_part_t *list_part);
void  partition_reset(partition_t *partition, const arch_fnct_t *arch);
partition_t *partition_new(const arch_fnct_t *arch);
int check_list_part(list_part_t *list_part);
unsigned int get_geometry_from_list_part(const disk_t *disk_car, const list_part_t *list_part, const int verbose);
int delete_list_disk(list_disk_t *list_disk);
const char *size_to_unit(uint64_t disk_size, char *buffer);
int is_part_overlapping(const list_part_t *list_part);

