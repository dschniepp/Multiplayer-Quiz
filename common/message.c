#include "message.h"

#define NET_TYPE_MESSAGE 1

#pragma pack(1)
struct NET_HEADER {
	uint16_t length; /* Total size in byte */
	uint8_t type;  /* predefined type */
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

/*
struct NET_MESSAGE {
	struct NET_HEADER h; // common first data always is HEADER 
	char text[10];
};
*/

void prepare_message(void* hdr , int type)
{
        //struct NET_HEADER* phdr = (struct NET_HEADER*)hdr;

	//phdr->length = htons((uint16_t)size);
//	phdr->type = (uint8_t)type;
        
        switch(type){
        
            case 1: {
                        
                        struct LOGIN_REQUEST pstr;
                                
                
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
        

}

void print_message(void * hdr)
{
	struct NET_HEADER* phdr = (struct NET_HEADER*)hdr;

	printf("message has %d byte\n", ntohs(phdr->length));

	switch(phdr->type) {
	case NET_TYPE_MESSAGE: {
			struct NET_MESSAGE *msg = (struct NET_MESSAGE*)phdr;
			printf("text: %s\n", msg->text);
			break;
		}
	default:
		printf("Unknown type: %d\n", phdr->type);
	}
}

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