#ifndef __BFIN_SDH_H__
#define __BFIN_SDH_H__
struct mmc_cid {
	unsigned long psn;
	unsigned short oid;
	unsigned char mid;
	unsigned char prv;
	unsigned char mdt;
	char pnm[7];
};

struct mmc_csd {
	u8	csd_structure:2,
		spec_vers : 4,
		rsvd1 : 2;
	u8	taac;
	u8	nsac;
	u8	tran_speed;
	u16	ccc:12,
		read_bl_len : 4;
	u64	read_bl_partial:1,
		write_blk_misalign : 1,
		read_blk_misalign : 1,
		dsr_imp : 1,
		rsvd2 : 2,
		c_size : 12,
		vdd_r_curr_min : 3,
		vdd_r_curr_max : 3,
		vdd_w_curr_min : 3,
		vdd_w_curr_max : 3,
		c_size_mult : 3,
		sector_size : 5,
		erase_grp_size : 5,
		wp_grp_size : 5,
		wp_grp_enable : 1,
		default_ecc : 2,
		r2w_factor : 3,
		write_bl_len : 4,
		write_bl_partial : 1,
		rsvd3 : 5;
	u8	file_format_grp:1,
		copy : 1,
		perm_write_protect : 1,
		tmp_write_protect : 1,
		file_format : 2,
		ecc : 2;
	u8	crc:7;
	u8	one:1;
};


#endif
