
#include "ddns_util.h"

int DDNS_base64_enc(char *in_str,char *out_str)
{
	// BASE64
	const char __base64_pad = '=';
	const char __base64_table[] =
	{
		'A',	'B',	'C',	'D',	'E',	'F',	'G',	'H',	'I',	'J',	'K',	'L',	'M',
		'N',	'O',	'P',	'Q',	'R',	'S',	'T',	'U',	'V',	'W',	'X',	'Y',	'Z',
		'a',	'b',	'c',	'd',	'e',	'f',	'g',	'h',	'i',	'j',	'k',	'l',	'm',
		'n',	'o',	'p',	'q',	'r',	's',	't',	'u',	'v',	'w',	'x',	'y',	'z',
		'0',	'1',	'2',	'3',	'4',	'5',	'6',	'7',	'8',	'9',	'+',	'/',	'\0',
	};

	const char *current = in_str;

	int i = 0;
	char *result = out_str;
	int length = strlen(in_str);

	while (length > 2)
	{
		// keep going until we have less than 24 bits
		result[i++] = __base64_table[current[0] >> 2];
		result[i++] = __base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
		result[i++] = __base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
		result[i++] = __base64_table[current[2] & 0x3f];

		current += 3;
		length -= 3; /* we just handle 3 octets of data */
	}

	// now deal with the tail end of things
	if (length != 0)
	{
		result[i++] = __base64_table[current[0] >> 2];

		if (length > 1)
		{
			result[i++] = __base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
			result[i++] = __base64_table[(current[1] & 0x0f) << 2];
			result[i++] = __base64_pad;
		}
		else
		{
			result[i++] = __base64_table[(current[0] & 0x03) << 4];
			result[i++] = __base64_pad;
			result[i++] = __base64_pad;
		}
	}

	result[i] = '\0';

	return i;
}

int DDNS_get_host(const char* domain, char* host)
{
	struct hostent* hostinfo;
	struct in_addr* addr;
	hostinfo = gethostbyname(domain);

	if(!hostinfo){
		return -1;
	}
	if(hostinfo->h_addrtype != AF_INET){
		return -1;
	}
	addr = (struct in_addr*)*hostinfo->h_addr_list;
	strcpy(host, inet_ntoa(*addr));
	return 0;
}




