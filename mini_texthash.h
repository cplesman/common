#pragma once

#ifdef __cplusplus
extern "C" {
#endif



	struct t_mini_texthashitem {
		union {
			struct {
				unsigned char m_namei;
				unsigned char m_nlen;
				unsigned char m_datauc;
				unsigned char m_nexti;
			};
			unsigned long m_wholestruct;
		};
	};
	typedef struct t_mini_texthashitem minitexthashitem;

	struct t_mini_texthashtable {
		char *m_textlist; //max 256 len
		minitexthashitem m_table[16]; //minimum 16, maximum is 256
		//minitexthashitem m_moreitems[];  
	};
	typedef struct t_mini_texthashtable minitexthashtable;

#define MINITEXTHASHKEY(smalltext,len)	(((smalltext)[0]+((len>1)?((smalltext)[1]<<2):0))&0xf)
	unsigned char FindMiniHashItem(minitexthashtable *p_table, char *p_name, int p_len);
	unsigned char FindMiniHashItem_Next(minitexthashtable *p_table, unsigned char p_last);

#ifdef __cplusplus
}
#endif

