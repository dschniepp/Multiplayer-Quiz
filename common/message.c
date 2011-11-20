#include "common/message.h"
#include "common/socket.h"
/*
#define NET_TYPE_MESSAGE 1

#pragma pack(1)
struct NET_HEADER {
	uint16_t size; // Total size in byte 
	uint8_t type;  // predefined type 
};
struct LOGIN_REQUEST {
        struct NET_HEADER h;
        char name[31];
};
struct LOGIN_RESPONSE_OK {
        struct NET_HEADER h;
        uint8_t clientID;
};
#pragma pack(0)
*/
/*
struct NET_MESSAGE {
	struct NET_HEADER h; // common first data always is HEADER 
	char text[10];
};
*/

void prepare_message(void * hdr, int type, int size)
{
	struct GB_NET_HEADER* phdr = (struct GB_NET_HEADER*)hdr;

	phdr->size = htons((uint16_t)size);
	phdr->type = (uint8_t)type;
}

/*void write_message(int sock, int type)
{
        //struct NET_HEADER* phdr = (struct NET_HEADER*)hdr;

	//phdr->length = htons((uint16_t)size);
//	phdr->type = (uint8_t)type;
        
        switch(type){
        
            case 1: {
                        struct LOGIN_REQUEST pstr;
                        pstr.h.type = (uint8_t)GB_LR.h.length;
                        pstr.h.length = htons((uint16_t)GB_LR.h.length);
                        strcpy(pstr.name, GB_LR.name);
                        write(sock, &pstr, sizeof(pstr));
                        break;
            }
            case 2: {
                
                break;
            }
            case 3: {
                
                break;
            }
            case 4: {
                
                break;
            }
            case 5: {
                
                break;
            }
            case 6: {
                
                break;
            }
            case 7: {
                
                break;
            }
            case 8: {
                
                break;
            }
            case 9: {
                
                break;
            }
            case 10: {
                
                break;
            }
            case 11: {
                
                break;
            }
            case 12: {
                
                break;
            }
            case 255: {
                
                break;
            }
            default: {
                
                break;
            }
        
        }
        

}*/
/*
void print_message(void * hdr)
{
	struct GB_NET_HEADER* phdr = (struct GB_NET_HEADER*)hdr;

	printf("message has %d byte\n", ntohs(phdr->size));

	switch(phdr->type) {
	case TYPE_LROK: {
			struct GB_LOGIN_RESPONSE_OK *LROK = (struct GB_LOGIN_RESPONSE_OK*)phdr;
			printf("Client_id: %d\n", LROK->client_id);
			break;
		}
	default:
		printf("Unknown type: %d\n", phdr->type);
	}
}
*/
/*
int main(int argc, char ** argv)
{
	struct NET_MESSAGE message;
	strcpy(message.text, "Hallo");

	prepare_message(&message, NET_TYPE_MESSAGE, sizeof(message));
	print_message(&message);

	return 0;
}

*/