#include "mini_texthash.h"

unsigned char FindMiniHashItem(minitexthashtable *p_table, char *p_name, int p_len) {
	//static const minitexthashitem nullitem = { 0,0,0,0 };
	int k = MINITEXTHASHKEY(p_name, p_len);
	if (p_table->m_table[k].m_nlen == 0) {
		return 0xff;
	}
	do {
		minitexthashitem ti = p_table->m_table[k];
		if (ti.m_nlen == p_len) {
			char *name = p_table->m_textlist + ti.m_namei;
			int i;
			for (i = 0; i < p_len; i++) {
				if (name[i] != p_name[i]) {
					break;
				}
			}
			if (i == p_len)
				return k;
		}
		k = (long)(unsigned long)ti.m_nexti;
	} while (k != 0xff);
	return 0xff;
}

unsigned char FindMiniHashItem_Next(minitexthashtable *p_table, unsigned char p_last) {
	//static const minitexthashitem nullitem = { 0,0,0,0 };
	unsigned char k = p_last;
	minitexthashitem ti = p_table->m_table[k];
	unsigned char p_namei = p_table->m_textlist + ti.m_namei;
	unsigned char p_len = ti.m_nlen;
	if (ti.m_nexti == 0xff) {
		return 0xff; //no others
	}
	k = (long)(unsigned short)ti.m_nexti;
	do {
		ti = p_table->m_table[k];
		if (ti.m_nlen == p_len && ti.m_namei == p_namei) {
			//there from the same table so they should have the same name index
			return k;
		}
		k = (int)(unsigned int)ti.m_nexti;
	} while (k != 0xff);
	return 0xff;
}

